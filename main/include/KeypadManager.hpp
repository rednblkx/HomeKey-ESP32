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
 * Scans a 5x3 key matrix, buffers entered digits, and reports completed codes
 * (submit key '#') or doorbell presses ('&') through callbacks. The '*' key
 * clears the buffer, and entry times out after CODE_ENTRY_TIMEOUT_US of
 * inactivity.
 */
class KeypadManager {
public:
    KeypadManager() = default;
    ~KeypadManager();

    KeypadManager(const KeypadManager&) = delete;
    KeypadManager& operator=(const KeypadManager&) = delete;

    /**
     * @brief Initializes the keypad GPIOs from config and registers the key callback.
     * @param config Keypad-related misc configuration (pins, active level, debounce).
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

    std::string code_buffer;
    esp_timer_handle_t entry_timer = nullptr;
    std::function<void(const std::string&)> on_code_entered;
    std::function<void()> on_db_pressed;

    static const char keymaps[5][3];

    void clear_buffer();
    void handle_key_press(char key);

    static void keyboard_cb_wrapper(keyboard_btn_handle_t handle, keyboard_btn_report_t report, void *user_data);
    static void entry_timeout_callback(void *arg);
};
