#pragma once
#include "GPIOAllocator.hpp"
#include "Blinker.h" // Blinkable interface for homeSpan.setStatusDevice()
#include "esp_timer.h"
#include "esp_log.h"

#include <functional>
#include <map>
#include <mutex>

/**
 * @brief Arbitrates a PinRole::Led pin shared by several drivers.
 *
 * Wraps a GPIOLease and implements HomeSpan's Blinkable interface so it can be
 * handed to homeSpan.setStatusDevice(). All writes go through the arbiter:
 * while the NFC feedback path holds a preemption, HomeSpan's writes are
 * swallowed, and when the preemption expires the pin returns to the HomeSpan
 * blinker's latest desired level (writes that arrived mid-preemption are
 * recorded, not lost). With a single driver attached the behavior is a plain
 * level write, identical to driving the lease directly.
 *
 * Other components that share the pin (HardwareManager's SUCCESS/FAIL flash)
 * do not need this object: they call the static preempt_pin(), which resolves
 * the arbiter registered for the pin and returns false when the pin is not
 * shared, so the caller can fall back to its plain lease path.
 */
class SharedLed : public Blinkable {
public:
  SharedLed(GPIOAllocator::GPIOLease lease, bool active_high = true)
      : m_lease(std::move(lease)), m_active_high(active_high) {
    if (m_lease.valid()) {
      std::lock_guard lock(s_mutex);
      registry()[static_cast<int>(m_lease.get_pin())] = this;
      const esp_timer_create_args_t args = {
          .callback = &SharedLed::timer_cb,
          .arg = this,
          .dispatch_method = ESP_TIMER_TASK,
          .name = "sharedled",
          .skip_unhandled_events = true,
      };
      if (esp_timer_create(&args, &m_restore_timer) != ESP_OK) {
        ESP_LOGW(TAG, "No esp_timer for SharedLed preemption on pin %d", static_cast<int>(pin()));
        m_restore_timer = nullptr;
      }
    }
  }
  ~SharedLed() {
    if (m_lease.valid()) {
      if (m_restore_timer) {
        esp_timer_stop(m_restore_timer);
        esp_timer_delete(m_restore_timer);
      }
      std::lock_guard lock(s_mutex);
      registry().erase(static_cast<int>(m_lease.get_pin()));
    }
  }
  SharedLed(const SharedLed&) = delete;
  SharedLed& operator=(const SharedLed&) = delete;

  gpio_num_t pin() const { return m_lease.get_pin(); }
  bool valid() const { return m_lease.valid(); }
  explicit operator bool() const { return valid(); }

  // Optional hook run when a preemption ends (e.g. homeSpan.refreshStatusDevice()).
  void set_restore_hook(std::function<void()> hook) { m_restore_hook = std::move(hook); }

  // --- Blinkable interface (HomeSpan status driver) ---
  void on() override { write_home_span(m_active_high); }
  void off() override { write_home_span(!m_active_high); }
  int getPin() override { return static_cast<int>(pin()); }

  void preempt(bool level, uint32_t duration_ms) {
    std::lock_guard lock(s_mutex);
    preempt_locked(level, duration_ms);
  }

  static bool preempt_pin(int pin, bool level, uint32_t duration_ms) {
    std::lock_guard lock(s_mutex);
    auto& reg = registry();
    auto it = reg.find(pin);
    if (it == reg.end() || it->second->m_restore_timer == nullptr) return false;
    it->second->preempt_locked(level, duration_ms);
    return true;
  }

private:
  static constexpr const char* TAG = "SharedLed";
  inline static std::mutex s_mutex;
  static std::map<int, SharedLed*>& registry() {
    static std::map<int, SharedLed*> reg;
    return reg;
  }

  void preempt_locked(bool level, uint32_t duration_ms) {
    if (!m_lease.valid()) return;
    gpio_set_level(pin(), level);
    m_preempted = true;
    if (m_restore_timer) {
      esp_timer_stop(m_restore_timer);
      if (esp_timer_start_once(m_restore_timer, static_cast<uint64_t>(duration_ms) * 1000) != ESP_OK) {
        m_preempted = false;
        gpio_set_level(pin(), off_level());
      }
    } else {
      m_preempted = false;
    }
  }

  bool off_level() const { return !m_active_high; }

  void write_home_span(bool level) {
    std::lock_guard lock(s_mutex);
    if (!m_lease.valid() || m_preempted) return; // swallowed while preempted
    gpio_set_level(pin(), level);
  }

  static void timer_cb(void* arg) {
    auto* self = static_cast<SharedLed*>(arg);
    std::function<void()> hook;
    {
      std::lock_guard lock(s_mutex);
      self->m_preempted = false;
      if (self->m_lease.valid()) {
        gpio_set_level(self->pin(), self->off_level());
      }
      hook = self->m_restore_hook;
    }
    if (hook) hook(); // outside the lock: may call back into on()/off()
  }

  GPIOAllocator::GPIOLease m_lease;
  bool m_active_high;
  bool m_preempted = false;
  esp_timer_handle_t m_restore_timer = nullptr;
  std::function<void()> m_restore_hook;
};
