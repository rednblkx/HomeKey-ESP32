#pragma once

// Narrow, host-testable decision for the "may this NFC pin change be
// accepted?" question in the captive-portal misc-config validator. Two
// facts the caller already has (the current persisted value and the
// current owner of the incoming pin) plus one flag (override_strapping)
// map to a single Ok/Conflict verdict. That is enough to fix the bug
// where resaving the whole misc-config form fails on NFC pins that the
// live NfcManager already holds -- and to keep unrelated subsystems
// (Ethernet SPI, HardwareManager action pins, etc.) rejected.
//
// Rules encoded here:
//   - Unchanged: incoming == current -> Ok. Nothing new is being
//     allocated; there is nothing to conflict with.
//   - Free: no current owner -> Ok.
//   - NFC-owned: the current owner is one of NfcManager's own eight
//     lease names -> Ok. The captive-portal save always reboots on
//     success (see esp_restart() in WebServerManager.cpp), which
//     releases the old NfcManager's leases before the new one is
//     constructed on the next boot.
//   - Strapping: only allowed when override_strapping is set.
//   - Anything else: Conflict. Cross-subsystem collisions must not be
//     silently accepted -- in particular ETH_SPI_MISO / ETH_SPI_MOSI /
//     ETH_SPI_SCK are NOT NFC-owned and must NOT be exempted.
//
// If a future NfcManager changes one of its acquire-name strings, the
// list here must move in the same commit; see NfcManager.cpp:258-269.

#include <optional>
#include <string>

namespace webcfg {

inline constexpr const char *kNfcOwnerNames[] = {
    "SPI2_SS",  "SPI2_SCK", "SPI2_MISO", "SPI2_MOSI",  // PN532 / PN7160
    "I2C_SDA",  "I2C_SCL",                             // ST25R3916
    "NFC_IRQ",  "NFC_VEN",                             // PN7160 side pins
};

inline bool isNfcOwner(const std::string &name) {
    for (auto n : kNfcOwnerNames) if (name == n) return true;
    return false;
}

enum class NfcPinDecision { Ok, Conflict };

inline NfcPinDecision decideNfcPin(uint8_t incoming_pin,
                                   uint8_t current_pin,
                                   const std::optional<std::string> &owner_of_incoming,
                                   bool override_strapping) {
    if (incoming_pin == current_pin)      return NfcPinDecision::Ok;
    if (!owner_of_incoming.has_value())   return NfcPinDecision::Ok;
    const std::string &owner = *owner_of_incoming;
    if (isNfcOwner(owner))                return NfcPinDecision::Ok;
    if (owner == "STRAPPING" && override_strapping) return NfcPinDecision::Ok;
    return NfcPinDecision::Conflict;
}

}  // namespace webcfg
