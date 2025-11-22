#include <event_manager.hpp>
#include "HardwareManager.hpp"
#include "LockManager.hpp"
#include "Pixel.h"
#include "config.hpp"
#include "esp_timer.h"
#include "eventStructs.hpp"

const char* HardwareManager::TAG = "HardwareManager";

const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };

/**
 * @brief Construct a HardwareManager and register its event handlers and publishers.
 *
 * Initializes internal state from the provided misc configuration and registers event subscribers
 * for "lock/action", "nfc/event", and "hardware/gpioPinChanged". Also creates publishers for
 * "lock/updateState" and "lock/altAction". Task and queue handles are initialized to nullptr.
 *
 * The registered subscribers:
 * - "lock/action": deserializes an EventLockState and applies its targetState via setLockOutput.
 * - "nfc/event": handles HOMEKEY_TAP and TAG_TAP events, triggering success/failure feedback and
 *   potentially the alternate action.
 * - "hardware/gpioPinChanged": updates pin configuration when GPIO ownership changes (resets old
 *   pin, disables pull-up, configures new pin as output and restores state for gpioActionPin).
 *
 * @param miscConfig Configuration values controlling GPIO pins, NeoPixel type/behavior, and
 *                   timing used by the HardwareManager.
 */
HardwareManager::HardwareManager(const espConfig::actions_config_t& miscConfig)
    : m_miscConfig(miscConfig),
      m_feedbackTaskHandle(nullptr),
      m_feedbackQueue(nullptr),
      m_lockControlTaskHandle(nullptr),
      m_lockControlQueue(nullptr)
{
  espp::EventManager::get().add_subscriber(
      "lock/action", "HardwareManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if(!ec) {
          setLockOutput(s.targetState);
        }
      }, 3072);
  espp::EventManager::get().add_subscriber("nfc/event", "HardwareManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    NfcEvent event = alpaca::deserialize<NfcEvent>(data, ec);
    if(ec) return;
    switch(event.type) {
      case HOMEKEY_TAP: {
        EventHKTap s = alpaca::deserialize<EventHKTap>(event.data, ec);
        if(!ec){
          if(s.status) {showSuccessFeedback();triggerAltAction();} else showFailureFeedback();
        }
        break;
      }
      case TAG_TAP: {
        EventTagTap s = alpaca::deserialize<EventTagTap>(event.data, ec);
        if(!ec){
          if (m_feedbackQueue != nullptr) {
              FeedbackType feedback = FeedbackType::TAG_EVENT;
              xQueueSend(m_feedbackQueue, &feedback, 0);
          }
        }
        break;
      }
      break;
      default:
        break;
    }
  }, 4096);
  espp::EventManager::get().add_subscriber(
      "hardware/gpioPinChanged", "HardwareManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventValueChanged s = alpaca::deserialize<EventValueChanged>(data, ec);
        if(!ec) {
          uint8_t state = 0;
          if(s.oldValue != 255 && s.oldValue < GPIO_NUM_MAX){
            state = gpio_get_level(gpio_num_t(s.oldValue));
            gpio_reset_pin(gpio_num_t(s.oldValue));
            gpio_pullup_dis(gpio_num_t(s.oldValue));
          }
          if(s.newValue != 255){    
            pinMode(s.newValue, OUTPUT);
            if(s.name == "gpioActionPin")
              digitalWrite(s.newValue, state);
          }
        }
      }, 3072);
  espp::EventManager::get().add_publisher("lock/updateState", "HardwareManager");
  espp::EventManager::get().add_publisher("lock/altAction", "HardwareManager");
}

/**
 * @brief Initialize hardware resources and runtime infrastructure based on misc configuration.
 *
 * Configures GPIO pins and initial output states for NFC indicators, action and alternate-action pins;
 * creates and configures the NeoPixel controller if present; installs the alternate-action ISR and
 * creates its initiator queue/task when configured; creates ESP timers for GPIO/pixel/alt-action sources;
 * and creates the feedback and lock-control queues and their associated tasks.
 *
 * This prepares the HardwareManager to receive events and perform timed feedback and lock control.
 */
void HardwareManager::begin() {
    ESP_LOGI(TAG, "Initializing hardware pins...");

    // --- Initialize GPIO Pins ---
    if (m_miscConfig.nfcSuccessPin != 255) {
        pinMode(m_miscConfig.nfcSuccessPin, OUTPUT);
        digitalWrite(m_miscConfig.nfcSuccessPin, !m_miscConfig.nfcSuccessHL);
    }
    if (m_miscConfig.nfcFailPin != 255) {
        pinMode(m_miscConfig.nfcFailPin, OUTPUT);
        digitalWrite(m_miscConfig.nfcFailPin, !m_miscConfig.nfcFailHL);
    }
    if (m_miscConfig.tagEventPin != 255) {
      pinMode(m_miscConfig.tagEventPin, OUTPUT);
      digitalWrite(m_miscConfig.tagEventPin, !m_miscConfig.tagEventHL);
    }
    if (m_miscConfig.gpioActionPin != 255) {
        pinMode(m_miscConfig.gpioActionPin, OUTPUT);
    }
    if (m_miscConfig.hkAltActionInitPin != 255) {
      pinMode(m_miscConfig.hkAltActionInitPin, INPUT_PULLUP);
      if (m_miscConfig.hkAltActionPin != 255) {
        pinMode(m_miscConfig.hkAltActionPin, OUTPUT);
      }
      if (m_miscConfig.hkAltActionInitLedPin != 255) {
        pinMode(m_miscConfig.hkAltActionInitLedPin, OUTPUT);
      }
      m_initiatorQueue = xQueueCreate(1, sizeof(uint8_t));
      xTaskCreateUniversal(initiator_task_entry, "initiator_task", 2048, this, 3, &m_initiatorTaskHandle, 1);
      gpio_install_isr_service(0);
      gpio_set_intr_type((gpio_num_t)m_miscConfig.hkAltActionInitPin, GPIO_INTR_NEGEDGE);
      gpio_isr_handler_add((gpio_num_t)m_miscConfig.hkAltActionInitPin, initiator_isr_handler, (void*) this);
    }

    // --- Initialize NeoPixel ---
    if (m_miscConfig.nfcNeopixelPin != 255) {
        size_t pixelTypeIndex = m_miscConfig.neoPixelType;
        if (pixelTypeIndex >= pixelTypeMap.size()) {
            ESP_LOGW(TAG, "Invalid NeoPixel type index (%d), defaulting to GRB.", pixelTypeIndex);
            pixelTypeIndex = 5; // GRB
        }
        m_pixel = new Pixel(m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
        m_pixel->off(); // Ensure pixel is off at startup
        ESP_LOGI(TAG, "NeoPixel initialized on pin %d with type %s.", m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
    }

    static TimerContext gpioS_context = {this, TimerSources::GPIO_S};
    const esp_timer_create_args_t gpioS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &gpioS_context,
            .name = "gpioSuccessTimer"
    };

    static TimerContext gpioF_context = {this, TimerSources::GPIO_F};
    const esp_timer_create_args_t gpioF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &gpioF_context,
            .name = "gpioFailTimer"
    };
    
    static TimerContext tagEvent_context = {this, TimerSources::TAG_EVENT};
    const esp_timer_create_args_t tagEvent_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &tagEvent_context,
            .name = "tagEventTimer"
    };
    static TimerContext pixelS_context = {this, TimerSources::PIXEL_S};
    const esp_timer_create_args_t pixelS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &pixelS_context,
            .name = "pixelSuccessTimer"
    };

    static TimerContext pixelF_context = {this, TimerSources::PIXEL_F};
    const esp_timer_create_args_t pixelF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &pixelF_context,
            .name = "pixelFailTimer"
    };

    static TimerContext pixelTagEvent_context = {this, TimerSources::PIXEL_TAG_EVENT};
    const esp_timer_create_args_t pixelTagEvent_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &pixelTagEvent_context,
            .name = "pixelTagEventTimer"
    };

    static TimerContext altAction_context = {this, TimerSources::ALT_GPIO};
    const esp_timer_create_args_t altAction_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &altAction_context,
            .name = "altActionTimer"
    };
  
    static TimerContext altActionInit_context = {this, TimerSources::ALT_GPIO_INIT};
    const esp_timer_create_args_t altActionInit_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &altActionInit_context,
            .name = "altActionInitTimer"
    };

    esp_timer_create(&gpioS_timer_args, &m_gpioSuccessTimer);
    esp_timer_create(&gpioF_timer_args, &m_gpioFailTimer);
    esp_timer_create(&tagEvent_timer_args, &m_tagEventTimer);
    esp_timer_create(&pixelS_timer_args, &m_pixelSuccessTimer);
    esp_timer_create(&pixelF_timer_args, &m_pixelFailTimer);
    esp_timer_create(&pixelTagEvent_timer_args, &m_pixelTagEventTimer);
    esp_timer_create(&altAction_timer_args, &m_altActionTimer);
    esp_timer_create(&altActionInit_timer_args, &m_altActionInitTimer);

    m_feedbackQueue = xQueueCreate(5, sizeof(FeedbackType));
    xTaskCreateUniversal(feedbackTaskEntry, "feedback_task", 4096, this, 3, &m_feedbackTaskHandle, 1);

    m_lockControlQueue = xQueueCreate(5, sizeof(int));
    xTaskCreateUniversal(lockControlTaskEntry, "lock_control_task", 4096, this, 3, &m_lockControlTaskHandle, 1);
    ESP_LOGI(TAG, "Hardware initialization complete.");
}

/**
 * @brief Enqueue a desired lock state for the lock-control task to apply.
 *
 * @param state Desired lock state (for example `LOCKED` or `UNLOCKED`) to be processed and applied by the manager.
 */

void HardwareManager::setLockOutput(int state) {
    if (m_lockControlQueue != nullptr) {
        xQueueSend(m_lockControlQueue, &state, pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Enqueue a success feedback event for the feedback task to process.
 *
 * If the internal feedback queue is not initialized, the call has no effect.
 */
void HardwareManager::showSuccessFeedback() {
    if (m_feedbackQueue != nullptr) {
        FeedbackType feedback = FeedbackType::SUCCESS;
        xQueueSend(m_feedbackQueue, &feedback, 0);
    }
}

/**
 * @brief Enqueues a failure feedback event for the feedback task.
 *
 * If the internal feedback queue has been created, posts a FAILURE event so the
 * feedback task will run the configured failure sequence (GPIOs/NeoPixel).
 */
void HardwareManager::showFailureFeedback() {
    if (m_feedbackQueue != nullptr) {
        FeedbackType feedback = FeedbackType::FAILURE;
        xQueueSend(m_feedbackQueue, &feedback, 0);
    }
}

/**
 * @brief Timer callback that handles configured hardware timeouts and applies the associated output changes.
 *
 * Processes the provided TimerContext and, based on its TimerSources value, performs one of:
 * - toggle the NFC success or failure GPIO,
 * - turn off the NeoPixel,
 * - toggle the alternate-action GPIO,
 * - clear the alt-action armed flag and turn off the alt-action init LED.
 *
 * @param arg Pointer to a TimerContext whose `hw_manager` and `timer_source` fields determine the target HardwareManager and the action to execute.
 */

void HardwareManager::handleTimer(void* arg){
  TimerContext* context = static_cast<TimerContext*>(arg);
  HardwareManager *i = context->hw_manager;
  TimerSources t = context->timer_source;

  switch (t) {
    case TimerSources::GPIO_S:
      digitalWrite(i->m_miscConfig.nfcSuccessPin, !i->m_miscConfig.nfcSuccessHL);
      ESP_LOGD(TAG, "GPIO_S");
      break;
    case TimerSources::GPIO_F:
      digitalWrite(i->m_miscConfig.nfcFailPin, !i->m_miscConfig.nfcFailHL);
      ESP_LOGD(TAG, "GPIO_F");
      break;
    case TimerSources::TAG_EVENT:
      digitalWrite(i->m_miscConfig.tagEventPin, !i->m_miscConfig.tagEventHL);
      ESP_LOGD(TAG, "TAG_EVENT");
      break;
    case TimerSources::PIXEL_S:
    case TimerSources::PIXEL_F:
    case TimerSources::PIXEL_TAG_EVENT:
      i->m_pixel->off();
      ESP_LOGD(TAG, "PIXEL");
      break;
    case TimerSources::ALT_GPIO:
      digitalWrite(i->m_miscConfig.hkAltActionPin, !i->m_miscConfig.hkAltActionGpioState);
      ESP_LOGD(TAG, "ALT_GPIO");
      break;
    case TimerSources::ALT_GPIO_INIT:
      i->m_altActionArmed = false;
      if (i->m_miscConfig.hkAltActionInitLedPin != 255) {
        digitalWrite(i->m_miscConfig.hkAltActionInitLedPin, LOW);
      }
      ESP_LOGD(TAG, "ALT_GPIO_INIT");
      break;
  }
}

/**
 * @brief Task entry that dispatches to a HardwareManager instance's initiator task.
 *
 * Casts the provided void pointer to a HardwareManager pointer and invokes its initiator_task().
 *
 * @param arg Pointer to the HardwareManager instance.
 */
void HardwareManager::initiator_task_entry(void* arg) {
    static_cast<HardwareManager*>(arg)->initiator_task();
}

/**
 * @brief ISR posted by the initiator GPIO; signals the initiator task by enqueuing a wake byte.
 *
 * Posts a single dummy byte to the HardwareManager's initiator queue from ISR context to wake
 * the initiator task without blocking.
 *
 * @param arg Pointer to the HardwareManager instance whose initiator queue will be signaled.
 */
void IRAM_ATTR HardwareManager::initiator_isr_handler(void* arg) {
    uint8_t dummy = 0;
    xQueueSendFromISR(static_cast<HardwareManager*>(arg)->m_initiatorQueue, &dummy, NULL);
}

/**
 * @brief Waits for initiator events and arms the alternate action window.
 *
 * This task blocks waiting for a byte posted to the initiator queue (typically from an ISR).
 * When an event is received and the alternate action is not already armed, it marks the
 * alternate action as armed, turns on the configured alt-action-init LED (if a valid pin is set),
 * and starts a one-shot timer that will disarm the initiation window after the configured timeout.
 *
 * Side effects:
 * - Sets `m_altActionArmed` to true.
 * - Writes HIGH to `m_miscConfig.hkAltActionInitLedPin` when that pin is not 255.
 * - Starts `m_altActionInitTimer` for `m_miscConfig.hkAltActionInitTimeout` milliseconds.
 */
void HardwareManager::initiator_task() {
    uint8_t dummy;
    while (true) {
        if (xQueueReceive(m_initiatorQueue, &dummy, portMAX_DELAY)) {
            if (!m_altActionArmed) {
                ESP_LOGI(TAG, "Alt action armed for %dms", m_miscConfig.hkAltActionInitTimeout);
                m_altActionArmed = true;
                if (m_miscConfig.hkAltActionInitLedPin != 255) {
                    digitalWrite(m_miscConfig.hkAltActionInitLedPin, HIGH);
                }

                esp_timer_start_once(m_altActionInitTimer, m_miscConfig.hkAltActionInitTimeout * 1000);
            }
        }
    }
}

/**
 * @brief Task entry wrapper that forwards to the HardwareManager instance's lockControlTask.
 *
 * @param instance Pointer to a HardwareManager instance (must not be null).
 */
void HardwareManager::lockControlTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->lockControlTask();
}

/**
 * @brief Processes incoming lock commands and applies them to hardware and system state.
 *
 * Continuously waits for lock state values from the internal lock-control queue. For each received
 * state, if a GPIO action pin is configured the function drives that pin to the configured lock
 * or unlock level and publishes an EventLockState with the received state as `currentState`,
 * `targetState` set to `UNKNOWN`, and `source` set to `INTERNAL` on the "lock/updateState" topic.
 * If no action pin is configured the received command is logged and ignored.
 */
void HardwareManager::lockControlTask() {
    int receivedState;
    while (true) {
        if (xQueueReceive(m_lockControlQueue, &receivedState, portMAX_DELAY)) {
          if (m_miscConfig.gpioActionPin == 255) {
              ESP_LOGI(TAG, "Received lock command but no action pin is configured.");
              continue;
          }
          
          ESP_LOGI(TAG, "Setting lock output for state: %d", receivedState);
          if (receivedState == LockManager::LOCKED) {
              digitalWrite(m_miscConfig.gpioActionPin, m_miscConfig.gpioActionLockState);
          } else if (receivedState == LockManager::UNLOCKED) {
              digitalWrite(m_miscConfig.gpioActionPin, m_miscConfig.gpioActionUnlockState);
          }
          EventLockState s{
            .currentState = static_cast<uint8_t>(receivedState),
            .targetState = LockManager::UNKNOWN,
            .source = LockManager::INTERNAL
          };
          std::vector<uint8_t> d;
          alpaca::serialize(s, d);
          espp::EventManager::get().publish("lock/updateState", d);
        }
    }
}

/**
 * @brief Triggers the configured alternate (home-key) action when armed.
 *
 * If the manager is armed, publishes the "lock/altAction" event. When an alternate-action GPIO is configured (pin != 255),
 * writes the configured GPIO state to that pin and starts the alt-action timer for the configured timeout.
 *
 * @note The timer is started for @c hkAltActionTimeout milliseconds.
 */
void HardwareManager::triggerAltAction() {
  if (m_altActionArmed) { 
      espp::EventManager::get().publish("lock/altAction", {});
      if (m_miscConfig.hkAltActionPin != 255) {
          ESP_LOGI(TAG, "Triggering alt action on pin %d for %dms", m_miscConfig.hkAltActionPin, m_miscConfig.hkAltActionTimeout);
          digitalWrite(m_miscConfig.hkAltActionPin, m_miscConfig.hkAltActionGpioState);
          esp_timer_start_once(m_altActionTimer, m_miscConfig.hkAltActionTimeout * 1000);
      }
  }
}

/**
 * @brief Starts the feedback task for the provided HardwareManager instance.
 *
 * This is a FreeRTOS task entry function that casts the opaque `instance` pointer
 * to `HardwareManager*` and invokes its feedback task routine.
 *
 * @param instance Pointer to a HardwareManager object (must be a valid `HardwareManager*`).
 */
void HardwareManager::feedbackTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->feedbackTask();
}

/**
 * @brief Processes queued feedback events and actuates configured hardware indicators.
 *
 * This task runs indefinitely, blocking on the internal feedback queue. For each
 * FeedbackType received it executes the corresponding sequence:
 * - FeedbackType::SUCCESS: stops conflicting timers, sets the NeoPixel to the
 *   configured success color and starts the pixel success timer, and drives
 *   the configured NFC success GPIO for the configured duration.
 * - FeedbackType::FAILURE: stops conflicting timers, sets the NeoPixel to the
 *   configured failure color and starts the pixel failure timer, and drives
 *   the configured NFC failure GPIO for the configured duration.
 *
 * GPIO pins configured with the sentinel value 255 are ignored. Timers and
 * NeoPixel behavior use durations and colors from the instance's misc configuration.
 */
void HardwareManager::feedbackTask() {
    FeedbackType feedback;
    while (true) {
        if (xQueueReceive(m_feedbackQueue, &feedback, portMAX_DELAY)) {
            switch (feedback) {
                case FeedbackType::SUCCESS:
                    ESP_LOGD(TAG, "Executing SUCCESS feedback sequence.");
                    if(esp_timer_is_active(m_gpioSuccessTimer)) esp_timer_stop(m_gpioSuccessTimer);
                    if(esp_timer_is_active(m_pixelSuccessTimer)) esp_timer_stop(m_gpioFailTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_miscConfig.neopixelSuccessColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelSuccessTimer, m_miscConfig.neopixelSuccessTime * 1000);
                    }
                    if (m_miscConfig.nfcSuccessPin != 255) {
                        digitalWrite(m_miscConfig.nfcSuccessPin, m_miscConfig.nfcSuccessHL);

                        esp_timer_start_once(m_gpioSuccessTimer, m_miscConfig.nfcSuccessTime * 1000);
                    }
                    break;

                case FeedbackType::FAILURE:
                    ESP_LOGD(TAG, "Executing FAILURE feedback sequence.");
                    if(esp_timer_is_active(m_gpioFailTimer)) esp_timer_stop(m_gpioFailTimer);
                    if(esp_timer_is_active(m_pixelFailTimer)) esp_timer_stop(m_pixelFailTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_miscConfig.neopixelFailureColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelFailTimer, m_miscConfig.neopixelFailTime * 1000);
                    }
                    if (m_miscConfig.nfcFailPin != 255) {
                        digitalWrite(m_miscConfig.nfcFailPin, m_miscConfig.nfcFailHL);

                        esp_timer_start_once(m_gpioFailTimer, m_miscConfig.nfcFailTime * 1000);
                    }
                    break;
                case FeedbackType::TAG_EVENT: 
                    ESP_LOGD(TAG, "Executing TAG_EVENT feedback sequence.");
                    if(esp_timer_is_active(m_tagEventTimer)) esp_timer_stop(m_tagEventTimer);
                    if(esp_timer_is_active(m_pixelTagEventTimer)) esp_timer_stop(m_pixelTagEventTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_miscConfig.neopixelTagEventColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelTagEventTimer, m_miscConfig.neopixelTagEventTime * 1000);
                    }
                    if (m_miscConfig.tagEventPin != 255) {
                        digitalWrite(m_miscConfig.tagEventPin, m_miscConfig.tagEventHL);

                        esp_timer_start_once(m_tagEventTimer, m_miscConfig.tagEventTimeout * 1000);
                    }
                    break;
            }
        }
    }
}
