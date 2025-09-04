#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <serialization.hpp>
#include <event_manager.hpp>

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
  std::vector<uint8_t> atqa;
  std::vector<uint8_t> sak;
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
    ALT_ACTION,
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
};

struct HomekitEvent {
    HomekitEventType type;
    std::vector<uint8_t> data;
};
