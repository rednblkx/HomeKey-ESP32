#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>
#include <utility>
#include "esp_event.h"

namespace AppEventLoop {

struct HandlerContext {
    std::function<void(const uint8_t*, size_t)> callback;
};

class SubscriptionHandle {
public:
    SubscriptionHandle() = default;
    SubscriptionHandle(esp_event_base_t base, int32_t id, esp_event_handler_instance_t instance,
                       HandlerContext* ctx)
        : m_base(base), m_id(id), m_instance(instance), m_ctx(ctx) {}

    SubscriptionHandle(const SubscriptionHandle&) = delete;
    SubscriptionHandle& operator=(const SubscriptionHandle&) = delete;

    SubscriptionHandle(SubscriptionHandle&& other) noexcept
        : m_base(other.m_base), m_id(other.m_id), m_instance(other.m_instance), m_ctx(other.m_ctx) {
        other.m_instance = nullptr;
        other.m_ctx = nullptr;
    }

    SubscriptionHandle& operator=(SubscriptionHandle&& other) noexcept {
        if (this != &other) {
            reset();
            m_base = other.m_base;
            m_id = other.m_id;
            m_instance = other.m_instance;
            m_ctx = other.m_ctx;
            other.m_instance = nullptr;
            other.m_ctx = nullptr;
        }
        return *this;
    }

    ~SubscriptionHandle() { reset(); }

    bool is_valid() const { return m_instance != nullptr; }

    void reset() {
        if (m_instance) {
            esp_event_handler_instance_unregister(m_base, m_id, m_instance);
            m_instance = nullptr;
        }
        delete m_ctx;
        m_ctx = nullptr;
    }

private:
    esp_event_base_t m_base = nullptr;
    int32_t m_id = 0;
    esp_event_handler_instance_t m_instance = nullptr;
    HandlerContext* m_ctx = nullptr;
};

SubscriptionHandle subscribe(esp_event_base_t base, int32_t id,
                              std::function<void(const uint8_t*, size_t)> callback);

esp_err_t publish(esp_event_base_t base, int32_t id, const void* data, size_t size);

} // namespace AppEventLoop
