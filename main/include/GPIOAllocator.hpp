#pragma once
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include "config.hpp" // PIN_UNSET sentinel
#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>


#ifdef CONFIG_IDF_TARGET_ESP32C6
inline constexpr uint8_t RESTRICTED_PINS[] = {
  12, 13, 24, 25, 26, 28, 29, 30
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  8, 9, 15,16, 17, 27
};
#elifdef CONFIG_IDF_TARGET_ESP32
inline constexpr uint8_t RESTRICTED_PINS[] = {
  1, 3, 6, 7, 8, 11, 16, 17
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  0, 2
};
#elifdef CONFIG_IDF_TARGET_ESP32C3
inline constexpr uint8_t RESTRICTED_PINS[] = {
  12, 13, 14, 15, 16, 17, 18, 19
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  2, 8, 9, 20, 21
};
#elifdef CONFIG_IDF_TARGET_ESP32S3
#if defined(CONFIG_ESPTOOLPY_OCT_FLASH) || defined(CONFIG_SPIRAM_MODE_OCT)
inline constexpr uint8_t RESTRICTED_PINS[] = {
  9, 19, 20, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 43, 44
};
#else
inline constexpr uint8_t RESTRICTED_PINS[] = {
  9, 19, 20, 26, 27, 28, 29, 30, 31, 32, 38, 43, 44
};
#endif
inline constexpr uint8_t STRAPPING_PINS[] = {
  0, 3, 45, 46, 47, 48
};
#else
inline constexpr uint8_t RESTRICTED_PINS[] = {};
inline constexpr uint8_t STRAPPING_PINS[] = {};
#endif

class GPIOAllocator {
public:
  static GPIOAllocator& instance() {
    static GPIOAllocator instance;
    return instance;
  }
  enum GPIOAllocatorError {
    INVALID_GPIO_NUM = 0,
    INVALID_GPIO_DIRECTION = 1,
    ALREADY_OWNED = 2,
    RESTRICTED = 3
  };

  enum class PinConsumer : uint8_t {
    None = 0,
    Hardware,
    Nfc,
    Eth,
    HomeKit
  };

  enum class PinRole : uint8_t {
    SpiSck = 0, SpiMiso, SpiMosi, SpiCs,   // SCK/MISO/MOSI passive; CS exclusive per device
    I2cSda, I2cScl,                        // passive I2C bus pins
    NfcIrq, NfcVen,                        // PN7160 side pins
    EthIrq, EthRst,                        // SPI ethernet side pins
    Irq,                                   // generic interrupt input
    Led,                                   // active-driven LED, shareable with other Led claims
    GpioOut, GpioIn                        // plain exclusive GPIO
  };

  static bool role_is_passive(PinRole role) {
    return (role >= PinRole::SpiSck && role <= PinRole::SpiMosi) ||
           (role >= PinRole::I2cSda && role <= PinRole::I2cScl);
  }

  struct PinHolder {
    PinConsumer consumer;
    PinRole role;
    const char* tag;
    uint32_t claim_id;
  };

  struct PinStatus {
    bool restricted = false;
    bool strapping = false;
    std::vector<PinHolder> holders;
    bool claimed() const { return !holders.empty(); }
  };

  struct PinControl {
    gpio_num_t pin() const { return pin_; }
    gpio_mode_t mode() const { return mode_; }
    ~PinControl();
  private:
    friend class GPIOAllocator;
    PinControl(gpio_num_t pin, gpio_mode_t mode) : pin_(pin), mode_(mode) { gpio_set_direction(pin_, mode_); }
    gpio_num_t pin_;
    gpio_mode_t mode_;
  };

  struct Claim {
    gpio_num_t pin;
    uint32_t id;
  };

  class GPIOLease {
  public:
    GPIOLease() = default;
    GPIOLease(const GPIOLease&) = default;
    GPIOLease& operator=(const GPIOLease&) = default;
    GPIOLease(GPIOLease&&) = default;
    GPIOLease& operator=(GPIOLease&&) = default;

    bool valid() const { return control_ != nullptr; }
    explicit operator bool() const { return valid(); }
    gpio_num_t get_pin() const { return control_ ? control_->pin_ : GPIO_NUM_NC; }
    gpio_mode_t get_mode() const { return control_ ? control_->mode_ : GPIO_MODE_DISABLE; }
    void set_level(bool level) {
      if (!control_) { ESP_LOGD("GPIOLease", "set_level on unconfigured/released lease"); return; }
      gpio_set_level(control_->pin_, level);
    }
    bool get_level() const { return control_ ? gpio_get_level(control_->pin_) : 0; }
    void set_direction(gpio_mode_t mode) {
      if (!control_) { ESP_LOGD("GPIOLease", "set_direction on unconfigured/released lease"); return; }
      gpio_set_direction(control_->pin_, mode);
    }
    void set_pullup(bool val) {
      if (!control_) { ESP_LOGD("GPIOLease", "set_pullup on unconfigured/released lease"); return; }
      val ? gpio_pullup_en(control_->pin_) : gpio_pullup_dis(control_->pin_);
    }
    void set_pulldown(bool val) {
      if (!control_) { ESP_LOGD("GPIOLease", "set_pulldown on unconfigured/released lease"); return; }
      val ? gpio_pulldown_en(control_->pin_) : gpio_pulldown_dis(control_->pin_);
    }

  private:
    friend class GPIOAllocator;
    explicit GPIOLease(std::shared_ptr<PinControl> control, std::shared_ptr<Claim> claim)
      : control_(std::move(control)), claim_(std::move(claim)) {}
    std::shared_ptr<PinControl> control_;
    std::shared_ptr<Claim> claim_;
  };

  std::expected<GPIOLease, GPIOAllocatorError> acquire(gpio_num_t pin, gpio_mode_t mode,
                                                       PinRole role, PinConsumer consumer,
                                                       const char* tag) {
    std::lock_guard lock(mutex_);
    // 255 = "this function has no pin" is a normal config state, not a
    // claim: succeed with an empty lease whose accessors safely no-op.
    if ((uint8_t)pin == PIN_UNSET) {
      return GPIOLease{};
    }
    if (auto verdict = validate_locked(pin, mode, role, consumer); !verdict) {
      ESP_LOGE("GPIOAllocator", "GPIO %d %s (requested by '%s')", pin,
               error_str(verdict.error()), tag);
      return std::unexpected<GPIOAllocatorError>(verdict.error());
    }
    ESP_LOGD("GPIOAllocator", "Allocating GPIO Pin %d for '%s'", pin, tag);
    if (std::find_if(std::begin(STRAPPING_PINS), std::end(STRAPPING_PINS), [&](auto e){ return e == pin;}) != std::end(STRAPPING_PINS)) {
      ESP_LOGW("GPIOAllocator", "GPIO Pin %d is a strapping pin!", pin);
    }

    auto& entry = entries_[static_cast<size_t>(pin)];
    auto control = entry.control.lock();
    if (!control) {
      control = std::shared_ptr<PinControl>(new PinControl(pin, mode));
      entry.control = control;
    }
    uint32_t claim_id = next_claim_id_++;
    entry.holders.push_back({consumer, role, tag, claim_id});
    auto claim = std::shared_ptr<Claim>(new Claim{pin, claim_id}, [](Claim* c) {
      GPIOAllocator::instance().release_claim(c);
      delete c;
    });
    return GPIOLease(std::move(control), std::move(claim));
  }

  std::expected<void, GPIOAllocatorError> validate(gpio_num_t pin, gpio_mode_t mode,
                                                   PinRole role, PinConsumer consumer,
                                                   PinConsumer ignore_consumer = PinConsumer::None) const {
    std::lock_guard lock(mutex_);
    if ((uint8_t)pin == PIN_UNSET) {
      return {}; // "no pin" is always a valid configuration choice
    }
    return validate_locked(pin, mode, role, consumer, ignore_consumer);
  }

  static constexpr const char* error_str(GPIOAllocatorError err) {
    switch (err) {
      case INVALID_GPIO_NUM:        return "is not a valid GPIO number";
      case INVALID_GPIO_DIRECTION:  return "cannot be used with the requested direction";
      case ALREADY_OWNED:           return "is already claimed by an incompatible component/role";
      case RESTRICTED:              return "is restricted, reserved by internal functions";
    }
    return "has an unknown validation error";
  }

  static constexpr const char* consumer_str(PinConsumer consumer) {
    switch (consumer) {
      case PinConsumer::None:     return "None";
      case PinConsumer::Hardware: return "Hardware";
      case PinConsumer::Nfc:      return "Nfc";
      case PinConsumer::Eth:      return "Eth";
      case PinConsumer::HomeKit:  return "HomeKit";
    }
    return "Unknown";
  }

  [[nodiscard]] PinStatus status_of(uint8_t pin) const {
    std::lock_guard lock(mutex_);
    PinStatus status;
    if (pin >= entries_.size()) {
      return status;
    }
    if(std::find_if(std::begin(RESTRICTED_PINS), std::end(RESTRICTED_PINS), [&](auto e){ return e == pin;}) != std::end(RESTRICTED_PINS)){
      status.restricted = true;
    }
    if(std::find_if(std::begin(STRAPPING_PINS), std::end(STRAPPING_PINS), [&](auto e){ return e == pin;}) != std::end(STRAPPING_PINS)){
      status.strapping = true;
    }
    status.holders = entries_[pin].holders;
    return status;
  }

  [[nodiscard]] std::optional<std::string> owner_of(uint8_t pin) const {
    std::lock_guard lock(mutex_);
    if (pin >= entries_.size()) {
      return std::nullopt;
    }
    const auto& holders = entries_[pin].holders;
    if (!holders.empty()) {
      return join_holders(holders);
    }
    if(std::find_if(std::begin(RESTRICTED_PINS), std::end(RESTRICTED_PINS), [&](auto e){ return e == pin;}) != std::end(RESTRICTED_PINS)){
      ESP_LOGE("GPIOAllocator", "GPIO Pin %d restricted, reserved by internal function!", pin);
      return std::string("INTERNAL");
    }
    if(std::find_if(std::begin(STRAPPING_PINS), std::end(STRAPPING_PINS), [&](auto e){ return e == pin;}) != std::end(STRAPPING_PINS)){
      ESP_LOGW("GPIOAllocator", "GPIO Pin %d is a strapping pin, using it may have unexpected consequences!", pin);
      return std::string("STRAPPING");
    }
    return std::nullopt;
  }
private:
  GPIOAllocator() = default;
  ~GPIOAllocator() = default;
  GPIOAllocator(const GPIOAllocator&) = delete;
  GPIOAllocator& operator=(const GPIOAllocator&) = delete;
  GPIOAllocator(GPIOAllocator&&) = delete;
  GPIOAllocator& operator=(GPIOAllocator&&) = delete;

  void release_claim(const Claim* claim) {
    std::lock_guard lock(mutex_);
    if ((uint8_t)claim->pin >= entries_.size()) return;
    auto& holders = entries_[static_cast<size_t>(claim->pin)].holders;
    std::erase_if(holders, [&](const PinHolder& h) { return h.claim_id == claim->id; });
  }

  struct PinEntry {
    std::vector<PinHolder> holders;
    std::weak_ptr<PinControl> control;
  };

  static bool shareable(PinRole role, PinConsumer consumer, const PinEntry& entry,
                        PinConsumer ignore_consumer = PinConsumer::None) {
    if (entry.holders.empty()) return true;
    if (role_is_passive(role)) {
      return std::all_of(entry.holders.begin(), entry.holders.end(),
                         [&](const PinHolder& h) { return h.role == role || h.consumer == ignore_consumer; });
    }
    if (role == PinRole::Led) {
      return std::all_of(entry.holders.begin(), entry.holders.end(),
                         [&](const PinHolder& h) { return h.role == PinRole::Led || h.consumer == ignore_consumer; });
    }
    return entry.holders.size() == 1 && entry.holders[0].consumer == ignore_consumer;
  }

  std::expected<void, GPIOAllocatorError> validate_locked(gpio_num_t pin, gpio_mode_t mode,
                                                          PinRole role, PinConsumer consumer,
                                                          PinConsumer ignore_consumer = PinConsumer::None) const {
    if ((uint8_t)pin == (uint8_t)GPIO_NUM_NC || pin >= GPIO_NUM_MAX || !GPIO_IS_VALID_GPIO(pin)) {
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_NUM);
    }
    if (std::find_if(std::begin(RESTRICTED_PINS), std::end(RESTRICTED_PINS), [&](auto e){ return e == pin;}) != std::end(RESTRICTED_PINS)) {
      return std::unexpected<GPIOAllocatorError>(RESTRICTED);
    }
    if ((mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_INPUT_OUTPUT || mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_INPUT_OUTPUT_OD) && !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_DIRECTION);
    }
    if (pin < entries_.size() && !entries_[static_cast<size_t>(pin)].holders.empty()) {
      const auto& entry = entries_[static_cast<size_t>(pin)];
      if (entry.control.expired()) {
        return std::expected<void, GPIOAllocatorError>{}; // racing release, pin is free
      }
      if (!shareable(role, consumer, entry, ignore_consumer)) {
        return std::unexpected<GPIOAllocatorError>(ALREADY_OWNED);
      }
    }
    return std::expected<void, GPIOAllocatorError>{};
  }

  static std::string join_holders(const std::vector<PinHolder>& holders) {
    std::string out;
    for (const auto& h : holders) {
      if (!out.empty()) out += ", ";
      out += h.tag;
    }
    return out;
  }

  void release_entry(gpio_num_t pin) {
    if ((uint8_t)pin == (uint8_t)GPIO_NUM_NC || pin >= GPIO_NUM_MAX) return;
    std::lock_guard lock(mutex_);
    auto& entry = entries_[static_cast<size_t>(pin)];
    if (entry.control.expired()) {
      entry.holders.clear();
    }
  }

  static std::mutex mutex_;
  static std::array<PinEntry, GPIO_NUM_MAX> entries_;
  uint32_t next_claim_id_ = 0;
};
