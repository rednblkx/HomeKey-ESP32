#include "app_event_loop.hpp"
#include "esp_log.h"
#include <cstring>
#include <memory>
#include <vector>

namespace AppEventLoop {

static const char* TAG = "AppEventLoop";

static void event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data) {
    (void)base;
    (void)id;
    auto* ctx = static_cast<HandlerContext*>(handler_arg);
    if (!ctx || !ctx->callback) return;

    const uint8_t* data = static_cast<const uint8_t*>(event_data);
    uint16_t size = 0;
    if (data) {
        std::memcpy(&size, data, sizeof(size));
    }
    ctx->callback(data ? data + sizeof(size) : nullptr, size);
}

SubscriptionHandle subscribe(esp_event_base_t base, int32_t id,
                              std::function<void(const uint8_t*, size_t)> callback) {
    auto ctx = std::make_unique<HandlerContext>();
    ctx->callback = std::move(callback);

    esp_event_handler_instance_t instance = nullptr;
    esp_err_t err = esp_event_handler_instance_register(base, id, &event_handler,
                                                         ctx.get(), &instance);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register handler: %d", err);
        return SubscriptionHandle{};
    }

    return SubscriptionHandle(base, id, instance, ctx.release());
}

esp_err_t publish(esp_event_base_t base, int32_t id, const void* data, size_t size) {
    constexpr size_t MAX_PAYLOAD = 256;
    if (size > MAX_PAYLOAD) {
        ESP_LOGW(TAG, "Payload too large (%zu), truncating", size);
        size = MAX_PAYLOAD;
    }

    uint8_t buffer[sizeof(uint16_t) + MAX_PAYLOAD];
    uint16_t len = static_cast<uint16_t>(size);
    std::memcpy(buffer, &len, sizeof(len));
    if (data && size > 0) {
        std::memcpy(buffer + sizeof(len), data, size);
    }

    return esp_event_post(base, id, buffer, sizeof(len) + size, portMAX_DELAY);
}

} // namespace AppEventLoop
