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
  std::vector<uint8_t> issuerId;
  std::vector<uint8_t> endpointId;
  std::vector<uint8_t> readerId;
};

struct EventValueChanged {
  std::string name = "";
  uint8_t oldValue = 255;
  uint8_t newValue = 255;
  std::string str = "";
};
