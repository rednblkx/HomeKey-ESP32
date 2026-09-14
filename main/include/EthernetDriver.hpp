#pragma once
#include <atomic>
#include "config.hpp"

/**
 * Owns ethernet bring-up and the Arduino ethernet event handler.
 *
 * The driver can be started twice in one boot: once during captive-portal
 * provisioning (to verify the module responds) and once at the normal boot
 * path. start() is idempotent — a later call with the same config is a no-op
 * that reports success, so a retry after a failed start is the only way the
 * driver actually re-attempts bring-up.
 */
namespace EthernetDriver {
bool start(const espConfig::misc_config_t& config);
} // namespace EthernetDriver
