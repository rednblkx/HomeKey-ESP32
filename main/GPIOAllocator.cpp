#include "GPIOAllocator.hpp"

GPIOAllocator::PinControl::~PinControl() {
  gpio_reset_pin(pin_);
  GPIOAllocator::instance().release_entry(pin_);
}

std::mutex GPIOAllocator::mutex_;
std::array<GPIOAllocator::PinEntry, GPIO_NUM_MAX> GPIOAllocator::entries_;
