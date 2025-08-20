#include "HardwareManager.hpp"
#include "Pixel.h"
#include "config.hpp"

#include <esp_log.h>

const char* HardwareManager::TAG = "HardwareManager";

const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };

HardwareManager::HardwareManager(const espConfig::misc_config_t& miscConfig)
    : m_miscConfig(miscConfig),
      m_pixel(nullptr),
      m_feedbackTaskHandle(nullptr),
      m_feedbackQueue(nullptr),
      m_lockControlTaskHandle(nullptr),
      m_lockControlQueue(nullptr)
{
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
      pinMode(m_miscConfig.hkAltActionInitPin, INPUT);
      if (m_miscConfig.hkAltActionPin != 255) {
        pinMode(m_miscConfig.hkAltActionPin, OUTPUT);
      }
      if (m_miscConfig.hkAltActionInitLedPin != 255) {
        pinMode(m_miscConfig.hkAltActionInitLedPin, OUTPUT);
      }
    }

    // --- Initialize NeoPixel ---
    if (m_miscConfig.nfcNeopixelPin != 255) {
        size_t pixelTypeIndex = m_miscConfig.neoPixelType;
        if (pixelTypeIndex >= pixelTypeMap.size()) {
            ESP_LOGW(TAG, "Invalid NeoPixel type index (%d), defaulting to GRB.", pixelTypeIndex);
            pixelTypeIndex = 5; // GRB
        }
        m_pixel = std::make_shared<Pixel>(m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
        m_pixel->off(); // Ensure pixel is off at startup
        ESP_LOGI(TAG, "NeoPixel initialized on pin %d with type %s.", m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
    }

    // --- Create Queues and Tasks ---
    m_feedbackQueue = xQueueCreate(5, sizeof(FeedbackType));
    xTaskCreate(feedbackTaskEntry, "feedback_task", 4096, this, 5, &m_feedbackTaskHandle);

    m_lockControlQueue = xQueueCreate(5, sizeof(int));
    xTaskCreate(lockControlTaskEntry, "lock_control_task", 4096, this, 10, &m_lockControlTaskHandle);
    
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

// --- Private Task Implementations ---

void HardwareManager::lockControlTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->lockControlTask();
}

void HardwareManager::lockControlTask() {
    int receivedState;
    while (true) {
        if (xQueueReceive(m_lockControlQueue, &receivedState, portMAX_DELAY)) {
            if (m_miscConfig.gpioActionPin == 255) {
                ESP_LOGD(TAG, "Received lock command but no action pin is configured.");
                continue;
            }
            
            ESP_LOGD(TAG, "Setting lock output for state: %d", receivedState);
            if (receivedState == lockStates::LOCKED) {
                digitalWrite(m_miscConfig.gpioActionPin, m_miscConfig.gpioActionLockState);
            } else if (receivedState == lockStates::UNLOCKED) {
                digitalWrite(m_miscConfig.gpioActionPin, m_miscConfig.gpioActionUnlockState);
            }
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
                    if (m_pixel) {
                        auto color = m_miscConfig.neopixelSuccessColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::misc_config_t::R], color[espConfig::misc_config_t::G], color[espConfig::misc_config_t::B]));
                    }
                    if (m_miscConfig.nfcSuccessPin != 255) {
                        digitalWrite(m_miscConfig.nfcSuccessPin, m_miscConfig.nfcSuccessHL);
                    }
                    
                    delay(m_miscConfig.neopixelSuccessTime > m_miscConfig.nfcSuccessTime ? m_miscConfig.neopixelSuccessTime : m_miscConfig.nfcSuccessTime);
                    
                    if (m_pixel) m_pixel->off();
                    if (m_miscConfig.nfcSuccessPin != 255) digitalWrite(m_miscConfig.nfcSuccessPin, !m_miscConfig.nfcSuccessHL);
                    break;

                case FeedbackType::FAILURE:
                    ESP_LOGD(TAG, "Executing FAILURE feedback sequence.");
                    if (m_pixel) {
                        auto color = m_miscConfig.neopixelFailureColor;
                        m_pixel->set(m_pixel->RGB(color[espConfig::misc_config_t::R], color[espConfig::misc_config_t::G], color[espConfig::misc_config_t::B]));
                    }
                    if (m_miscConfig.nfcFailPin != 255) {
                        digitalWrite(m_miscConfig.nfcFailPin, m_miscConfig.nfcFailHL);
                    }

                    delay(m_miscConfig.neopixelFailTime > m_miscConfig.nfcFailTime ? m_miscConfig.neopixelFailTime : m_miscConfig.nfcFailTime);

                    if (m_pixel) m_pixel->off();
                    if (m_miscConfig.nfcFailPin != 255) digitalWrite(m_miscConfig.nfcFailPin, !m_miscConfig.nfcFailHL);
                    break;
            }
        }
    }
}
