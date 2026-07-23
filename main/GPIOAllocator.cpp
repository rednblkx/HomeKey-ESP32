#include "GPIOAllocator.hpp"
std::mutex GPIOAllocator::mutex_;
std::array<std::string, GPIO_NUM_MAX> GPIOAllocator::owners_;
