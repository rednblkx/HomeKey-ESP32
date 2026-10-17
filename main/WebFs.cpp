#include "WebFs.hpp"
#include "esp_littlefs.h"
#include "esp_log.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

namespace webfs {

static const char *TAG = "WebFs";
static constexpr const char *BASE_PATH = "/www";
static constexpr const char *PARTITION_LABEL = "spiffs";
static std::atomic<bool> s_mounted{false};

bool mount() {
  if (s_mounted) return true;

  esp_vfs_littlefs_conf_t conf = {};
  conf.base_path = BASE_PATH;
  conf.partition_label = PARTITION_LABEL;
  conf.format_if_mount_failed = true;

  esp_err_t err = esp_vfs_littlefs_register(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "LittleFS mount failed: %s", esp_err_to_name(err));
    return false;
  }

  size_t total = 0, used = 0;
  if (esp_littlefs_info(PARTITION_LABEL, &total, &used) == ESP_OK) {
    ESP_LOGI(TAG, "LittleFS mounted: %u/%u bytes", static_cast<unsigned>(used),
             static_cast<unsigned>(total));
  }
  s_mounted = true;
  return true;
}

void unmount() {
  if (!s_mounted) return;
  esp_vfs_littlefs_unregister(PARTITION_LABEL);
  s_mounted = false;
}

bool usage(size_t &total, size_t &used) {
  if (!s_mounted) return false;
  return esp_littlefs_info(PARTITION_LABEL, &total, &used) == ESP_OK;
}

bool exists(const char *path) {
  if (!s_mounted || !path) return false;
  char full[128];
  snprintf(full, sizeof(full), "%s%s", BASE_PATH, path);
  struct stat st;
  return stat(full, &st) == 0 && S_ISREG(st.st_mode);
}

FILE *open_read(const char *path) {
  if (!s_mounted || !path) return nullptr;
  char full[128];
  snprintf(full, sizeof(full), "%s%s", BASE_PATH, path);
  return fopen(full, "rb");
}

size_t read(FILE *f, uint8_t *buf, size_t len) {
  if (!f) return 0;
  return fread(buf, 1, len, f);
}

} // namespace webfs
