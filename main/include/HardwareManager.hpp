#pragma once
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <cstdint>

class ConfigManager;
class Pixel;
namespace espConfig { struct actions_config_t; struct misc_config_t; }

/**
 * @class HardwareManager
 * @brief Manages all low-level hardware interactions (GPIO, NeoPixels, etc.).
 *
 * This class provides a high-level API for controlling physical hardware. It abstracts
 * away pin numbers, active high/low logic, and the implementation details of
 * providing user feedback (e.g., blinking an LED or lighting a NeoPixel).
 */
class HardwareManager {
public:
    /**
     * @brief Constructs the HardwareManager.
     * @param configManager Reference to the application configuration manager.
     */
    HardwareManager(const espConfig::misc_config_t &, const espConfig::actions_config_t &);

    /**
     * @brief Initializes all hardware pins and starts background tasks for feedback.
     */
    void begin();

    /**
     * @brief Sets the physical output for the lock mechanism (e.g., relay or motor driver).
     * @param state The desired lock state (e.g., lockStates::LOCKED).
     */
    void setLockOutput(int state);

    /**
     * @brief Checks the door sensor state with debouncing.
     * @return 1 for Closed, 0 for Open, -1 for no change/invalid.
     */
    int checkDoorSensor();

    /**
     * @brief Triggers a non-blocking visual/audible signal for a successful action.
     */
    void showSuccessFeedback();

    /**
     * @brief Triggers a non-blocking visual/audible signal for a failed action.
     */
    void showFailureFeedback();

    /**
     * @brief Triggers the alternative GPIO pin action.
     */
    void triggerAltAction();

private:
    /**
     * @enum FeedbackType
     * @brief Defines the types of feedback that can be requested.
     */
    enum class FeedbackType {
        SUCCESS,
        FAILURE,
        TAG_EVENT
    };

    enum class TimerSources : uint8_t {
      GPIO_S,
      GPIO_F,
      TAG_EVENT,
      PIXEL_S,
      PIXEL_F,
      PIXEL_TAG_EVENT,
      ALT_GPIO,
      ALT_GPIO_INIT
    };

    struct TimerContext {
        HardwareManager* hw_manager;
        TimerSources timer_source;
    };

    // --- FreeRTOS Task Management ---
    static void feedbackTaskEntry(void* instance);
    void feedbackTask();

    static void lockControlTaskEntry(void* instance);
    void lockControlTask();

    static void initiator_task_entry(void* arg);
    void initiator_task();
    static void IRAM_ATTR initiator_isr_handler(void* arg);

    static void handleTimer(void *instance);
    
    // --- Member Variables ---
    const espConfig::misc_config_t& m_sysConfig;
    const espConfig::actions_config_t& m_miscConfig;

    Pixel* m_pixel = nullptr;
  
    int m_lastDoorState = -1;
    int m_lastRawDoorState = -1;
    uint32_t m_lastDoorChangeTime = 0;
    static const uint32_t DEBOUNCE_DELAY = 50;

    esp_timer_handle_t m_gpioSuccessTimer;
    esp_timer_handle_t m_gpioFailTimer;
    esp_timer_handle_t m_tagEventTimer;
    esp_timer_handle_t m_pixelSuccessTimer;
    esp_timer_handle_t m_pixelFailTimer;
    esp_timer_handle_t m_pixelTagEventTimer;
    esp_timer_handle_t m_altActionTimer;
    esp_timer_handle_t m_altActionInitTimer;

    TaskHandle_t m_feedbackTaskHandle;
    QueueHandle_t m_feedbackQueue;

    TaskHandle_t m_lockControlTaskHandle;
    QueueHandle_t m_lockControlQueue;

    TaskHandle_t m_initiatorTaskHandle;
    QueueHandle_t m_initiatorQueue;



    bool m_altActionArmed = false;
    
    static const char* TAG;
};

