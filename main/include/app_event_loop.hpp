#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>
#include <utility>
#include "esp_event.h"

namespace AppEventLoop {

class SubscriptionHandle {
public:
    SubscriptionHandle() = default;
    SubscriptionHandle(esp_event_base_t base, int32_t id, esp_event_handler_instance_t instance)
        : m_base(base), m_id(id), m_instance(instance) {}

    SubscriptionHandle(const SubscriptionHandle&) = delete;
    SubscriptionHandle& operator=(const SubscriptionHandle&) = delete;

    SubscriptionHandle(SubscriptionHandle&& other) noexcept
        : m_base(other.m_base), m_id(other.m_id), m_instance(other.m_instance) {
        other.m_instance = nullptr;
    }

    SubscriptionHandle& operator=(SubscriptionHandle&& other) noexcept {
        if (this != &other) {
            reset();
            m_base = other.m_base;
            m_id = other.m_id;
            m_instance = other.m_instance;
            other.m_instance = nullptr;
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
    }

private:
    esp_event_base_t m_base = nullptr;
    int32_t m_id = 0;
    esp_event_handler_instance_t m_instance = nullptr;
};

SubscriptionHandle subscribe(esp_event_base_t base, int32_t id,
                              std::function<void(const uint8_t*, size_t)> callback);

esp_err_t publish(esp_event_base_t base, int32_t id, const void* data, size_t size);

} // namespace AppEventLoop
