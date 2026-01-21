#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <serialization.hpp>

#define LOCK_BUS_TOPIC "lock/stateChanged"
#define LOCK_UPDATE_BUS_TOPIC "lock/updateState"
#define LOCK_T_STATE_CHANGED "lock/targetStateChanged"
#define LOCK_O_STATE_CHANGED "lock/overrideState"
#define HK_BUS_TOPIC "homekit/internal"
#define NFC_BUS_TOPIC "nfc/event"
#define MQTT_BUS_TOPIC "mqtt/event"
#define HARDWARE_ACTION_BUS_TOPIC "hardware/action"
#define HARDWARE_ALT_ACTION_BUS_TOPIC "hardware/altAction"
#define HARDWARE_CONFIG_BUS_TOPIC "hardware/gpioPinChanged"
#define DOOR_STATE_BUS_TOPIC "door/stateChanged"

struct EventLockState {
  uint8_t currentState = 255;
  uint8_t targetState = 255;
  uint8_t source = 0;
};

struct EventBinaryStatus {
  bool status;
};

struct EventHKTap {
  bool status;
  std::vector<uint8_t> issuerId;
  std::vector<uint8_t> endpointId;
  std::vector<uint8_t> readerId;
};

struct EventTagTap {
  std::vector<uint8_t> uid;
  std::array<uint8_t,2> atqa;
  uint8_t sak;
};

struct EventValueChanged {
  std::string name = "";
  uint8_t oldValue = 255;
  uint8_t newValue = 255;
  std::string str = "";
};

struct EventDoorState {
  uint8_t doorState = 255;  // 0 = Open, 1 = Closed, 2 = Opening, 3 = Closing, 4 = Stopped
};

enum NfcEventType : uint8_t {
    HOMEKEY_TAP,
    TAG_TAP,
    UPDATE_ECP,
    FORCE_AUTH_FLOW
};

struct NfcEvent {
    NfcEventType type;
    std::vector<uint8_t> data;
};

enum HomekitEventType : uint8_t {
    SETUP_CODE_CHANGED,
    BTR_PROP_CHANGED,
    ACCESSDATA_CHANGED,
    DEBUG_AUTH_FLOW
};

struct HomekitEvent {
    HomekitEventType type;
    std::vector<uint8_t> data;
};
