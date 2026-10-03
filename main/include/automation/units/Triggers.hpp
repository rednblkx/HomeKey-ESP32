#pragma once
/**
 * @file Triggers.hpp
 * @brief Built-in trigger units.
 *
 * Each unit matches one event source; `matches` decodes the payload it needs
 * and applies its filter. Filter semantics per unit are documented inline.
 */
#include "automation/AutomationRegistry.hpp"
#include "eventStructs.hpp"
#include "LockManager.hpp"

namespace automation::units {

// --- lock_state ------------------------------------------------------------
// Listens: LOCK_EVENT / LOCK_STATE_CHANGED (EventLockState).
// Filter: "from" (state before, optional), "to" (state after, optional),
//         "source" (LockManager::Source name, optional; "any" or unset = any).
struct LockStateTrigger {
  static constexpr const char* kType = "lock_state";
  static constexpr const char* kLabel = "Lock state changed";
  static inline const esp_event_base_t kBase = LOCK_EVENT;
  static constexpr int32_t kId = LOCK_STATE_CHANGED;
  // LockManager::lockStates order: UNLOCKED, LOCKED, JAMMED, UNKNOWN, UNLOCKING, LOCKING
  static constexpr const char* kStateOptions[] = {
      "any", "0 (unlocked)", "1 (locked)", "2 (jammed)", "3 (unknown)", "4 (unlocking)", "5 (locking)", nullptr};
  static constexpr const char* kSourceOptions[] = {"any", "internal", "homekit", "nfc", "mqtt", nullptr};
  static constexpr ParamDesc kParams[] = {
      {"from", ParamDesc::Kind::Select, kStateOptions},
      {"to", ParamDesc::Kind::Select, kStateOptions},
      {"source", ParamDesc::Kind::Select, kSourceOptions},
  };
  static bool matches(const EventContext& ctx, const ParamMap& f) {
    EventLockState s;
    std::span<const uint8_t> payload(ctx.data, ctx.size);
    std::error_code ec;
    if (ctx.size == 0 || (s = alpaca::deserialize<EventLockState>(payload, ec), ec)) return false;
    // Option values are "N (label)"; atoi() picks up the leading number and
    // "any" must not filter.
    auto optU8 = [&f](const char* k) -> std::optional<uint8_t> {
      auto it = f.find(k);
      if (it == f.end() || it->second.empty() || it->second == "any") return std::nullopt;
      return static_cast<uint8_t>(atoi(it->second.c_str()));
    };
    // "from" matches prevState — the true pre-transition state carried by the
    // event (dumb-switch mode overwrites currentState with the new target).
    // prevState == 255 means "no transition" (boot announce): fail closed.
    if (auto v = optU8("from")) {
      if (s.prevState == 255 || *v != s.prevState) return false;
    }
    if (auto v = optU8("to"); v && *v != s.targetState) return false;
    if (auto it = f.find("source"); it != f.end() && !it->second.empty() && it->second != "any") {
      uint8_t src = static_cast<uint8_t>(atoi(it->second.c_str()));
      if (src != s.source) return false;
    }
    return true;
  }
};

// --- nfc_tap ---------------------------------------------------------------
// Listens: NFC_EVENT / NFC_TAP_EVENT. The event carries a NfcEvent; for
// HOMEKEY_TAP the inner payload is EventHKTap (status = auth result, issuer/
// endpoint ids on success), for TAG_TAP it is EventTagTap (uid).
// Filter: "type": "any" | "homekey" | "tag",
//         "result": "any" | "success" | "failure" (HomeKey auth result),
//         "issuerId" / "endpointId": hex ids (optional, HomeKey only; either,
//         both or none — both match AND when set),
//         "uid": hex tag UID to match (optional; tag taps only).
struct NfcTapTrigger {
  static constexpr const char* kType = "nfc_tap";
  static constexpr const char* kLabel = "NFC tap";
  static inline const esp_event_base_t kBase = NFC_EVENT;
  static constexpr int32_t kId = NFC_TAP_EVENT;
  static constexpr const char* kTypeOptions[] = {"any", "homekey", "tag", nullptr};
  static constexpr const char* kResultOptions[] = {"any", "success", "failure", nullptr};
  static constexpr ParamDesc kParams[] = {
      {"type", ParamDesc::Kind::Select, kTypeOptions},
      {"result", ParamDesc::Kind::Select, kResultOptions, "type", "homekey"},
      {"issuerId", ParamDesc::Kind::String, nullptr, "type", "homekey"},
      {"endpointId", ParamDesc::Kind::String, nullptr, "type", "homekey"},
      {"uid", ParamDesc::Kind::String, nullptr, "type", "tag"},
  };
  static bool matches(const EventContext& ctx, const ParamMap& f) {
    if (ctx.size == 0) return false;
    std::span<const uint8_t> payload(ctx.data, ctx.size);
    std::error_code ec;
    NfcEvent ev = alpaca::deserialize<NfcEvent>(payload, ec);
    if (ec) return false;
    const bool isHomekey = ev.type == HOMEKEY_TAP;
    const char* typeFilter = "any";
    if (auto it = f.find("type"); it != f.end() && !it->second.empty()) typeFilter = it->second.c_str();
    if (strcmp(typeFilter, "homekey") == 0 && !isHomekey) return false;
    if (strcmp(typeFilter, "tag") == 0 && isHomekey) return false;

    const bool applyHkFilters = isHomekey && strcmp(typeFilter, "tag") != 0;
    const bool applyTagFilters = !isHomekey && strcmp(typeFilter, "homekey") != 0;

    if (applyHkFilters) {
      // "result": HomeKey auth outcome filter.
      const char* resultFilter = "any";
      if (auto it = f.find("result"); it != f.end() && !it->second.empty()) resultFilter = it->second.c_str();
      if (resultFilter[0] != 'a') { // not "any"
        std::error_code ec2;
        EventHKTap tap = alpaca::deserialize<EventHKTap>(ev.data, ec2);
        if (ec2) return false;
        const bool wantSuccess = strcmp(resultFilter, "success") == 0;
        if (tap.status != wantSuccess) return false;
      }
      if (auto it = f.find("issuerId"); it != f.end() && !it->second.empty()) {
        std::error_code ec2;
        EventHKTap tap = alpaca::deserialize<EventHKTap>(ev.data, ec2);
        if (ec2 || !uidBytesMatch(tap.issuerId, it->second)) return false;
      }
      if (auto it = f.find("endpointId"); it != f.end() && !it->second.empty()) {
        std::error_code ec2;
        EventHKTap tap = alpaca::deserialize<EventHKTap>(ev.data, ec2);
        if (ec2 || !uidBytesMatch(tap.endpointId, it->second)) return false;
      }
    }

    if (applyTagFilters) {
      // "uid": hex string ("AAE5B089", ':'/' '/'-' ignored).
      if (auto it = f.find("uid"); it != f.end() && !it->second.empty()) {
        std::error_code ec2;
        EventTagTap tag = alpaca::deserialize<EventTagTap>(ev.data, ec2);
        if (ec2 || !uidBytesMatch(tag.uid, it->second)) return false;
      }
    }
    return true;
  }
  static int hexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
  }
  // Shared hex matcher: filter must parse as whole bytes and equal `bytes`
  // for its full length (separators ':' ' ' '-' ignored, case-insensitive).
  static bool uidBytesMatch(const std::vector<uint8_t>& bytes, const std::string& filter) {
    size_t byteIdx = 0;
    bool high = true;
    int hi = 0;
    for (char c : filter) {
      if (c == ':' || c == ' ' || c == '-') continue;
      const int v = hexVal(c);
      if (v < 0) return false;
      if (high) { hi = v; high = false; continue; }
      if (byteIdx >= bytes.size() || bytes[byteIdx] != static_cast<uint8_t>((hi << 4) | v)) return false;
      ++byteIdx;
      high = true;
    }
    return high && byteIdx == bytes.size();
  }
};

// --- keypad_code -----------------------------------------------------------
// Listens: KEYPAD_EVENT / KEYPAD_CODE_ENTERED (EventKeypadCode).
// Filter: "code": exact match (optional), "prefix": starts-with (optional).
struct KeypadCodeTrigger {
  static constexpr const char* kType = "keypad_code";
  static constexpr const char* kLabel = "Keypad code entered";
  static inline const esp_event_base_t kBase = KEYPAD_EVENT;
  static constexpr int32_t kId = KEYPAD_CODE_ENTERED;
  static constexpr ParamDesc kParams[] = {
      {"code", ParamDesc::Kind::String, nullptr},
      {"prefix", ParamDesc::Kind::String, nullptr},
  };
  static bool matches(const EventContext& ctx, const ParamMap& f) {
    if (ctx.size == 0) return false;
    std::span<const uint8_t> payload(ctx.data, ctx.size);
    std::error_code ec;
    EventKeypadCode s = alpaca::deserialize<EventKeypadCode>(payload, ec);
    if (ec) return false;
    if (auto it = f.find("code"); it != f.end() && !it->second.empty() && s.code != it->second) return false;
    if (auto it = f.find("prefix"); it != f.end() && !it->second.empty() && s.code.rfind(it->second, 0) != 0) return false;
    return true;
  }
};

// --- keypad_doorbell -------------------------------------------------------
// Listens: KEYPAD_EVENT / KEYPAD_DOORBELL. No payload, no filter.
struct KeypadDoorbellTrigger {
  static constexpr const char* kType = "keypad_doorbell";
  static constexpr const char* kLabel = "Doorbell pressed";
  static inline const esp_event_base_t kBase = KEYPAD_EVENT;
  static constexpr int32_t kId = KEYPAD_DOORBELL;
  static constexpr ParamDesc kParams[] = {{"", ParamDesc::Kind::String, nullptr}};
  static bool matches(const EventContext&, const ParamMap&) { return true; }
};

} // namespace automation::units

namespace automation {
using units::LockStateTrigger;
using units::NfcTapTrigger;
using units::KeypadCodeTrigger;
using units::KeypadDoorbellTrigger;

inline constexpr TriggerUnit kTriggers[] = {
    makeTriggerUnit<LockStateTrigger>(),
    makeTriggerUnit<NfcTapTrigger>(),
    makeTriggerUnit<KeypadCodeTrigger>(),
    makeTriggerUnit<KeypadDoorbellTrigger>(),
};
} // namespace automation
