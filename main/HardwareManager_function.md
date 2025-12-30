# HardwareManager Module

## Description
The `HardwareManager` serves as the Hardware Abstraction Layer (HAL). It translates logical commands from the application into physical electrical signals to control the lock mechanism, LEDs, and buzzer.

## Key Responsibilities
- **GPIO Control**: Manages the input/output pins for the relay/solenoid, LEDs, and buttons.
- **Feedback**: Controls the buzzer and LED patterns to provide user feedback (e.g., success beep, error flash).
- **Button Handling**: Debounces and interprets physical button presses to trigger local lock/unlock actions.
- **Battery Monitoring**: (If implemented) Reads battery voltage and reports status.

## Dependencies
- **espp::EventManager**: Subscribes to `lock/action` events to trigger physical hardware.
- **ConfigManager**: Reads hardware configuration (pin mappings, signal logic levels).

## Key Files
- `main/HardwareManager.cpp`
- `main/include/HardwareManager.hpp`
