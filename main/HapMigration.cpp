#include "HapMigration.hpp"
#include "Esp32Storage.hpp"
#include "esp_log.h"
#include "fmt/format.h"
#include "nvs.h"

#include <sodium/crypto_sign.h>

#include <cstring>
#include <string>
#include <vector>

namespace hap_migration {

static const char *TAG = "HapMigration";

// ---------------------------------------------------------------------------
// HomeSpan blob layouts (components/HomeSpan/upstream, release-2.1.6):
//
//   struct Accessory {            // HAP/ACCESSORY
//     uint8_t ID[17];             // "XX:XX:XX:XX:XX:XX", no NUL
//     uint8_t LTSK[64];
//     uint8_t LTPK[32];
//   };                            // 113 bytes
//
//   class Controller {            // HAP/CONTROLLERS, packed array
//     boolean allocated;          // 1 byte, nonzero = record valid
//     boolean admin;              // 1 byte
//     uint8_t ID[36];             // ASCII UUID
//     uint8_t LTPK[32];
//   };                            // 70 bytes per record
// ---------------------------------------------------------------------------
static constexpr size_t HS_ACCESSORY_SIZE = 17 + 64 + 32;
static constexpr size_t HS_CONTROLLER_SIZE = 1 + 1 + 36 + 32;

bool parse_accessory(const uint8_t *blob, size_t len, AccessoryData &out) {
  if (len < HS_ACCESSORY_SIZE) {
    ESP_LOGE(TAG, "ACCESSORY blob too small: %zu bytes", len);
    return false;
  }
  out.id.assign(reinterpret_cast<const char *>(blob), 17);
  out.ltsk.assign(blob + 17, blob + 17 + 64);
  out.ltpk.assign(blob + 17 + 64, blob + 17 + 64 + 32);

  // Sanity: the stored LTPK must match the public key derived from LTSK
  // (libsodium Ed25519 secret keys are [seed(32) | pubkey(32)]).
  uint8_t derived_pk[32];
  uint8_t derived_sk[64];
  crypto_sign_seed_keypair(derived_pk, derived_sk, out.ltsk.data());
  if (memcmp(derived_pk, out.ltpk.data(), 32) != 0) {
    ESP_LOGE(TAG, "ACCESSORY LTPK does not match LTSK — refusing migration");
    return false;
  }
  return true;
}

bool parse_controllers(const uint8_t *blob, size_t len,
                       std::vector<ControllerData> &out) {
  if (len % HS_CONTROLLER_SIZE != 0) {
    ESP_LOGE(TAG, "CONTROLLERS blob size %zu not a multiple of %zu", len,
             HS_CONTROLLER_SIZE);
    return false;
  }
  const size_t count = len / HS_CONTROLLER_SIZE;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    const uint8_t *rec = blob + i * HS_CONTROLLER_SIZE;
    if (rec[0] == 0) continue; // allocated == false
    ControllerData c;
    c.admin = rec[1] != 0;
    c.id.assign(reinterpret_cast<const char *>(rec + 2), 36);
    c.ltpk.assign(rec + 2 + 36, rec + 2 + 36 + 32);
    out.push_back(std::move(c));
  }
  return true;
}

// ---------------------------------------------------------------------------
// Write + verify against hap_storage via the same PAL class YAHAP uses.
// ---------------------------------------------------------------------------
namespace {

/// Return the shared storage adapter used for migration writes and readback.
Esp32Storage &storage() {
  static Esp32Storage s;
  return s;
}

/// Write a pairing value and return whether readback exists and matches it.
bool write_and_verify(const char *key, const std::vector<uint8_t> &value) {
  storage().set(key, value);
  auto readback = storage().get(key);
  if (!readback || *readback != value) {
    ESP_LOGE(TAG, "Verify failed for key '%s'", key);
    return false;
  }
  return true;
}

/// Erase and commit an old NVS namespace; a missing namespace counts as success.
/// Return false if opening or committing fails; the erase result is not checked.
bool erase_namespace(const char *ns) {
  nvs_handle_t h;
  esp_err_t err = nvs_open(ns, NVS_READWRITE, &h);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    return true; // namespace never existed
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Cannot open namespace '%s' for erase: %s", ns,
             esp_err_to_name(err));
    return false;
  }
  nvs_erase_all(h);
  err = nvs_commit(h);
  nvs_close(h);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Commit after erase of '%s' failed: %s", ns,
             esp_err_to_name(err));
    return false;
  }
  ESP_LOGI(TAG, "Erased NVS namespace '%s'", ns);
  return true;
}

} // namespace

MigrationStats run() {
  MigrationStats stats;

  nvs_handle_t hap;
  if (nvs_open("HAP", NVS_READONLY, &hap) != ESP_OK) {
    stats.skipped = true;
    return stats; // fresh device
  }

  size_t acc_len = 0;
  esp_err_t err = nvs_get_blob(hap, "ACCESSORY", nullptr, &acc_len);
  if (err != ESP_OK) {
    nvs_close(hap);
    stats.skipped = true; // already migrated or never paired
    return stats;
  }

  std::vector<uint8_t> acc_blob(acc_len);
  nvs_get_blob(hap, "ACCESSORY", acc_blob.data(), &acc_len);

  size_t ctrl_len = 0;
  std::vector<uint8_t> ctrl_blob;
  bool has_controllers =
      nvs_get_blob(hap, "CONTROLLERS", nullptr, &ctrl_len) == ESP_OK && ctrl_len > 0;
  if (has_controllers) {
    ctrl_blob.resize(ctrl_len);
    nvs_get_blob(hap, "CONTROLLERS", ctrl_blob.data(), &ctrl_len);
  }
  nvs_close(hap);

  // WiFi credentials live in HomeSpan's WIFI/WIFIDATA blob (char ssid[33] +
  // char pwd[65]) — copy them to SAVED_DATA/WIFICREDS before the namespace
  // is erased, so a previously-provisioned device reconnects after migration.
  nvs_handle_t wifi;
  if (nvs_open("WIFI", NVS_READONLY, &wifi) == ESP_OK) {
    size_t wlen = 0;
    if (nvs_get_blob(wifi, "WIFIDATA", nullptr, &wlen) == ESP_OK && wlen >= 33) {
      std::vector<uint8_t> wblob(wlen);
      nvs_get_blob(wifi, "WIFIDATA", wblob.data(), &wlen);
      nvs_handle_t saved;
      if (nvs_open("SAVED_DATA", NVS_READWRITE, &saved) == ESP_OK) {
        // Strip NUL padding; HomeSpan stores fixed-size char arrays.
        std::vector<uint8_t> creds(wblob.begin(), wblob.begin() + 33 + 65);
        nvs_set_blob(saved, "WIFICREDS", creds.data(), creds.size());
        nvs_commit(saved);
        nvs_close(saved);
        ESP_LOGI(TAG, "Migrated WiFi credentials from WIFI/WIFIDATA");
      } else {
        ESP_LOGE(TAG, "Cannot open SAVED_DATA for WiFi credential migration");
      }
    } else {
      ESP_LOGI(TAG, "No WIFI/WIFIDATA blob found (device not WiFi-provisioned)");
    }
    nvs_close(wifi);
  }

  AccessoryData acc;
  std::vector<ControllerData> controllers;
  if (!parse_accessory(acc_blob.data(), acc_blob.size(), acc) ||
      !parse_controllers(ctrl_blob.data(), ctrl_blob.size(), controllers)) {
    stats.failed = true;
    return stats;
  }
  stats.controllers = static_cast<int>(controllers.size());

  ESP_LOGI(TAG, "Migrating accessory %s with %d paired controller(s) to hap_storage",
           acc.id.c_str(), stats.controllers);

  // --- Write everything into hap_storage ---
  bool ok = true;
  ok &= write_and_verify("accessory_id",
                         std::vector<uint8_t>(acc.id.begin(), acc.id.end()));
  ok &= write_and_verify("accessory_ltsk", acc.ltsk);
  ok &= write_and_verify("accessory_ltpk", acc.ltpk);

  // pairing_list is a JSON array of UUID strings (YAHAP's own format).
  std::string list = "[";
  for (size_t i = 0; i < controllers.size(); ++i) {
    if (i) list += ",";
    list += fmt::format("\"{}\"", controllers[i].id);
  }
  list += "]";

  for (const auto &c : controllers) {
    const std::string key = "pairing_" + c.id;
    ok &= write_and_verify(key.c_str(), c.ltpk);
    const std::string perm_key = key + "_perm";
    ok &= write_and_verify(perm_key.c_str(), {c.admin ? std::uint8_t{0x01} : std::uint8_t{0x00}});
  }
  ok &= write_and_verify("pairing_list",
                         std::vector<uint8_t>(list.begin(), list.end()));

  // Configuration Number starts at 1; YAHAP bumps it when its own db_hash
  // (computed fresh from the new accessory tree) mismatches.
  ok &= write_and_verify("config_number",
                         std::vector<uint8_t>({'1'}));

  if (!ok) {
    ESP_LOGE(TAG, "Migration verification FAILED — HomeSpan data left intact, "
                  "will retry next boot");
    stats.failed = true;
    return stats;
  }

  // --- Success: wipe the old namespaces ---
  bool erased = erase_namespace("HAP") && erase_namespace("SRP") &&
                erase_namespace("WIFI") && erase_namespace("CHAR") &&
                erase_namespace("OTA");
  if (!erased) {
    // hap_storage is already correct and YAHAP will work; leftover namespaces
    // are harmless (next boot sees HAP/ACCESSORY gone and skips). Do not mark
    // the migration failed — the data migration itself succeeded.
    ESP_LOGW(TAG, "One or more old namespaces could not be erased; ignoring");
  }

  stats.migrated = true;
  ESP_LOGI(TAG, "HomeSpan -> YAHAP NVS migration complete");
  return stats;
}

} // namespace hap_migration
