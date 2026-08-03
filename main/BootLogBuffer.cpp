#include "BootLogBuffer.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <nvs.h>
#include <nvs_flash.h>

namespace bootlog {
namespace {

constexpr const char* TAG = "BootLog";
constexpr const char* kNvsNamespace = "SAVED_DATA";  // same namespace ConfigManager uses
constexpr size_t kLineMax = 256;
constexpr uint16_t kMaxKb = 32;  // sanity bound; the ring lives in heap for good

// The allocation is split in two.
//
// A purely rolling buffer defeats its own purpose here: at VERBOSE this device
// produces enough output to wrap 8 KB in about five seconds, so by the time
// anyone reads it after a crash the boot header -- the reset reason, the thing
// it exists to preserve -- is long gone. Measured, not assumed: a ring read
// minutes after a restart held only the last ~5 s and no reset reason at all.
//
// So the first slice is written once and then frozen, guaranteeing the boot
// sequence survives indefinitely, and the remainder rolls to keep whatever
// happened most recently.
constexpr size_t kPrologueBytes = 2048;

char* s_buf = nullptr;      // single allocation: prologue then ring
size_t s_prologueCap = 0;   // frozen region size
size_t s_prologueLen = 0;   // bytes written before it froze
char* s_ring = nullptr;     // s_buf + s_prologueCap
size_t s_capacity = 0;      // rolling region size
size_t s_head = 0;          // next write position within the rolling region
bool s_wrapped = false;     // has the rolling region overwritten itself
SemaphoreHandle_t s_lock = nullptr;
uint32_t s_dropped = 0;
vprintf_like_t s_prev = nullptr;


/**
 * Log hook, chained on top of whatever LogHook::install() put in place.
 *
 * Deliberately not an ISink. Sinker applies a level filter, and that filter is
 * only set from configuration once ConfigManager has read NVS -- which happens
 * *after* the reset reason is logged. A sink therefore misses precisely the
 * lines this buffer exists to capture. Sitting above LogHook in the vprintf
 * chain records everything ESP_LOG emits, unfiltered and independent of the
 * user's configured log level, which is what a crash buffer wants.
 *
 * The previous handler is always called first, so console and WebSocket output
 * are byte-for-byte unchanged whether or not capture is enabled.
 *
 * The lock is taken with a zero timeout. Dropping a line is strictly better
 * than blocking a logging caller, and far better than deadlocking if something
 * logs from a context that cannot wait.
 */
int hook(const char* fmt, va_list args) {
    int written = 0;
    if (s_prev) {
        va_list copy;
        va_copy(copy, args);
        written = s_prev(fmt, copy);
        va_end(copy);
    }
    if (!s_ring || !s_lock) return written;

    char line[kLineMax];
    const int n = vsnprintf(line, sizeof(line), fmt, args);
    if (n <= 0) return written;
    const size_t len = (n >= (int)sizeof(line)) ? sizeof(line) - 1 : (size_t)n;

    if (xSemaphoreTake(s_lock, 0) != pdTRUE) {
        s_dropped++;
        return written;
    }
    if (s_prologueLen + len <= s_prologueCap) {
        // Still filling the frozen region. Whole lines only: a half-written line
        // at the boundary would be worse than starting the rolling region early.
        memcpy(s_buf + s_prologueLen, line, len);
        s_prologueLen += len;
    } else {
        if (s_prologueLen < s_prologueCap) {
            // Freeze it -- this line did not fit, and nothing later will either.
            s_prologueCap = s_prologueLen;
        }
        for (size_t i = 0; i < len; i++) {
            s_ring[s_head] = line[i];
            s_head = (s_head + 1) % s_capacity;
            if (s_head == 0) s_wrapped = true;
        }
    }
    xSemaphoreGive(s_lock);
    return written;
}

uint16_t readConfiguredKb() {
    nvs_handle_t h;
    if (nvs_open(kNvsNamespace, NVS_READONLY, &h) != ESP_OK) return 0;
    uint16_t kb = 0;
    if (nvs_get_u16(h, kNvsKey, &kb) != ESP_OK) kb = 0;
    nvs_close(h);
    return kb > kMaxKb ? kMaxKb : kb;
}

}  // namespace

void initFromNvs() {
    if (s_ring) return;

    // nvs_flash_init() is normally called later in setup(); doing it here lets
    // the setting be read before anything worth capturing is logged. It is
    // idempotent, so the later call is unaffected.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Deliberately not erasing. A corrupt NVS is for the owner to handle;
        // wiping pairing data to enable a debug buffer would be an appalling trade.
        return;
    }
    if (err != ESP_OK) return;

    const uint16_t kb = readConfiguredKb();
    if (kb == 0) return;

    const size_t bytes = static_cast<size_t>(kb) * 1024;
    s_lock = xSemaphoreCreateMutex();
    if (!s_lock) return;
    s_buf = static_cast<char*>(malloc(bytes));
    if (!s_buf) {
        vSemaphoreDelete(s_lock);
        s_lock = nullptr;
        return;
    }
    // Give the frozen prologue at most a quarter of the allocation, so a small
    // buffer still leaves a usable rolling window.
    s_prologueCap = (kPrologueBytes < bytes / 4) ? kPrologueBytes : bytes / 4;
    s_prologueLen = 0;
    s_ring = s_buf + s_prologueCap;
    s_capacity = bytes - s_prologueCap;
    s_head = 0;
    s_wrapped = false;

    // Installed last so this sits above LogHook in the vprintf chain and
    // therefore sees every message, before Sinker's level filter applies.
    s_prev = esp_log_set_vprintf(hook);

    // esp_log applies its own runtime level check *before* calling any vprintf
    // hook, and until ConfigManager applies the configured level the default is
    // CONFIG_LOG_DEFAULT_LEVEL -- WARN. That silently discards the whole early
    // boot sequence, including the normal-boot "Reset reason" line, which is
    // exactly what this buffer exists to record. (A crash logs at ERROR and
    // would survive, but depending on that would be fragile.) Widen the window;
    // setup() applies the user's real level a few lines later.
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    ESP_LOGI(TAG, "Rolling log buffer enabled: %u KB", static_cast<unsigned>(kb));
}

bool enabled() { return s_buf != nullptr; }
size_t capacity() { return s_prologueCap + s_capacity; }
uint32_t dropped() { return s_dropped; }

size_t size() {
    if (!s_buf) return 0;
    return s_prologueLen + (s_wrapped ? s_capacity : s_head);
}

std::string contents() {
    if (!s_buf || !s_lock) return {};
    std::string out;
    if (xSemaphoreTake(s_lock, pdMS_TO_TICKS(200)) != pdTRUE) return {};
    out.reserve(s_prologueLen + (s_wrapped ? s_capacity : s_head) + 48);
    out.append(s_buf, s_prologueLen);
    if (s_wrapped || s_head) {
        out.append("--- boot prologue ends, rolling window follows ---\n");
        if (s_wrapped) {
            // Oldest data sits after the write head.
            out.append(s_ring + s_head, s_capacity - s_head);
            out.append(s_ring, s_head);
        } else {
            out.append(s_ring, s_head);
        }
    }
    xSemaphoreGive(s_lock);
    return out;
}

void clear() {
    if (!s_buf || !s_lock) return;
    if (xSemaphoreTake(s_lock, pdMS_TO_TICKS(200)) != pdTRUE) return;
    // Only the rolling window. The prologue is boot evidence and clearing it on
    // request would defeat the point -- it is reclaimed by rebooting.
    s_head = 0;
    s_wrapped = false;
    s_dropped = 0;
    xSemaphoreGive(s_lock);
}

}  // namespace bootlog
