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

/// Mount at /www, formatting the partition if mounting fails.
/// Return false on registration failure; return true if already mounted.
bool mount();
/// Unmount (used around OTA writes to the partition).
void unmount();
/// Fill total and used with partition sizes in bytes.
/// Return false when unmounted or usage cannot be queried.
bool usage(size_t &total, size_t &used);

/// Return whether path names a regular file under /www.
/// path must start with a slash; null paths and an unmounted partition return false.
bool exists(const char *path);
/// Open path (starting with a slash) under /www for binary reading.
/// Return nullptr if unmounted, path is null, or opening fails; fclose() to release.
FILE *open_read(const char *path);
/// Read up to len bytes into buf and advance the stream.
/// Return bytes read; zero can mean EOF, a read error, len == 0, or a null stream.
size_t read(FILE *f, uint8_t *buf, size_t len);

} // namespace webfs
