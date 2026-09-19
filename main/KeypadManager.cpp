#include "KeypadManager.hpp"
#include "GPIOAllocator.hpp"
#include "esp_log.h"

// 5 rows x 3 columns with a dedicated doorbell row. '\0' marks unused keys.
const char KeypadManager::keymap_5x3[5][3] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {KEY_CLEAR, '0', KEY_ENTER}, {'\0', KEY_DB, '\0'}
};

// 4 rows x 4 columns with A-D. The doorbell key defaults to 'A' but can be
// remapped to any of A-D via keypadDoorbellKey; a remapped key keeps its
// position here and is routed to the doorbell callback at press time.
const char KeypadManager::keymap_4x4[4][4] = {
    {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {KEY_CLEAR, '0', KEY_ENTER, 'D'}
};

KeypadManager::~KeypadManager() {
    end();
    if (entry_timer) {
        esp_timer_delete(entry_timer);
        entry_timer = nullptr;
    }
}

bool KeypadManager::begin(const espConfig::misc_config_t& config) {
    if (kbd_handle) {
        return true;
    }

    if (config.keypadLayout == LAYOUT_4X4) {
        active_keymap = &keymap_4x4[0][0];
        keymap_rows = 4;
        keymap_cols = 4;
        doorbell_key = config.keypadDoorbellKey == 0 ? 'A' : (char)config.keypadDoorbellKey;
    } else {
        active_keymap = &keymap_5x3[0][0];
        keymap_rows = 5;
        keymap_cols = 3;
        doorbell_key = config.keypadDoorbellKey == 255 ? '\0' : KEY_DB;
    }
    if (doorbell_key != '\0' && config.keypadLayout == LAYOUT_4X4) {
        bool present = false;
        for (uint8_t r = 0; r < keymap_rows && !present; r++) {
            for (uint8_t c = 0; c < keymap_cols; c++) {
                if (active_keymap[r * keymap_cols + c] == doorbell_key) { present = true; break; }
            }
        }
        if (!present) {
            ESP_LOGW(TAG, "Doorbell key '%c' not present on 4x4 layout, disabling doorbell.", doorbell_key);
            doorbell_key = '\0';
        }
    }

    // 255 marks an unused pin slot; skip those so only wired pins are claimed
    // and scanned. The 5x3 layout consumes all 5 row / 3 column pins, the 4x4
    // layout the first 4 of each.
    uint8_t row_count = config.keypadLayout == LAYOUT_4X4 ? 4 : 5;
    uint8_t col_count = config.keypadLayout == LAYOUT_4X4 ? 4 : 3;
    for (uint8_t i = 0; i < row_count; i++) {
        if (config.keypadRowPins[i] == 255) continue;
        out_gpios.push_back(config.keypadRowPins[i]);
    }
    for (uint8_t i = 0; i < col_count; i++) {
        if (config.keypadColumnPins[i] == 255) continue;
        in_gpios.push_back(config.keypadColumnPins[i]);
    }

    for (auto pin : out_gpios) {
        auto lease = GPIOAllocator::instance().acquire(gpio_num_t(pin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut, GPIOAllocator::PinConsumer::Hardware, "KEYPAD_ROW");
        if (!lease) {
            ESP_LOGE(TAG, "Could not acquire keypad output pin %d, error: %d", pin, lease.error());
            out_gpios.clear();
            in_gpios.clear();
            return false;
        }
    }
    for (auto pin : in_gpios) {
        auto lease = GPIOAllocator::instance().acquire(gpio_num_t(pin), GPIO_MODE_INPUT, GPIOAllocator::PinRole::GpioIn, GPIOAllocator::PinConsumer::Hardware, "KEYPAD_COL");
        if (!lease) {
            ESP_LOGE(TAG, "Could not acquire keypad input pin %d, error: %d", pin, lease.error());
            out_gpios.clear();
            in_gpios.clear();
            return false;
        }
    }

    keyboard_btn_config_t cfg = {};
    cfg.output_gpios = out_gpios.data();
    cfg.input_gpios = in_gpios.data();
    cfg.output_gpio_num = out_gpios.size();
    cfg.input_gpio_num = in_gpios.size();
    cfg.active_level = config.keypadActiveLevel;
    cfg.debounce_ticks = config.keypadDebounceTicks;
    cfg.ticks_interval = 100 * 100; // 10ms scan interval

    esp_err_t err = keyboard_button_create(&cfg, &kbd_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create keyboard_button instance: %d", err);
        kbd_handle = nullptr;
        out_gpios.clear();
        in_gpios.clear();
        return false;
    }

    keyboard_btn_cb_config_t cb_cfg = {
        .event = KBD_EVENT_PRESSED,
        .callback = KeypadManager::keyboard_cb_wrapper,
        .user_data = this,
    };
    ESP_ERROR_CHECK(keyboard_button_register_cb(kbd_handle, cb_cfg, NULL));

    if (!entry_timer) {
        const esp_timer_create_args_t timer_args = {
            .callback = &entry_timeout_callback,
            .arg = this,
            .name = "keypad-timeout"
        };
        ESP_ERROR_CHECK(esp_timer_create(&timer_args, &entry_timer));
    }

    ESP_LOGI(TAG, "Keypad initialized: %ux%u layout, %d output and %d input pins, doorbell key '%c'.",
             keymap_cols, keymap_rows, out_gpios.size(), in_gpios.size(), doorbell_key ? doorbell_key : '-');
    return true;
}

void KeypadManager::end() {
    if (kbd_handle) {
        keyboard_button_delete(kbd_handle);
        kbd_handle = nullptr;
    }
    if (entry_timer && esp_timer_is_active(entry_timer)) {
        esp_timer_stop(entry_timer);
    }
    code_buffer.clear();
    out_gpios.clear();
    in_gpios.clear();
}

void KeypadManager::registerCodeEnteredCallback(std::function<void(const std::string&)> callback) {
    on_code_entered = callback;
}

void KeypadManager::registerDoorbellCallback(std::function<void()> callback) {
    on_db_pressed = callback;
}

// --- Private Methods ---

void KeypadManager::clear_buffer() {
    if (!code_buffer.empty()) {
        ESP_LOGI(TAG, "Clearing code buffer.");
        code_buffer.clear();
    }
    esp_timer_stop(entry_timer);
}

void KeypadManager::handle_key_press(char key) {
    if (key == '\0') return; // Ignore unmapped keys

    esp_timer_stop(entry_timer);
    esp_timer_start_once(entry_timer, CODE_ENTRY_TIMEOUT_US);

    if (key == doorbell_key) {
        ESP_LOGI(TAG, "DB key pressed.");
        if (on_db_pressed)
            on_db_pressed();
        return;
    }

    switch (key) {
        case KEY_CLEAR:
            ESP_LOGI(TAG, "Clear key pressed.");
            clear_buffer();
            break;

        case KEY_ENTER:
            ESP_LOGI(TAG, "Enter key pressed.");
            if (!code_buffer.empty() && on_code_entered) {
                on_code_entered(code_buffer);
            }
            clear_buffer(); // Clear buffer after submitting
            break;

        default: // A number was pressed
            code_buffer += key;
            ESP_LOGD(TAG, "Key pressed: %c, Buffer: [%s]", key, code_buffer.c_str());
            break;
    }
}

// --- Static Callbacks for C APIs ---

void KeypadManager::entry_timeout_callback(void *arg) {
    KeypadManager* instance = static_cast<KeypadManager*>(arg);
    if (instance) {
        instance->clear_buffer();
    }
}

void KeypadManager::keyboard_cb_wrapper(keyboard_btn_handle_t handle, keyboard_btn_report_t report, void *user_data) {
    KeypadManager* instance = static_cast<KeypadManager*>(user_data);
    if (!instance || report.key_pressed_num == 0) {
        return;
    }

    uint8_t out_idx = report.key_data[0].output_index;
    uint8_t in_idx = report.key_data[0].input_index;
    if (out_idx >= instance->keymap_rows || in_idx >= instance->keymap_cols) {
        return;
    }
    char key_pressed = instance->active_keymap[out_idx * instance->keymap_cols + in_idx];

    instance->handle_key_press(key_pressed);
}
