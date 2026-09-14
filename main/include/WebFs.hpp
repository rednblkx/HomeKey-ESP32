#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <sys/stat.h>

/**
 * @brief Native LittleFS mount + file helpers for the web UI partition.
 *
 * Replaces the Arduino LittleFS wrapper. Mounts the partition labelled
 * "spiffs" at /www via esp_vfs_littlefs_register and exposes the handful of
 * operations WebServerManager needs (exists/read streaming, mount info,
 * unmount/remount around OTA).
 */
namespace webfs {

/// Mount the web UI partition. Safe to call again (returns true if mounted).
bool mount();
/// Unmount (used around OTA writes to the partition).
void unmount();
/// Partition usage in bytes.
bool usage(size_t &total, size_t &used);

/// true if the file exists under the mount point.
bool exists(const char *path);
/// Open a file for reading; returns nullptr on failure. fclose() to release.
FILE *open_read(const char *path);
/// Read up to `len` bytes; returns bytes read (0 at EOF).
size_t read(FILE *f, uint8_t *buf, size_t len);

} // namespace webfs
