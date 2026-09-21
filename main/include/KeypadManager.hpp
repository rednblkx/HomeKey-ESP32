#pragma once

#include "esp_timer.h"
#include "keyboard_button.h"
#include "config.hpp"
#include <functional>
#include <string>

#define KEY_ENTER '#'
#define KEY_CLEAR '*'
#define KEY_DB '&'
#define CODE_ENTRY_TIMEOUT_US (5000 * 1000) // esp_timer uses microseconds

/**
 * @class KeypadManager
 * @brief Drives a GPIO matrix keypad via the espressif/keyboard_button component.
 *
 * Scans a key matrix (5x3 with doorbell row, or 4x4 with A-D), buffers entered
 * digits, and reports completed codes (submit key '#') or doorbell presses
 * through callbacks. The '*' key clears the buffer, and entry times out after
 * CODE_ENTRY_TIMEOUT_US of inactivity.
 */
class KeypadManager {
public:
    static constexpr uint8_t LAYOUT_5X3 = 0; // 5 rows x 3 cols + dedicated doorbell row
    static constexpr uint8_t LAYOUT_4X4 = 1; // 4 rows x 4 cols with A-D

    KeypadManager() = default;
    ~KeypadManager();

    KeypadManager(const KeypadManager&) = delete;
    KeypadManager& operator=(const KeypadManager&) = delete;

    /**
     * @brief Initializes the keypad GPIOs from config and registers the key callback.
     * @param config Keypad-related misc configuration (layout, pins, active level, debounce, doorbell key).
     * @return true on success, false if the keyboard_button instance could not be created.
     */
    bool begin(const espConfig::misc_config_t& config);

    /**
     * @brief Stops the scanning task and releases resources.
     */
    void end();

    /**
     * @brief Registers a callback invoked when a full code is entered.
     * @param callback The function to call with the buffered code.
     */
    void registerCodeEnteredCallback(std::function<void(const std::string&)> callback);

    /**
     * @brief Registers a callback invoked when the doorbell key is pressed.
     * @param callback The function to call.
     */
    void registerDoorbellCallback(std::function<void()> callback);

private:
    const char* TAG = "KeypadManager";
    keyboard_btn_handle_t kbd_handle = nullptr;
    std::vector<int> out_gpios;
    std::vector<int> in_gpios;

    char doorbell_key = '\0'; // Resolved doorbell key; '\0' when disabled
    const char* active_keymap = nullptr; // First element of the selected layout's keymap
    uint8_t keymap_rows = 0;
    uint8_t keymap_cols = 0;

    std::string code_buffer;
    esp_timer_handle_t entry_timer = nullptr;
    std::function<void(const std::string&)> on_code_entered;
    std::function<void()> on_db_pressed;

    static const char keymap_5x3[5][3];
    static const char keymap_4x4[4][4];

    void clear_buffer();
    void handle_key_press(char key);

    static void keyboard_cb_wrapper(keyboard_btn_handle_t handle, keyboard_btn_report_t report, void *user_data);
    static void entry_timeout_callback(void *arg);
};
