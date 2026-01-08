#include "esp_log.h"
#include "esp_timer.h"
#include "config.hpp"
#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "eventStructs.hpp"

const char* LockManager::TAG = "LockManager";

static EventBus::Bus& event_bus = EventBus::Bus::instance();

/**
 * @brief Initialize a LockManager and set up state handling and event subscriptions.
 *
 * Initializes current and target lock states to LOCKED, stores provided configuration,
 * subscribes to lock override/target/update topics on the event bus to handle external
 * state changes, and creates the momentary-state timer used for temporary unlocks.
 *
 * @param miscConfig Configuration controlling NFC behavior, momentary timeout, and flags
 *                   such as lockAlwaysUnlock, lockAlwaysLock, and related GPIO/momentary source settings.
 * @param actionsConfig Configuration mapping hardware actions and GPIO behavior used when emitting hardware action events.
 */
LockManager::LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig)
    : m_miscConfig(miscConfig),
      m_actionsConfig(actionsConfig),
      m_currentState(lockStates::LOCKED),
      m_targetState(lockStates::LOCKED),
      bus_topic(event_bus.register_topic(LOCK_BUS_TOPIC))
{
  m_override_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_O_STATE_CHANGED), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) return;
      ESP_LOGD(TAG, "Received override state event: %d -> %d", s.currentState, s.targetState);
      overrideState(s.currentState, s.targetState);
    });
  m_target_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_T_STATE_CHANGED), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) return;
      ESP_LOGD(TAG, "Received target state event: %d -> %d", s.currentState, s.targetState);
      setTargetState(s.targetState, Source(s.source));
    });
  m_update_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_UPDATE_BUS_TOPIC), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) return;
      m_currentState = s.currentState;
      s.currentState = m_currentState;
      s.targetState = m_targetState;
      EventLockState sp{
        .currentState = static_cast<uint8_t>(m_currentState),
        .targetState = static_cast<uint8_t>(m_targetState),
        .source = LockManager::INTERNAL
      };
      std::vector<uint8_t> d;
      alpaca::serialize(sp, d);
      event_bus.publish({bus_topic, 0, d.data(), d.size()});
    });
  esp_timer_create_args_t momentaryStateTimer_arg = {
    .callback = handleTimer,
    .arg = this,
    .name = "momentaryStateTimer"
  };
  esp_timer_create(&momentaryStateTimer_arg, &momentaryStateTimer);
}

/**
 * @brief Initializes NFC handling and publishes the initial lock state to the hardware action topic.
 *
 * Subscribes to the NFC event topic and stores the subscription in m_nfc_event. Incoming NFC events with a non-empty payload are deserialized to NfcEvent; HOMEKEY_TAP events trigger processing of an embedded EventHKTap. If the tap payload deserializes and its status is true, the lock target is set via NFC according to configuration: force unlock if lockAlwaysUnlock is enabled, force lock if lockAlwaysLock is enabled, or toggle the current lock state otherwise. Deserialization errors or empty payloads are ignored. After subscribing, an EventLockState reflecting the current and target states (source INTERNAL) is serialized and published to HARDWARE_ACTION_BUS_TOPIC.
 */
void LockManager::begin() {
  m_nfc_event = event_bus.subscribe(event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      NfcEvent nfc_event = alpaca::deserialize<NfcEvent>(payload, ec);
      ESP_LOGD(TAG, "Received NFC event: %d", nfc_event.type);
      if(ec) return;
      if(nfc_event.type == HOMEKEY_TAP) {
        ESP_LOGI(TAG, "Processing NFC tap request...");
        EventHKTap s = alpaca::deserialize<EventHKTap>(nfc_event.data, ec);
        if (!ec && s.status) {
          if (m_miscConfig.lockAlwaysUnlock) {
            setTargetState(lockStates::UNLOCKED, Source::NFC);
          } else if (m_miscConfig.lockAlwaysLock) {
            setTargetState(lockStates::LOCKED, Source::NFC);
          } else {
            int newState = (m_currentState == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
            setTargetState(newState, Source::NFC);
          }
        }
      }
    });
  EventLockState s{
    .currentState = static_cast<uint8_t>(m_currentState),
    .targetState = static_cast<uint8_t>(m_targetState),
    .source = LockManager::INTERNAL
  };
  std::vector<uint8_t> d;
  alpaca::serialize(s, d);
  event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
}

/**
 * @brief Timer callback that sets the lock's target state to LOCKED with source INTERNAL.
 *
 * @param instance Pointer to the LockManager instance provided to the timer callback.
 */
void LockManager::handleTimer(void* instance){
  static_cast<LockManager*>(instance)->setTargetState(LOCKED, INTERNAL);
}

/**
 * @brief Gets the current lock state.
 *
 * @return int Current lock state as an integer code (e.g., LOCKED or UNLOCKED).
 */

int LockManager::getCurrentState() const {
    return m_currentState;
}

/**
 * @brief Retrieves the manager's configured target lock state.
 *
 * @return int The target lock state (e.g., LOCKED or UNLOCKED).
 */
int LockManager::getTargetState() const {
    return m_targetState;
}

/**
 * @brief Set the lock's desired target state and propagate the resulting state to observers.
 *
 * Updates the stored target state, optionally updates the current state when configured for
 * "dumb switch" mode, publishes an event for hardware actions when appropriate, always
 * publishes the updated state to the lock state topic, and starts the momentary unlock timer
 * when an unlock was requested by a configured momentary source.
 *
 * @param state Desired target lock state (use values from lockStates).
 * @param source Origin of the request; influences whether a hardware action event is published
 *               and whether the request is treated as momentary.

void LockManager::setTargetState(uint8_t state, Source source) {
    if (state == m_targetState && m_currentState == m_targetState) {
        ESP_LOGD(TAG, "Requested state is already the current state. No action taken.");
        return;
    }

    ESP_LOGI(TAG, "Setting target state to %d (c:%d,t:%d), from source %d", state, m_currentState, m_targetState, static_cast<int>(source));

    if(esp_timer_is_active(momentaryStateTimer)) esp_timer_stop(momentaryStateTimer);

    m_targetState = state;

    EventLockState s{
      .currentState = m_currentState,
      .targetState = m_targetState,
      .source = LockManager::INTERNAL
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    if (m_actionsConfig.hkDumbSwitchMode) {
      ESP_LOGI(TAG, "Dummy Action is enabled!");
      m_currentState = m_targetState;
      s.currentState = m_targetState;
      d.clear();
      alpaca::serialize(s, d);
    } else if((source == NFC && m_actionsConfig.hkGpioControlledState) || source != NFC) {
      event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
    }
    event_bus.publish({bus_topic, 0, d.data(), d.size()});

    uint8_t momentarySources = (((m_actionsConfig.gpioActionMomentaryEnabled |
                                  m_actionsConfig.gpioActionPin) == 255) &
                                !m_actionsConfig.hkDumbSwitchMode)
                                   ? 0
                                   : m_actionsConfig.gpioActionMomentaryEnabled;
    bool isMomentarySource = ((static_cast<uint8_t>(source) & momentarySources) != 0);

    if (m_targetState == lockStates::UNLOCKED && isMomentarySource) {
        ESP_LOGI(TAG, "Starting momentary unlock timer for %d ms.", m_actionsConfig.gpioActionMomentaryTimeout);
        esp_timer_start_once(momentaryStateTimer, m_actionsConfig.gpioActionMomentaryTimeout * 1000);
    }
}

/**
 * @brief Apply an external override to the lock's current and/or target state and notify observers.
 *
 * Updates the manager's current and/or target state using the provided values (a value of 255 means "no change"),
 * stops any active momentary-state timer, and publishes the resulting state to the configured event topics so
 * observers and hardware are informed.
 *
 * @param c_state External current state to apply, or 255 to leave the current state unchanged.
 * @param t_state External target state to apply, or 255 to leave the target state unchanged.
 */
void LockManager::overrideState(uint8_t c_state, uint8_t t_state) {
    if (c_state == m_currentState && t_state == m_targetState) return;

    ESP_LOGI(TAG, "External source reported new c_state: %d t_state: %d. Overriding internal state.", c_state, t_state);

    m_currentState = c_state != 255 ? c_state : m_currentState;
    m_targetState = t_state != 255 ? t_state : m_targetState;

    if(momentaryStateTimer && esp_timer_is_active(momentaryStateTimer)) esp_timer_stop(momentaryStateTimer);
    EventLockState s{
      .currentState = static_cast<uint8_t>(m_currentState),
      .targetState = static_cast<uint8_t>(m_targetState),
      .source = LockManager::INTERNAL
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
    event_bus.publish({bus_topic, 0, d.data(), d.size()});
}
