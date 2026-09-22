#include "HardwareManager.hpp"
#include "LockManager.hpp"
#include "Pixel.h"
#include "config.hpp"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "eventStructs.hpp"
#include "hal/gpio_types.h"
#include "soc/gpio_struct.h"
#include "hal/gpio_ll.h"
#include "soc/gpio_num.h"
#include "SharedLed.hpp"

const char* HardwareManager::TAG = "HardwareManager";

const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };


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
 * @param miscConfig Configuration values controlling GPIO pins, NeoPixel behavior, and timing.
 */
HardwareManager::HardwareManager(const espConfig::actions_config_t& miscConfig)
    : m_miscConfig(miscConfig),
      m_hwEventTaskHandle(nullptr),
      m_hwEventQueue(nullptr)
{
  pinAllocations.emplace(PinFunctions::ACTION,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.gpioActionPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut, GPIOAllocator::PinConsumer::Hardware, "ACTION_PIN"));
  pinAllocations.emplace(PinFunctions::SUCCESS,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.nfcSuccessPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::Hardware, "SUCCESS_AUTH"));
  pinAllocations.emplace(PinFunctions::FAIL,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.nfcFailPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::Hardware, "FAIL_AUTH"));
  pinAllocations.emplace(PinFunctions::PIXEL,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.nfcNeopixelPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut, GPIOAllocator::PinConsumer::Hardware, "NEOPIXEL_PIN"));
  pinAllocations.emplace(PinFunctions::ALT_ACTION,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.hkAltActionPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut, GPIOAllocator::PinConsumer::Hardware, "ALT_ACTION"));
  pinAllocations.emplace(PinFunctions::ALT_ACTION_LED,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.hkAltActionInitLedPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::Hardware, "ALT_ACTION_LED"));
  pinAllocations.emplace(PinFunctions::ALT_ACTION_INIT,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.hkAltActionInitPin), GPIO_MODE_INPUT, GPIOAllocator::PinRole::Irq, GPIOAllocator::PinConsumer::Hardware, "INIT_ALT_ACTION"));
  pinAllocations.emplace(PinFunctions::TAG_EVENT,
      GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.tagEventPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::Hardware, "TAG_EVENT_PIN"));
  for(auto &p : pinAllocations){
    if(!p.second.has_value()){
      ESP_LOGW(TAG, "Could not acquire GPIO Pin for '%s' with error '%s'", pin_function_str(p.first), GPIOAllocator::error_str(p.second.error()));
    }
  }
  m_hardware_action_event = AppEventLoop::subscribe(HW_EVENT, HW_ACTION, [&](const uint8_t* data, size_t size){
    if(size == 0 || data == nullptr) return;
    std::span<const uint8_t> payload(data, size);
    std::error_code ec;
    EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
    if(ec) { ESP_LOGE(TAG, "Failed to deserialize lock state event: %s", ec.message().c_str()); return; }
    ESP_LOGD(TAG, "Received action event: %d -> %d", s.currentState, s.targetState);
    setLockOutput(s.targetState);
  });
  m_gpio_pin_event = AppEventLoop::subscribe(HW_EVENT, HW_CONFIG_CHANGED, [&](const uint8_t* data, size_t size) {
    if (size == 0 || data == nullptr) return;
    std::span<const uint8_t> payload(data, size);
    std::error_code ec;
    EventValueChanged s = alpaca::deserialize<EventValueChanged>(payload, ec);
    if (ec) { 
      ESP_LOGE(TAG, "Failed to deserialize hardware config event: %s", ec.message().c_str()); 
      return; 
    }

    ESP_LOGD(TAG, "Received hardware config event: %s -> %d (old=%d)", s.name.c_str(), s.newValue, s.oldValue);

    if (s.newValue == s.oldValue) return;

    struct PinMeta {
      PinFunctions func;
      const char* config_name;
      const char* tag;
      gpio_mode_t default_mode;
      GPIOAllocator::PinRole role;
    };

    static const PinMeta pin_meta_table[] = {
      { ACTION,          "gpioActionPin",         "ACTION_PIN",      GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut },
      { SUCCESS,         "nfcSuccessPin",         "SUCCESS_AUTH",    GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led     },
      { FAIL,            "nfcFailPin",            "FAIL_AUTH",       GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led     },
      { PIXEL,           "nfcNeopixelPin",        "NEOPIXEL_PIN",    GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut },
      { ALT_ACTION,      "hkAltActionPin",        "ALT_ACTION",      GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::GpioOut },
      { ALT_ACTION_LED,  "hkAltActionInitLedPin", "ALT_ACTION_LED",  GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led     },
      { ALT_ACTION_INIT, "hkAltActionInitPin",    "INIT_ALT_ACTION", GPIO_MODE_INPUT,  GPIOAllocator::PinRole::Irq     },
      { TAG_EVENT,       "tagEventPin",           "TAG_EVENT_PIN",   GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led     }
    };

    const PinMeta* meta = nullptr;
    for (const auto& item : pin_meta_table) {
      if (s.name == item.config_name) {
        meta = &item;
        break;
      }
    }

    if (!meta) {
      ESP_LOGW(TAG, "Unknown hardware config parameter: %s", s.name.c_str());
      return;
    }

    auto& alloc_entry = pinAllocations.at(meta->func);
    gpio_mode_t mode = meta->default_mode;
    bool level = false;

    if (live(alloc_entry)) {
      auto& lease = alloc_entry.value();
      mode = lease.get_mode();
      level = lease.get_level();

      if (meta->func == PinFunctions::ALT_ACTION_INIT) {
        // We don't uninstall the ISR as it might be used by the Ethernet driver
        // and HardwareManager doesn't have access to misc_config_t to check
        gpio_isr_handler_remove(lease.get_pin());
      }

      alloc_entry = std::unexpected<GPIOAllocator::GPIOAllocatorError>(GPIOAllocator::INVALID_GPIO_NUM);
      gpio_pulldown_en(gpio_num_t(s.oldValue));
    }

    if (s.newValue == 255) {
      ESP_LOGI(TAG, "%s set to undefined (255), stopping here.", meta->tag);
      return;
    }

    auto new_lease = GPIOAllocator::instance().acquire(gpio_num_t(s.newValue), mode, meta->role, GPIOAllocator::PinConsumer::Hardware, meta->tag);
    if (new_lease.has_value()) {
      if (mode != GPIO_MODE_INPUT) {
        new_lease.value().set_level(level);
      }

      if (meta->func == PinFunctions::ALT_ACTION_INIT) {
        new_lease.value().set_pulldown(false);
        new_lease.value().set_pullup(true);
        if (m_initiatorQueue == nullptr) {
          m_initiatorQueue = xQueueCreate(1, sizeof(uint8_t));
        }
        if (m_initiatorTaskHandle == nullptr) {
          xTaskCreateUniversal(initiator_task_entry, "initiator_task", 3580, this, 3, &m_initiatorTaskHandle, 1);
        }
        if(!isr_service_installed){
          esp_err_t err = gpio_install_isr_service(0);
          if(err == ESP_OK || err == ESP_ERR_INVALID_STATE) isr_service_installed = true;
        }
        gpio_set_intr_type(gpio_num_t(s.newValue), GPIO_INTR_NEGEDGE);
        gpio_ll_clear_intr_status(&GPIO, 1U << s.newValue);
        gpio_isr_handler_add(gpio_num_t(s.newValue), initiator_isr_handler, (void*)this);
      }

      alloc_entry = std::move(new_lease);
      ESP_LOGI(TAG, "Acquired pin %d for %s", s.newValue, meta->tag);
    } else {
      ESP_LOGE(TAG, "Failed to acquire pin %d, error: %d", s.newValue, new_lease.error());
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
  m_nfc_event = AppEventLoop::subscribe(NFC_EVENT, NFC_TAP_EVENT, [&](const uint8_t* data, size_t size){
    if(size == 0 || data == nullptr) return;
    std::span<const uint8_t> payload(data, size);
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
          if (m_hwEventQueue != nullptr) {
              HwEvent event{HwEventType::FEEDBACK_TAG_EVENT, 0};
              xQueueSend(m_hwEventQueue, &event, 0);
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
    if(live(pinAllocations.at(SUCCESS))){
      pinAllocations.at(SUCCESS).value().set_level(!m_miscConfig.nfcSuccessHL);
    }
    if(live(pinAllocations.at(FAIL))){
      pinAllocations.at(FAIL).value().set_level(!m_miscConfig.nfcFailHL);
    }
    if(live(pinAllocations.at(TAG_EVENT))){
      pinAllocations.at(TAG_EVENT).value().set_level(!m_miscConfig.tagEventHL);
    }
    if(live(pinAllocations.at(ALT_ACTION_LED))){
      pinAllocations.at(ALT_ACTION_LED).value().set_level(0);
    }
    if(live(pinAllocations.at(ALT_ACTION_INIT))){
      gpio_num_t init_pin = pinAllocations.at(ALT_ACTION_INIT).value().get_pin();
      pinAllocations.at(ALT_ACTION_INIT).value().set_pullup(true);
      m_initiatorQueue = xQueueCreate(1, sizeof(uint8_t));
      xTaskCreateUniversal(initiator_task_entry, "initiator_task", 3580, this, 3, &m_initiatorTaskHandle, 1);
      if(esp_err_t err = gpio_install_isr_service(0); err == ESP_OK || err == ESP_ERR_INVALID_STATE){
        isr_service_installed = true;
      }
      gpio_set_intr_type(init_pin, GPIO_INTR_NEGEDGE);
      gpio_ll_clear_intr_status(&GPIO, 1U << init_pin);
      gpio_isr_handler_add(init_pin, initiator_isr_handler, (void*) this);
    }

    // --- Initialize NeoPixel ---
    if (live(pinAllocations.at(PIXEL))) {
        size_t pixelTypeIndex = m_miscConfig.neoPixelType;
        if (pixelTypeIndex >= pixelTypeMap.size()) {
            ESP_LOGW(TAG, "Invalid NeoPixel type index (%d), defaulting to GRB.", pixelTypeIndex);
            pixelTypeIndex = 5; // GRB
        }
        m_pixel = new Pixel(m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
        m_pixel->off(); // Ensure pixel is off at startup
        ESP_LOGI(TAG, "NeoPixel initialized on pin %d with type %s.", m_miscConfig.nfcNeopixelPin, pixelTypeMap[pixelTypeIndex]);
    } else {
        ESP_LOGI(TAG, "NeoPixel disabled, no pin configured.");
    }

    // Initialize timer contexts as member variables (not static locals)
    m_gpioS_context = {this, TimerSources::GPIO_S};
    m_gpioF_context = {this, TimerSources::GPIO_F};
    m_tagEvent_context = {this, TimerSources::TAG_EVENT};
    m_pixelS_context = {this, TimerSources::PIXEL_S};
    m_pixelF_context = {this, TimerSources::PIXEL_F};
    m_pixelTagEvent_context = {this, TimerSources::PIXEL_TAG_EVENT};
    m_altAction_context = {this, TimerSources::ALT_GPIO};
    m_altActionInit_context = {this, TimerSources::ALT_GPIO_INIT};

    const esp_timer_create_args_t gpioS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_gpioS_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "gpioSuccessTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t gpioF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_gpioF_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "gpioFailTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t tagEvent_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_tagEvent_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "tagEventTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t pixelS_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_pixelS_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "pixelSuccessTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t pixelF_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_pixelF_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "pixelFailTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t pixelTagEvent_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_pixelTagEvent_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "pixelTagEventTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t altAction_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_altAction_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "altActionTimer",
            .skip_unhandled_events = false
    };

    const esp_timer_create_args_t altActionInit_timer_args = {
            .callback = &handleTimer,
            .arg = (void*) &m_altActionInit_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "altActionInitTimer",
            .skip_unhandled_events = false
    };

    auto create_timer = [](const esp_timer_create_args_t& args, esp_timer_handle_t& handle, const char* name) {
        esp_err_t err = esp_timer_create(&args, &handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create %s: %s", name, esp_err_to_name(err));
            handle = nullptr;
        }
    };

    create_timer(gpioS_timer_args, m_gpioSuccessTimer, "gpioSuccessTimer");
    create_timer(gpioF_timer_args, m_gpioFailTimer, "gpioFailTimer");
    create_timer(tagEvent_timer_args, m_tagEventTimer, "tagEventTimer");
    create_timer(pixelS_timer_args, m_pixelSuccessTimer, "pixelSuccessTimer");
    create_timer(pixelF_timer_args, m_pixelFailTimer, "pixelFailTimer");
    create_timer(pixelTagEvent_timer_args, m_pixelTagEventTimer, "pixelTagEventTimer");
    create_timer(altAction_timer_args, m_altActionTimer, "altActionTimer");
    create_timer(altActionInit_timer_args, m_altActionInitTimer, "altActionInitTimer");

    m_hwEventQueue = xQueueCreate(10, sizeof(HwEvent));
    xTaskCreateUniversal(hwEventTaskEntry, "hw_event_task", 3580, this, 3, &m_hwEventTaskHandle, 1);
    ESP_LOGI(TAG, "Hardware initialization complete.");
}

/**
 * @brief Enqueue a desired lock state for the lock-control task to apply.
 *
 * @param state Desired lock state (for example `LOCKED` or `UNLOCKED`) to be processed and applied by the manager.
 */

void HardwareManager::setLockOutput(int state) {
    if (m_hwEventQueue != nullptr) {
        HwEvent event{HwEventType::LOCK_STATE, state};
        xQueueSend(m_hwEventQueue, &event, pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Enqueue a success feedback event for the feedback task to process.
 *
 * If the internal feedback queue is not initialized, the call has no effect.
 */
void HardwareManager::showSuccessFeedback() {
    if (m_hwEventQueue != nullptr) {
        HwEvent event{HwEventType::FEEDBACK_SUCCESS, 0};
        xQueueSend(m_hwEventQueue, &event, 0);
    }
}

/**
 * @brief Enqueues a failure feedback event for the feedback task.
 *
 * If the internal feedback queue has been created, posts a FAILURE event so the
 * feedback task will run the configured failure sequence (GPIOs/NeoPixel).
 */
void HardwareManager::showFailureFeedback() {
    if (m_hwEventQueue != nullptr) {
        HwEvent event{HwEventType::FEEDBACK_FAILURE, 0};
        xQueueSend(m_hwEventQueue, &event, 0);
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
      if(live(i->pinAllocations.at(SUCCESS))) i->pinAllocations.at(SUCCESS).value().set_level(!i->m_miscConfig.nfcSuccessHL);
      ESP_LOGD(TAG, "GPIO_S");
      break;
    case TimerSources::GPIO_F:
      if(live(i->pinAllocations.at(FAIL))) i->pinAllocations.at(FAIL).value().set_level(!i->m_miscConfig.nfcFailHL);
      ESP_LOGD(TAG, "GPIO_F");
      break;
    case TimerSources::TAG_EVENT:
      if(live(i->pinAllocations.at(TAG_EVENT))) i->pinAllocations.at(TAG_EVENT).value().set_level(!i->m_miscConfig.tagEventHL);
      ESP_LOGD(TAG, "TAG_EVENT");
      break;
    case TimerSources::PIXEL_S:
    case TimerSources::PIXEL_F:
    case TimerSources::PIXEL_TAG_EVENT:
      i->m_pixel->off();
      ESP_LOGD(TAG, "PIXEL");
      break;
    case TimerSources::ALT_GPIO:
      if(live(i->pinAllocations.at(ALT_ACTION))) i->pinAllocations.at(ALT_ACTION).value().set_level(!i->m_miscConfig.hkAltActionGpioState);
      ESP_LOGD(TAG, "ALT_GPIO");
      break;
    case TimerSources::ALT_GPIO_INIT:
      i->m_altActionArmed = false;
      if(live(i->pinAllocations.at(ALT_ACTION_LED))) i->pinAllocations.at(ALT_ACTION_LED).value().set_level(0);
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
    QueueHandle_t queue = static_cast<HardwareManager*>(arg)->m_initiatorQueue;
    if (queue == nullptr) return;
    uint8_t dummy = 0;
    xQueueSendFromISR(queue, &dummy, NULL);
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
                if(live(pinAllocations.at(ALT_ACTION_LED))) pinAllocations.at(ALT_ACTION_LED).value().set_level(1);

                if (m_altActionInitTimer) esp_timer_start_once(m_altActionInitTimer, m_miscConfig.hkAltActionInitTimeout * 1000);
            }
        }
    }
}

/**
 * @brief Applies a lock state to the configured action GPIO and publishes the resulting lock state.
 *
 * If a GPIO action pin is configured the function sets that pin to the configured lock or unlock level and publishes an EventLockState whose `currentState` equals the received state, `targetState` is `UNKNOWN`, and `source` is `INTERNAL` to the lock update topic. If no action pin is configured (pin value 255), the command is ignored.
 */
void HardwareManager::applyLockState(int receivedState) {
    if (m_miscConfig.gpioActionPin == 255) {
        ESP_LOGI(TAG, "Received lock command but no action pin is configured.");
        return;
    }

    ESP_LOGI(TAG, "Setting lock output for state: %d", receivedState);
    auto &action = pinAllocations.at(ACTION);
    if(live(action)){
      gpio_hold_dis(action->get_pin());
    } else {
      ESP_LOGW(TAG, "GPIOLease not held for action pin; skipping lock output");
      return;
    }
    if (receivedState == LockManager::LOCKED) {
        action->set_level(m_miscConfig.gpioActionLockState);
    } else if (receivedState == LockManager::UNLOCKED) {
        action->set_level(m_miscConfig.gpioActionUnlockState);
    }
    gpio_hold_en(action->get_pin());
    EventLockState s{
      .currentState = static_cast<uint8_t>(receivedState),
      .targetState = LockManager::UNKNOWN,
      .source = LockManager::INTERNAL
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    AppEventLoop::publish(LOCK_EVENT, LOCK_UPDATE_STATE, d.data(), d.size());
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
      AppEventLoop::publish(HW_EVENT, HW_ALT_ACTION, nullptr, 0);
      if (live(pinAllocations.at(ALT_ACTION))) {
          ESP_LOGI(TAG, "Triggering alt action on pin %d for %dms", m_miscConfig.hkAltActionPin, m_miscConfig.hkAltActionTimeout);
          pinAllocations.at(ALT_ACTION)->set_level(m_miscConfig.hkAltActionGpioState);
          if (m_altActionTimer) esp_timer_start_once(m_altActionTimer, m_miscConfig.hkAltActionTimeout * 1000);
      }
  }
}

/**
 * @brief Starts the hardware event task for the provided HardwareManager instance.
 *
 * This is a FreeRTOS task entry function that casts the opaque `instance` pointer
 * to `HardwareManager*` and invokes its hardware event task routine.
 *
 * @param instance Pointer to a HardwareManager object (must be a valid `HardwareManager*`).
 */
void HardwareManager::hwEventTaskEntry(void* instance) {
    static_cast<HardwareManager*>(instance)->hwEventTask();
}

/**
 * @brief Processes queued hardware events (feedback sequences and lock state changes).
 *
 * This task runs indefinitely, blocking on the internal hardware event queue. For each
 * HwEvent received it executes the corresponding action:
 * - FEEDBACK_SUCCESS/FEEDBACK_FAILURE/FEEDBACK_TAG_EVENT: runs the feedback sequence
 *   (NeoPixel color, GPIO pulse, timer handling).
 * - LOCK_STATE: applies the requested lock state to the action GPIO.
 *
 * GPIO pins configured with the sentinel value 255 are ignored. Timers and
 * NeoPixel behavior use durations and colors from the instance's misc configuration.
 */
void HardwareManager::hwEventTask() {
    HwEvent event;
    while (true) {
        if (xQueueReceive(m_hwEventQueue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case HwEventType::FEEDBACK_SUCCESS:
                    runFeedbackSequence(HwEventType::FEEDBACK_SUCCESS);
                    break;
                case HwEventType::FEEDBACK_FAILURE:
                    runFeedbackSequence(HwEventType::FEEDBACK_FAILURE);
                    break;
                case HwEventType::FEEDBACK_TAG_EVENT:
                    runFeedbackSequence(HwEventType::FEEDBACK_TAG_EVENT);
                    break;
                case HwEventType::LOCK_STATE:
                    applyLockState(event.state);
                    break;
            }
        }
    }
}

/**
 * @brief Executes a feedback sequence for the given event type.
 *
 * Stops conflicting timers, sets the NeoPixel to the configured color and starts the
 * corresponding pixel timer, and drives the configured feedback GPIO for the configured
 * duration (via SharedLed arbitration when the pin is shared with the status LED).
 */
void HardwareManager::runFeedbackSequence(HwEventType feedback) {
    switch (feedback) {
        case HwEventType::FEEDBACK_SUCCESS:
            ESP_LOGD(TAG, "Executing SUCCESS feedback sequence.");
            if(m_gpioSuccessTimer && esp_timer_is_active(m_gpioSuccessTimer)) esp_timer_stop(m_gpioSuccessTimer);
            if(m_pixelSuccessTimer && esp_timer_is_active(m_pixelSuccessTimer)) esp_timer_stop(m_pixelSuccessTimer);
            if (m_pixel != nullptr) {
                auto color = m_miscConfig.neopixelSuccessColor;
                m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                if (m_pixelSuccessTimer) esp_timer_start_once(m_pixelSuccessTimer, m_miscConfig.neopixelSuccessTime * 1000);
            }
            if (live(pinAllocations.at(SUCCESS))) {
                auto& success_lease = pinAllocations.at(SUCCESS).value();
                // Shared LED (e.g. HomeSpan status pin): the arbiter
                // drives the flash and restores the blink afterwards.
                if (!SharedLed::preempt_pin(static_cast<int>(success_lease.get_pin()),
                                            m_miscConfig.nfcSuccessHL,
                                            m_miscConfig.nfcSuccessTime)) {
                    success_lease.set_level(m_miscConfig.nfcSuccessHL);
                    if (m_gpioSuccessTimer) esp_timer_start_once(m_gpioSuccessTimer, m_miscConfig.nfcSuccessTime * 1000);
                }
            }
            break;

        case HwEventType::FEEDBACK_FAILURE:
            ESP_LOGD(TAG, "Executing FAILURE feedback sequence.");
            if(m_gpioFailTimer && esp_timer_is_active(m_gpioFailTimer)) esp_timer_stop(m_gpioFailTimer);
            if(m_pixelFailTimer && esp_timer_is_active(m_pixelFailTimer)) esp_timer_stop(m_pixelFailTimer);
            if (m_pixel != nullptr) {
                auto color = m_miscConfig.neopixelFailureColor;
                m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                if (m_pixelFailTimer) esp_timer_start_once(m_pixelFailTimer, m_miscConfig.neopixelFailTime * 1000);
            }
            if (live(pinAllocations.at(FAIL))) {
                auto& fail_lease = pinAllocations.at(FAIL).value();
                if (!SharedLed::preempt_pin(static_cast<int>(fail_lease.get_pin()),
                                            m_miscConfig.nfcFailHL,
                                            m_miscConfig.nfcFailTime)) {
                    fail_lease.set_level(m_miscConfig.nfcFailHL);
                    if (m_gpioFailTimer) esp_timer_start_once(m_gpioFailTimer, m_miscConfig.nfcFailTime * 1000);
                }
            }
            break;
        case HwEventType::FEEDBACK_TAG_EVENT:
            ESP_LOGD(TAG, "Executing TAG_EVENT feedback sequence.");
            if(m_tagEventTimer && esp_timer_is_active(m_tagEventTimer)) esp_timer_stop(m_tagEventTimer);
            if(m_pixelTagEventTimer && esp_timer_is_active(m_pixelTagEventTimer)) esp_timer_stop(m_pixelTagEventTimer);
            if (m_pixel != nullptr) {
                auto color = m_miscConfig.neopixelTagEventColor;
                m_pixel->set(m_pixel->RGB(color[espConfig::actions_config_t::colorMap::R], color[espConfig::actions_config_t::colorMap::G], color[espConfig::actions_config_t::colorMap::B]));

                if (m_pixelTagEventTimer) esp_timer_start_once(m_pixelTagEventTimer, m_miscConfig.neopixelTagEventTime * 1000);
            }
            if (live(pinAllocations.at(TAG_EVENT))) {
                auto& tag_lease = pinAllocations.at(TAG_EVENT).value();
                if (!SharedLed::preempt_pin(static_cast<int>(tag_lease.get_pin()),
                                            m_miscConfig.tagEventHL,
                                            m_miscConfig.tagEventTimeout)) {
                    tag_lease.set_level(m_miscConfig.tagEventHL);
                            if (m_tagEventTimer) esp_timer_start_once(m_tagEventTimer, m_miscConfig.tagEventTimeout * 1000);
                        }
                    }
                    break;
        case HwEventType::LOCK_STATE:
            break;
    }
}
