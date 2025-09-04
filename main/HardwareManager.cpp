#include <event_manager.hpp>
#include "HardwareManager.hpp"
#include "LockManager.hpp"
#include "Pixel.h"
#include "config.hpp"
#include "eventStructs.hpp"

const char* HardwareManager::TAG = "HardwareManager";

const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };

HardwareManager::HardwareManager(const espConfig::misc_config_t& miscConfig)
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
  espp::EventManager::get().add_subscriber(
      "lock/feedback", "HardwareManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventBinaryStatus s = alpaca::deserialize<EventBinaryStatus>(data, ec);
        if(!ec) {
          if(s.status == true){
            showSuccessFeedback();
          } else {
            showFailureFeedback();
          }
        }
      }, 3072);
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
  espp::EventManager::get().add_subscriber(
      "nfc/event", "HardwareManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        NfcEvent event = alpaca::deserialize<NfcEvent>(data, ec);
        if(ec) return;
        if(event.type == ALT_ACTION) {
          triggerAltAction();
        }
      }, 3072);
}

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
      xTaskCreateUniversal(initiator_task_entry, "initiator_task", 2048, this, 1, &m_initiatorTaskHandle, 1);
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
    const esp_timer_create_args_t gpioS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "gpioSuccessTimer"
    };
    const esp_timer_create_args_t gpioF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "gpioFailTimer"
    };
    const esp_timer_create_args_t pixelS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "pixelSuccessTimer"
    };
    const esp_timer_create_args_t pixelF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "pixelFailTimer"
    };
    const esp_timer_create_args_t altAction_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "altActionTimer"
    };
    const esp_timer_create_args_t altActionInit_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) this,
            .name = "altActionInitTimer"
    };
    esp_timer_create(&gpioS_timer_args, &m_gpioSuccessTimer);
    esp_timer_create(&gpioF_timer_args, &m_gpioFailTimer);
    esp_timer_create(&pixelS_timer_args, &m_pixelSuccessTimer);
    esp_timer_create(&pixelF_timer_args, &m_pixelFailTimer);
    esp_timer_create(&altAction_timer_args, &m_altActionTimer);
    esp_timer_create(&altActionInit_timer_args, &m_altActionInitTimer);

    m_timerSources = xQueueCreate(6, sizeof(TimerSources));

    m_feedbackQueue = xQueueCreate(5, sizeof(FeedbackType));
    xTaskCreateUniversal(feedbackTaskEntry, "feedback_task", 4096, this, 2, &m_feedbackTaskHandle, 1);

    m_lockControlQueue = xQueueCreate(5, sizeof(int));
    xTaskCreateUniversal(lockControlTaskEntry, "lock_control_task", 4096, this, 2, &m_lockControlTaskHandle, 1);
    ESP_LOGI(TAG, "Hardware initialization complete.");
}

// --- Public API Methods ---

void HardwareManager::setLockOutput(int state) {
    if (m_lockControlQueue != nullptr) {
        xQueueSend(m_lockControlQueue, &state, pdMS_TO_TICKS(100));
    }
}

void HardwareManager::showSuccessFeedback() {
    if (m_feedbackQueue != nullptr) {
        FeedbackType feedback = FeedbackType::SUCCESS;
        xQueueSend(m_feedbackQueue, &feedback, 0);
    }
}

void HardwareManager::showFailureFeedback() {
    if (m_feedbackQueue != nullptr) {
        FeedbackType feedback = FeedbackType::FAILURE;
        xQueueSend(m_feedbackQueue, &feedback, 0);
    }
}

// --- Private Implementations ---

void HardwareManager::handleTimer(void* instance){
  HardwareManager *i = static_cast<HardwareManager*>(instance);
  TimerSources t;
  uint8_t n = uxQueueMessagesWaiting(i->m_timerSources);
  ESP_LOGD(TAG, "Timer expired, %d messages waiting in queue!", n);
  for (uint8_t x = 0; x<n; x++) {
    if(xQueuePeek(i->m_timerSources, &t, 10)){
      switch (t) {
        case TimerSources::GPIO_S:
          if(!esp_timer_is_active(i->m_gpioSuccessTimer)){
            digitalWrite(i->m_miscConfig.nfcSuccessPin, !i->m_miscConfig.nfcSuccessHL);
            ESP_LOGD(TAG, "GPIO_S");
            xQueueReceive(i->m_timerSources, &t, 0);
          }
          break;
        case TimerSources::GPIO_F:
          if(!esp_timer_is_active(i->m_gpioFailTimer)){
            digitalWrite(i->m_miscConfig.nfcFailPin, !i->m_miscConfig.nfcFailHL);
            ESP_LOGD(TAG, "GPIO_F");
            xQueueReceive(i->m_timerSources, &t, 0);
          }
          break;
        case TimerSources::PIXEL:
          if(!esp_timer_is_active(i->m_pixelSuccessTimer) || !esp_timer_is_active(i->m_pixelFailTimer)){
            i->m_pixel->off();
            ESP_LOGD(TAG, "PIXEL");
            xQueueReceive(i->m_timerSources, &t, 0);
          }
          break;
        case TimerSources::ALT_GPIO:
          if(!esp_timer_is_active(i->m_altActionTimer)){
            digitalWrite(i->m_miscConfig.hkAltActionPin, !i->m_miscConfig.hkAltActionGpioState);
            ESP_LOGD(TAG, "ALT_GPIO");
            xQueueReceive(i->m_timerSources, &t, 0);
          }
          break;
        case TimerSources::ALT_GPIO_INIT:
          if(!esp_timer_is_active(i->m_altActionInitTimer)){
            i->m_altActionArmed = false;
            if (i->m_miscConfig.hkAltActionInitLedPin != 255) {
              digitalWrite(i->m_miscConfig.hkAltActionInitLedPin, LOW);
            }
            ESP_LOGD(TAG, "ALT_GPIO_INIT");
            xQueueReceive(i->m_timerSources, &t, 0);
          }
          break;
      }
    } 
  }
}

void HardwareManager::initiator_task_entry(void* arg) {
    static_cast<HardwareManager*>(arg)->initiator_task();
}

void IRAM_ATTR HardwareManager::initiator_isr_handler(void* arg) {
    uint8_t dummy = 0;
    xQueueSendFromISR(static_cast<HardwareManager*>(arg)->m_initiatorQueue, &dummy, NULL);
}

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
                TimerSources t = TimerSources::ALT_GPIO_INIT;
                xQueueSend(m_timerSources, &t, 0);
                esp_timer_start_once(m_altActionInitTimer, m_miscConfig.hkAltActionInitTimeout * 1000);
            }
        }
    }
}

void HardwareManager::lockControlTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->lockControlTask();
}

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

void HardwareManager::triggerAltAction() {
  if (m_altActionArmed) {
      if (m_miscConfig.hkAltActionPin != 255) {
          ESP_LOGI(TAG, "Triggering alt action on pin %d for %dms", m_miscConfig.hkAltActionPin, m_miscConfig.hkAltActionTimeout);
          digitalWrite(m_miscConfig.hkAltActionPin, m_miscConfig.hkAltActionGpioState);
          TimerSources t = TimerSources::ALT_GPIO;
          xQueueSend(m_timerSources, &t, 0);
          esp_timer_start_once(m_altActionTimer, m_miscConfig.hkAltActionTimeout * 1000);
      }
  }
}

void HardwareManager::feedbackTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->feedbackTask();
}

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
                        m_pixel->set(m_pixel->RGB(color[espConfig::misc_config_t::R], color[espConfig::misc_config_t::G], color[espConfig::misc_config_t::B]));
                        TimerSources t = TimerSources::PIXEL;
                        xQueueSend(m_timerSources, &t, 0);
                        esp_timer_start_once(m_pixelSuccessTimer, m_miscConfig.neopixelSuccessTime * 1000);
                    }
                    if (m_miscConfig.nfcSuccessPin != 255) {
                        digitalWrite(m_miscConfig.nfcSuccessPin, m_miscConfig.nfcSuccessHL);
                        TimerSources t = TimerSources::GPIO_S;
                        xQueueSend(m_timerSources, &t, 0);
                        esp_timer_start_once(m_gpioSuccessTimer, m_miscConfig.nfcSuccessTime * 1000);
                    }
                    break;

                case FeedbackType::FAILURE:
                    ESP_LOGD(TAG, "Executing FAILURE feedback sequence.");
                    if(esp_timer_is_active(m_gpioFailTimer)) esp_timer_stop(m_gpioFailTimer);
                    if(esp_timer_is_active(m_pixelFailTimer)) esp_timer_stop(m_pixelFailTimer);
                    if (m_pixel != nullptr) {
                        auto color = m_miscConfig.neopixelFailureColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::misc_config_t::R], color[espConfig::misc_config_t::G], color[espConfig::misc_config_t::B]));
                        TimerSources t = TimerSources::PIXEL;
                        xQueueSend(m_timerSources, &t, 0);
                        esp_timer_start_once(m_pixelFailTimer, m_miscConfig.neopixelFailTime * 1000);
                    }
                    if (m_miscConfig.nfcFailPin != 255) {
                        digitalWrite(m_miscConfig.nfcFailPin, m_miscConfig.nfcFailHL);
                        TimerSources t = TimerSources::GPIO_F;
                        xQueueSend(m_timerSources, &t, 0);
                        esp_timer_start_once(m_gpioFailTimer, m_miscConfig.nfcFailTime * 1000);
                    }
                    break;
            }
        }
    }
}
