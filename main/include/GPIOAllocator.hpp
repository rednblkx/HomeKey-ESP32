#pragma once
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>
#include <iterator>
#include <mutex>
#include <optional>
#include <string>


#ifdef CONFIG_IDF_TARGET_ESP32C6
inline constexpr uint8_t RESTRICTED_PINS[] = {
  12, 13, 24, 25, 26, 28, 29, 30
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  8, 9, 15,16, 17, 27
};
#elifdef CONFIG_IDF_TARGET_ESP32
inline constexpr uint8_t RESTRICTED_PINS[] = {
  1, 3, 6, 7, 8, 9, 10, 11, 16, 17
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  0, 2, 4, 5, 12, 15
};
#elifdef CONFIG_IDF_TARGET_ESP32C3
inline constexpr uint8_t RESTRICTED_PINS[] = {
  12, 13, 14, 15, 16, 17, 18, 19
};
inline constexpr uint8_t STRAPPING_PINS[] = {
  2, 4, 5, 6, 7, 8, 9, 20, 21
};
#elifdef CONFIG_IDF_TARGET_ESP32S3
#if defined(CONFIG_ESPTOOLPY_OCT_FLASH) || defined(CONFIG_SPIRAM_MODE_OCT)
inline constexpr uint8_t RESTRICTED_PINS[] = {
  9, 19, 20, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 43, 44
};
#else
inline constexpr uint8_t RESTRICTED_PINS[] = {
  9, 19, 20, 26, 27, 28, 29, 30, 31, 32, 38, 39, 43, 44
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

  class GPIOLease {
  public:
    GPIOLease() = delete;
    GPIOLease(const GPIOLease&) = delete;
    GPIOLease& operator=(const GPIOLease&) = delete;
    GPIOLease(GPIOLease&& other) noexcept : pin_(other.pin_), mode_(other.mode_) {
      other.pin_ = GPIO_NUM_NC;
    }

    GPIOLease& operator=(GPIOLease&& other) noexcept {
      if (this != &other) {
        reset();
        pin_ = other.pin_;
        mode_ = other.mode_;
        other.pin_ = GPIO_NUM_NC;
      }
      return *this;
    }
    ~GPIOLease() { if(pin_ != GPIO_NUM_NC) { ESP_LOGD("GPIOLease", "GPIO pin %d released", pin_); reset(); } };
    void set_level(bool level) { gpio_set_level(pin_, level); };
    bool get_level() const { return gpio_get_level(pin_); };
    gpio_mode_t get_mode() const { return mode_; };
    void set_direction(gpio_mode_t mode) { gpio_set_direction(pin_, mode); };
    void set_pullup(bool val) { val ? gpio_pullup_en(pin_) : gpio_pullup_dis(pin_); };
    void set_pulldown(bool val) { val ? gpio_pulldown_en(pin_) : gpio_pulldown_dis(pin_); };
    gpio_num_t get_pin() const { return pin_; };

  private:
    friend class GPIOAllocator;
    GPIOLease(gpio_num_t pin, gpio_mode_t mode) : pin_(pin), mode_(mode) {gpio_set_direction(pin, mode);};
    void reset() {
      if (pin_ != GPIO_NUM_NC && pin_ >= 0 && pin_ < GPIO_NUM_MAX) {
        gpio_reset_pin(pin_);
        std::lock_guard lock(GPIOAllocator::mutex_);
        GPIOAllocator::owners_[pin_].clear();
      }
    }
    gpio_num_t pin_ = GPIO_NUM_NC;
    gpio_mode_t mode_ = GPIO_MODE_INPUT_OUTPUT_OD;
  };

  std::expected<GPIOLease, GPIOAllocatorError> acquire(gpio_num_t pin, gpio_mode_t mode, const std::string &tag) {
    std::lock_guard lock(mutex_);
    if((uint8_t)pin == (uint8_t)GPIO_NUM_NC){
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_NUM);
    }
    ESP_LOGD("GPIOAllocator", "Allocating GPIO Pin %d for '%s'", pin, tag.c_str());
    if(pin >= GPIO_NUM_MAX){
      ESP_LOGE("GPIOAllocator", "'%d' Outside gpio number pin", pin);
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_NUM);
    }
    if(std::find_if(std::begin(RESTRICTED_PINS), std::end(RESTRICTED_PINS), [&](auto e){ return e == pin;}) != std::end(RESTRICTED_PINS)){
      ESP_LOGE("GPIOAllocator", "GPIO Pin %d restricted, reserved by internal function!", pin);
      return std::unexpected<GPIOAllocatorError>(RESTRICTED);
    }
    if(std::find_if(std::begin(STRAPPING_PINS), std::end(STRAPPING_PINS), [&](auto e){ return e == pin;}) != std::end(STRAPPING_PINS)){
      ESP_LOGW("GPIOAllocator", "GPIO Pin %d is a strapping pin, using it may have unexpected consequences!", pin);
    }
    if(!GPIO_IS_VALID_GPIO(pin)){
      ESP_LOGE("GPIOAllocator", "INVALID GPIO NUMBER!");
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_NUM);
    }
    if((mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_INPUT_OUTPUT || mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_INPUT_OUTPUT_OD) && !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
      ESP_LOGE("GPIOAllocator", "Invalid GPIO Direction!");
      return std::unexpected<GPIOAllocatorError>(INVALID_GPIO_DIRECTION);
    }
    if(!owners_[pin].empty()){
      ESP_LOGE("GPIOAllocator", "GPIO %d already owned by '%s'!", pin, owners_[pin].c_str());
      return std::unexpected<GPIOAllocatorError>(ALREADY_OWNED);
    }
    owners_[pin] = tag;
    return GPIOLease(pin, mode);
  }

  [[nodiscard]] std::optional<std::string> owner_of(uint8_t pin) const {
    std::lock_guard lock(mutex_);
    if (pin >= owners_.size() || owners_[pin].empty()) {
        if(std::find_if(std::begin(RESTRICTED_PINS), std::end(RESTRICTED_PINS), [&](auto e){ return e == pin;}) != std::end(RESTRICTED_PINS)){
          ESP_LOGE("GPIOAllocator", "GPIO Pin %d restricted, reserved by internal function!", pin);
          return "INTERNAL";
        }
        if(std::find_if(std::begin(STRAPPING_PINS), std::end(STRAPPING_PINS), [&](auto e){ return e == pin;}) != std::end(STRAPPING_PINS)){
          ESP_LOGW("GPIOAllocator", "GPIO Pin %d is a strapping pin, using it may have unexpected consequences!", pin);
          return "STRAPPING";
        }
        return std::nullopt;
    }
    ESP_LOGD("GPIOAllocator", "Owners Size: %d Pin: %d Owner: %s", owners_.size(), pin, owners_[pin].c_str());
    return owners_[pin];
  }
private:
  GPIOAllocator() = default;
  ~GPIOAllocator() = default;
  GPIOAllocator(const GPIOAllocator&) = delete;
  GPIOAllocator& operator=(const GPIOAllocator&) = delete;
  GPIOAllocator(GPIOAllocator&&) = delete;
  GPIOAllocator& operator=(GPIOAllocator&&) = delete;
  friend class GPIOLease;
  static std::mutex mutex_;
  static std::array<std::string, GPIO_NUM_MAX> owners_;
};
