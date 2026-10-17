#pragma once
/**
 * @file GpioLeaseCache.hpp
 * @brief Long-lived GPIO leases for automation actions.
 *
 * GPIOAllocator resets a pin to its default (input) state when the last lease
 * on it dies (~PinControl -> gpio_reset_pin), so a transient acquire can't
 * latch a level: gpio_set would undo itself the moment the action returns.
 * The engine therefore keeps one lease per pin for its lifetime; a gpio_set
 * drives the pin and the level stays until another action (or rule reload)
 * changes it. Leases for pins no longer referenced by any rule are dropped on
 * reload via retainOnly().
 */
#include "GPIOAllocator.hpp"
#include "driver/gpio.h"
#include <cstdint>
#include <map>
#include <set>

namespace automation {

class GpioLeaseCache {
public:
  GPIOAllocator::GPIOLease* leaseFor(uint8_t pin, const char* tag) {
    auto it = m_leases.find(pin);
    if (it != m_leases.end()) return &it->second;
    auto res = GPIOAllocator::instance().acquire(
        static_cast<gpio_num_t>(pin), GPIO_MODE_OUTPUT,
        GPIOAllocator::PinRole::GpioOut, GPIOAllocator::PinConsumer::HomeKit, tag);
    if (!res.has_value()) return nullptr;
    gpio_hold_dis(static_cast<gpio_num_t>(pin));
    return &m_leases.emplace(pin, std::move(*res)).first->second;
  }

  void retainOnly(const std::set<uint8_t>& pins) {
    std::erase_if(m_leases, [&](const auto& kv) { return !pins.contains(kv.first); });
  }

private:
  std::map<uint8_t, GPIOAllocator::GPIOLease> m_leases;
};

} // namespace automation
