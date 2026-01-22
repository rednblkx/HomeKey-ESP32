#include "HardwareManager.hpp"
#include "LockManager.hpp"
#include "Pixel.h"
#include "config.hpp"
#include "esp_timer.h"
#include "eventStructs.hpp"

const char* HardwareManager::TAG = "HardwareManager";

const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };

static EventBus::Bus& event_bus = EventBus::Bus::instance();

/**
 * @brief Initialize HardwareManager state and register event topics and subscribers.
 *
 * Constructs the HardwareManager by storing the provided configuration, initializing task and
 * queue handles to nullptr, registering publishers for lock update and alternate-action topics,
 * and subscribing to hardware-related events. Subscribed handlers:
 * - apply lock actions by setting the lock output to the event's target state;
 * - reconfigure GPIO ownership when hardware configuration changes (reset old pin, disable pull-up,
 *   configure new pin as output and restore action pin state when applicable);
 * - handle NFC-related events to trigger success/failure feedback and the alternate action.
 *
 * @param actionsConfig Configuration values controlling GPIO pins, NeoPixel behavior, and timing.
 * @param miscConfig Configuration values for miscellaneous settings including door sensor.
 */
HardwareManager::HardwareManager(const espConfig::actions_config_t& actionsConfig, const espConfig::misc_config_t& miscConfig)
    : m_actionsConfig(actionsConfig),
      m_miscConfig(miscConfig),
      m_feedbackTaskHandle(nullptr),
      m_feedbackQueue(nullptr),
      m_lockControlTaskHandle(nullptr),
      m_lockControlQueue(nullptr),
      m_doorSensorTaskHandle(nullptr),
      m_doorSensorQueue(nullptr),
      m_hardware_action_topic(event_bus.register_topic(HARDWARE_ACTION_BUS_TOPIC)),
      m_alt_action_topic(event_bus.register_topic(HARDWARE_ALT_ACTION_BUS_TOPIC)),
      m_door_state_topic(event_bus.register_topic(DOOR_STATE_BUS_TOPIC))
{
  m_hardware_action_event = event_bus.subscribe(m_hardware_action_topic, [&](const EventBus::Event& event, void* context){
    if(event.payload_size == 0 || event.payload == nullptr) return;
    std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
    std::error_code ec;
    EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
    if(ec) { ESP_LOGE(TAG, "Failed to deserialize lock state event: %s", ec.message().c_str()); return; }
    ESP_LOGD(TAG, "Received action event: %d -> %d", s.currentState, s.targetState);
    setLockOutput(s.targetState);
  });
  m_gpio_pin_event = event_bus.subscribe(event_bus.register_topic(HARDWARE_CONFIG_BUS_TOPIC), [&](const EventBus::Event& event, void* context){
    if(event.payload_size == 0 || event.payload == nullptr) return;
    std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
    std::error_code ec;
    EventValueChanged s = alpaca::deserialize<EventValueChanged>(payload, ec);
    if(!ec) {
      ESP_LOGD(TAG, "Received hardware config event: %s -> %d (old=%d)", s.name.c_str(), s.newValue, s.oldValue);
      uint8_t state = 0;
      if(s.oldValue != 255 && GPIO_IS_VALID_OUTPUT_GPIO(s.oldValue)){
        state = gpio_get_level(gpio_num_t(s.oldValue));
        gpio_reset_pin(gpio_num_t(s.oldValue));
        gpio_pullup_dis(gpio_num_t(s.oldValue));
      } else ESP_LOGW(TAG, "Old GPIO %d is invalid", s.oldValue);
      if(s.newValue != 255 && GPIO_IS_VALID_OUTPUT_GPIO(s.newValue)){
        pinMode(s.newValue, OUTPUT);
        if(s.name == "gpioActionPin")
          digitalWrite(s.newValue, state);
      } else ESP_LOGW(TAG, "New GPIO %d is invalid", s.newValue);
    } else {
      ESP_LOGE(TAG, "Failed to deserialize hardware config event: %s", ec.message().c_str());
      return;
    }
  });
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
  m_nfc_event = event_bus.subscribe(event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), [&](const EventBus::Event& event, void* context){
    if(event.payload_size == 0 || event.payload == nullptr) return;
    std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
    std::error_code ec;
    NfcEvent nfc_event = alpaca::deserialize<NfcEvent>(payload, ec);
    if(ec) { ESP_LOGE(TAG, "Failed to deserialize NFC event: %s", ec.message().c_str()); return; }
    ESP_LOGD(TAG, "Received NFC event: %d", nfc_event.type);
    switch(nfc_event.type) {
      case HOMEKEY_TAP: {
        EventHKTap s = alpaca::deserialize<EventHKTap>(nfc_event.data, ec);
        if(!ec){
          if(s.status) {showSuccessFeedback();triggerAltAction();} else showFailureFeedback();
        } else {
          ESP_LOGE(TAG, "Failed to deserialize HomeKey event: %s", ec.message().c_str());
          return;
        }
      }
      break;
      case TAG_TAP: {
        EventTagTap s = alpaca::deserialize<EventTagTap>(nfc_event.data, ec);
        if(!ec){
          if (m_feedbackQueue != nullptr) {
              FeedbackType feedback = FeedbackType::TAG_EVENT;
              xQueueSend(m_feedbackQueue, &feedback, 0);
          }
        } else {
          ESP_LOGE(TAG, "Failed to deserialize Tag event: %s", ec.message().c_str());
          return;
        }
      }
      break;
      default:
      break;
    }
  });
    ESP_LOGI(TAG, "Initializing hardware pins...");

    // --- Initialize GPIO Pins ---
    if (m_actionsConfig.nfcSuccessPin != 255) {
        pinMode(m_actionsConfig.nfcSuccessPin, OUTPUT);
        digitalWrite(m_actionsConfig.nfcSuccessPin, !m_actionsConfig.nfcSuccessHL);
    }
    if (m_actionsConfig.nfcFailPin != 255) {
        pinMode(m_actionsConfig.nfcFailPin, OUTPUT);
        digitalWrite(m_actionsConfig.nfcFailPin, !m_actionsConfig.nfcFailHL);
    }
    if (m_actionsConfig.tagEventPin != 255) {
      pinMode(m_actionsConfig.tagEventPin, OUTPUT);
      digitalWrite(m_actionsConfig.tagEventPin, !m_actionsConfig.tagEventHL);
    }
    if (m_actionsConfig.gpioActionPin != 255) {
        pinMode(m_actionsConfig.gpioActionPin, OUTPUT);
    }
    if (m_actionsConfig.hkAltActionInitPin != 255) {
      pinMode(m_actionsConfig.hkAltActionInitPin, INPUT_PULLUP);
      if (m_actionsConfig.hkAltActionPin != 255) {
        pinMode(m_actionsConfig.hkAltActionPin, OUTPUT);
      }
      if (m_actionsConfig.hkAltActionInitLedPin != 255) {
        pinMode(m_actionsConfig.hkAltActionInitLedPin, OUTPUT);
      }
      m_initiatorQueue = xQueueCreate(1, sizeof(uint8_t));
      xTaskCreateUniversal(initiator_task_entry, "initiator_task", 2048, this, 3, &m_initiatorTaskHandle, 1);
      gpio_install_isr_service(0);
      gpio_set_intr_type((gpio_num_t)m_actionsConfig.hkAltActionInitPin, GPIO_INTR_NEGEDGE);
      gpio_isr_handler_add((gpio_num_t)m_actionsConfig.hkAltActionInitPin, initiator_isr_handler, (void*) this);
    }

    // --- Initialize NeoPixel ---
    if (m_actionsConfig.nfcNeopixelPin != 255) {
        size_t pixelTypeIndex = m_actionsConfig.neoPixelType;
        if (pixelTypeIndex >= pixelTypeMap.size()) {
            ESP_LOGW(TAG, "Invalid NeoPixel type index (%d), defaulting to GRB.", pixelTypeIndex);
            pixelTypeIndex = 5; // GRB
        }
        m_pixel = new Pixel(m_actionsConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
        m_pixel->off(); // Ensure pixel is off at startup
        ESP_LOGI(TAG, "NeoPixel initialized on pin %d with type %s.", m_actionsConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
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

    // --- Initialize Door Sensor ---
    if (m_miscConfig.doorSensorPin != 255) {
        ESP_LOGI(TAG, "Initializing door sensor on GPIO %d (invert=%d)", m_miscConfig.doorSensorPin, m_miscConfig.doorSensorInvert);
        pinMode(m_miscConfig.doorSensorPin, INPUT_PULLUP);

        m_doorSensorQueue = xQueueCreate(1, sizeof(uint8_t));
        if (!m_doorSensorQueue) {
            ESP_LOGE(TAG, "Failed to create door sensor queue");
        } else if (xTaskCreateUniversal(door_sensor_task_entry, "door_sensor_task", 2048, this, 3, &m_doorSensorTaskHandle, 1) != pdPASS) {
            ESP_LOGE(TAG, "Failed to create door sensor task");
            vQueueDelete(m_doorSensorQueue);
            m_doorSensorQueue = nullptr;
            m_doorSensorTaskHandle = nullptr;
        } else {
            // Install ISR service if not already installed
            if (m_actionsConfig.hkAltActionInitPin == 255) {
                gpio_install_isr_service(0);
            }
            gpio_set_intr_type((gpio_num_t)m_miscConfig.doorSensorPin, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add((gpio_num_t)m_miscConfig.doorSensorPin, door_sensor_isr_handler, (void*) this);

            // Publish initial door state
            uint8_t initialState = digitalRead(m_miscConfig.doorSensorPin);
            bool isClosed = m_miscConfig.doorSensorInvert ? (initialState == HIGH) : (initialState == LOW);
            m_lastDoorState = isClosed ? 1 : 0;
            EventDoorState event{.doorState = m_lastDoorState};
            std::vector<uint8_t> data;
            alpaca::serialize(event, data);
            event_bus.publish({m_door_state_topic, 0, data.data(), data.size()});
            ESP_LOGI(TAG, "Door sensor initialized, initial state: %s", isClosed ? "Closed" : "Open");
        }
    }

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
      digitalWrite(i->m_actionsConfig.nfcSuccessPin, !i->m_actionsConfig.nfcSuccessHL);
      ESP_LOGD(TAG, "GPIO_S");
      break;
    case TimerSources::GPIO_F:
      digitalWrite(i->m_actionsConfig.nfcFailPin, !i->m_actionsConfig.nfcFailHL);
      ESP_LOGD(TAG, "GPIO_F");
      break;
    case TimerSources::TAG_EVENT:
      digitalWrite(i->m_actionsConfig.tagEventPin, !i->m_actionsConfig.tagEventHL);
      ESP_LOGD(TAG, "TAG_EVENT");
      break;
    case TimerSources::PIXEL_S:
    case TimerSources::PIXEL_F:
    case TimerSources::PIXEL_TAG_EVENT:
      i->m_pixel->off();
      ESP_LOGD(TAG, "PIXEL");
      break;
    case TimerSources::ALT_GPIO:
      digitalWrite(i->m_actionsConfig.hkAltActionPin, !i->m_actionsConfig.hkAltActionGpioState);
      ESP_LOGD(TAG, "ALT_GPIO");
      break;
    case TimerSources::ALT_GPIO_INIT:
      i->m_altActionArmed = false;
      if (i->m_actionsConfig.hkAltActionInitLedPin != 255) {
        digitalWrite(i->m_actionsConfig.hkAltActionInitLedPin, LOW);
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
 * - Writes HIGH to `m_actionsConfig.hkAltActionInitLedPin` when that pin is not 255.
 * - Starts `m_altActionInitTimer` for `m_actionsConfig.hkAltActionInitTimeout` milliseconds.
 */
void HardwareManager::initiator_task() {
    uint8_t dummy;
    while (true) {
        if (xQueueReceive(m_initiatorQueue, &dummy, portMAX_DELAY)) {
            if (!m_altActionArmed) {
                ESP_LOGI(TAG, "Alt action armed for %dms", m_actionsConfig.hkAltActionInitTimeout);
                m_altActionArmed = true;
                if (m_actionsConfig.hkAltActionInitLedPin != 255) {
                    digitalWrite(m_actionsConfig.hkAltActionInitLedPin, HIGH);
                }

                esp_timer_start_once(m_altActionInitTimer, m_actionsConfig.hkAltActionInitTimeout * 1000);
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
 * @brief Processes lock commands from the internal lock-control queue, drives the configured GPIO, and publishes the resulting lock state.
 *
 * Waits for a lock state value from the internal queue; if a GPIO action pin is configured the function sets that pin to the configured lock or unlock level and publishes an EventLockState whose `currentState` equals the received state, `targetState` is `UNKNOWN`, and `source` is `INTERNAL` to the lock update topic. If no action pin is configured (pin value 255), the command is ignored.
 */
void HardwareManager::lockControlTask() {
    int receivedState;
    while (true) {
        if (xQueueReceive(m_lockControlQueue, &receivedState, portMAX_DELAY)) {
          if (m_actionsConfig.gpioActionPin == 255) {
              ESP_LOGI(TAG, "Received lock command but no action pin is configured.");
              continue;
          }
          
          ESP_LOGI(TAG, "Setting lock output for state: %d", receivedState);
          if (receivedState == LockManager::LOCKED) {
              digitalWrite(m_actionsConfig.gpioActionPin, m_actionsConfig.gpioActionLockState);
          } else if (receivedState == LockManager::UNLOCKED) {
              digitalWrite(m_actionsConfig.gpioActionPin, m_actionsConfig.gpioActionUnlockState);
          }
          EventLockState s{
            .currentState = static_cast<uint8_t>(receivedState),
            .targetState = LockManager::UNKNOWN,
            .source = LockManager::INTERNAL
          };
          std::vector<uint8_t> d;
          alpaca::serialize(s, d);
          event_bus.publish({event_bus.get_topic(LOCK_UPDATE_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
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
      event_bus.publish({m_alt_action_topic, 1});
      if (m_actionsConfig.hkAltActionPin != 255) {
          ESP_LOGI(TAG, "Triggering alt action on pin %d for %dms", m_actionsConfig.hkAltActionPin, m_actionsConfig.hkAltActionTimeout);
          digitalWrite(m_actionsConfig.hkAltActionPin, m_actionsConfig.hkAltActionGpioState);
          esp_timer_start_once(m_altActionTimer, m_actionsConfig.hkAltActionTimeout * 1000);
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
                    if(esp_timer_is_active(m_pixelSuccessTimer)) esp_timer_stop(m_pixelSuccessTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_actionsConfig.neopixelSuccessColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelSuccessTimer, m_actionsConfig.neopixelSuccessTime * 1000);
                    }
                    if (m_actionsConfig.nfcSuccessPin != 255) {
                        digitalWrite(m_actionsConfig.nfcSuccessPin, m_actionsConfig.nfcSuccessHL);

                        esp_timer_start_once(m_gpioSuccessTimer, m_actionsConfig.nfcSuccessTime * 1000);
                    }
                    break;

                case FeedbackType::FAILURE:
                    ESP_LOGD(TAG, "Executing FAILURE feedback sequence.");
                    if(esp_timer_is_active(m_gpioFailTimer)) esp_timer_stop(m_gpioFailTimer);
                    if(esp_timer_is_active(m_pixelFailTimer)) esp_timer_stop(m_pixelFailTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_actionsConfig.neopixelFailureColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelFailTimer, m_actionsConfig.neopixelFailTime * 1000);
                    }
                    if (m_actionsConfig.nfcFailPin != 255) {
                        digitalWrite(m_actionsConfig.nfcFailPin, m_actionsConfig.nfcFailHL);

                        esp_timer_start_once(m_gpioFailTimer, m_actionsConfig.nfcFailTime * 1000);
                    }
                    break;
                case FeedbackType::TAG_EVENT: 
                    ESP_LOGD(TAG, "Executing TAG_EVENT feedback sequence.");
                    if(esp_timer_is_active(m_tagEventTimer)) esp_timer_stop(m_tagEventTimer);
                    if(esp_timer_is_active(m_pixelTagEventTimer)) esp_timer_stop(m_pixelTagEventTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_actionsConfig.neopixelTagEventColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                        esp_timer_start_once(m_pixelTagEventTimer, m_actionsConfig.neopixelTagEventTime * 1000);
                    }
                    if (m_actionsConfig.tagEventPin != 255) {
                        digitalWrite(m_actionsConfig.tagEventPin, m_actionsConfig.tagEventHL);

                        esp_timer_start_once(m_tagEventTimer, m_actionsConfig.tagEventTimeout * 1000);
                    }
                    break;
            }
        }
    }
}
/**
 * @brief Task entry wrapper that forwards to the HardwareManager instance's door sensor task.
 *
 * @param arg Pointer to a HardwareManager instance (must not be null).
 */
void HardwareManager::door_sensor_task_entry(void* arg) {
    static_cast<HardwareManager*>(arg)->door_sensor_task();
}

/**
 * @brief ISR posted by the door sensor GPIO; signals the door sensor task by enqueuing a wake byte.
 *
 * Posts a single dummy byte to the HardwareManager's door sensor queue from ISR context to wake
 * the door sensor task without blocking.
 *
 * @param arg Pointer to the HardwareManager instance whose door sensor queue will be signaled.
 */
void IRAM_ATTR HardwareManager::door_sensor_isr_handler(void* arg) {
    uint8_t dummy = 0;
    xQueueSendFromISR(static_cast<HardwareManager*>(arg)->m_doorSensorQueue, &dummy, NULL);
}

/**
 * @brief Waits for door sensor GPIO interrupt events and publishes door state changes via EventBus.
 *
 * This task blocks waiting for a byte posted to the door sensor queue (from the ISR).
 * When an event is received, it reads the current GPIO state with debouncing (50ms delay),
 * applies the configured invert logic, and publishes a door state event if the state has changed.
 *
 * Door states:
 * - 0 = Open
 * - 1 = Closed
 *
 * The invert logic allows for different wiring configurations:
 * - invert=false: LOW=closed, HIGH=open
 * - invert=true: LOW=open, HIGH=closed
 */
void HardwareManager::door_sensor_task() {
    uint8_t dummy;
    while (true) {
        if (xQueueReceive(m_doorSensorQueue, &dummy, portMAX_DELAY)) {
            // Debounce: wait 50ms before reading the state
            vTaskDelay(pdMS_TO_TICKS(50));

            // Read current door sensor state
            uint8_t currentState = digitalRead(m_miscConfig.doorSensorPin);
            
            // Apply invert logic: if inverted, swap the interpretation
            bool isClosed = m_miscConfig.doorSensorInvert ? (currentState == HIGH) : (currentState == LOW);
            uint8_t doorState = isClosed ? 1 : 0;  // 1 = Closed, 0 = Open

            // Only publish if the state has changed
            if (doorState != m_lastDoorState) {
                m_lastDoorState = doorState;
                
                EventDoorState event{.doorState = doorState};
                std::vector<uint8_t> data;
                alpaca::serialize(event, data);
                event_bus.publish({m_door_state_topic, 0, data.data(), data.size()});
                
                ESP_LOGI(TAG, "Door sensor state changed: %s", isClosed ? "Closed" : "Open");
            }
        }
    }
}
