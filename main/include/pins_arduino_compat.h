#pragma once
// Native replacement for Arduino's pins_arduino.h pin aliases.
// Values mirror the arduino-esp32 variant headers for each supported target
// (esp32c6: SS=18 MOSI=19 MISO=20 SCK=21; etc.) so saved configs keep working.

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32C6
static const uint8_t SS = 18;
static const uint8_t MOSI = 19;
static const uint8_t MISO = 20;
static const uint8_t SCK = 21;
static const uint8_t A3 = 3;
static const uint8_t A4 = 4;
static const uint8_t A5 = 5;
#elif CONFIG_IDF_TARGET_ESP32C3
static const uint8_t SS = 7;
static const uint8_t MOSI = 6;
static const uint8_t MISO = 5;
static const uint8_t SCK = 4;
static const uint8_t A3 = 3;
static const uint8_t A4 = 4;
static const uint8_t A5 = 5;
#elif CONFIG_IDF_TARGET_ESP32S3
static const uint8_t SS = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 13;
static const uint8_t SCK = 12;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
#elif CONFIG_IDF_TARGET_ESP32
static const uint8_t SS = 5;
static const uint8_t MOSI = 23;
static const uint8_t MISO = 19;
static const uint8_t SCK = 18;
static const uint8_t A3 = 39;
static const uint8_t A4 = 32;
static const uint8_t A5 = 33;
#else
static const uint8_t SS = 255;
static const uint8_t MOSI = 255;
static const uint8_t MISO = 255;
static const uint8_t SCK = 255;
static const uint8_t A3 = 255;
static const uint8_t A4 = 255;
static const uint8_t A5 = 255;
#endif
