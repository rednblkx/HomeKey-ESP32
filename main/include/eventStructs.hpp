#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <serialization.hpp>
#include "app_events.hpp"

enum class MqttErrorCode : uint8_t {
    NONE = 0,
    CONNECTION_REFUSED = 1,
    AUTH_FAILED = 2,
    NETWORK_ERROR = 3,
    SSL_ERROR = 4,
    TIMEOUT = 5,
    UNKNOWN = 255
};

struct EventMqttStatus {
    bool connected;
    MqttErrorCode errorCode;
    std::string errorMessage;
};

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

struct EventNfcStatus {
    bool connected;
    uint8_t firmwareVersionMajor;
    uint8_t firmwareVersionMinor;
};
