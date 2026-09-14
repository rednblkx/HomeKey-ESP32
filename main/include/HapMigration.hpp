#pragma once
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief One-time migration of HomeSpan's NVS state into YAHAP's hap_storage.
 *
 * HomeSpan state lives in NVS namespaces "HAP" (ACCESSORY/CONTROLLERS blobs),
 * "SRP" (VERIFYDATA), "WIFI" (WIFIDATA/REBOOTS), "CHAR" (per-characteristic
 * values) and "OTA". YAHAP keeps everything in "hap_storage" under different
 * keys/layouts. This module parses the HomeSpan blobs, writes the YAHAP keys
 * via the same Esp32Storage PAL class the server uses, verifies the result
 * byte-for-byte, and only then erases the old namespaces. Idempotent: when
 * HAP/ACCESSORY is gone, migration is considered done and this is a no-op.
 */
namespace hap_migration {

struct MigrationStats {
  bool migrated = false;       // a migration was actually performed this boot
  bool skipped = false;        // nothing to migrate (fresh device or already done)
  bool failed = false;         // parse/write/verify failure; old data preserved
  int controllers = 0;         // number of paired controllers migrated
};

/// Run the migration. Call once from setup() before constructing the HAP server.
MigrationStats run();

/// Test-only hooks: parse the raw HomeSpan blobs without touching NVS.
struct AccessoryData {
  std::string id;                  // 17 ASCII bytes "XX:XX:XX:XX:XX:XX"
  std::vector<uint8_t> ltsk;       // 64 bytes
  std::vector<uint8_t> ltpk;       // 32 bytes
};

struct ControllerData {
  std::string id;                  // 36 ASCII bytes (UUID)
  std::vector<uint8_t> ltpk;       // 32 bytes
  bool admin;
};

bool parse_accessory(const uint8_t *blob, size_t len, AccessoryData &out);
bool parse_controllers(const uint8_t *blob, size_t len,
                       std::vector<ControllerData> &out);

} // namespace hap_migration
