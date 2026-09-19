#include "KeypadManager.hpp"
#include "GPIOAllocator.hpp"
#include "esp_log.h"

const char KeypadManager::keymaps[5][3] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {KEY_CLEAR, '0', KEY_ENTER}, {'\0', '&', '\0'} // Using \0 for unused keys
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

    for (auto pin : config.keypadRowPins) {
        out_gpios.push_back(pin);
    }
    for (auto pin : config.keypadColumnPins) {
        in_gpios.push_back(pin);
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

    ESP_LOGI(TAG, "Keypad initialized with %d output and %d input pins.", out_gpios.size(), in_gpios.size());
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
        case KEY_DB:
            ESP_LOGI(TAG, "DB key pressed.");
            if(on_db_pressed)
              on_db_pressed();
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
    char key_pressed = instance->keymaps[out_idx][in_idx];

    instance->handle_key_press(key_pressed);
}
