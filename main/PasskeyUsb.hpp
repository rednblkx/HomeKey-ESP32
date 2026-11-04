
#pragma once

#include <stdint.h>
#include <stddef.h>

class PasskeyUsb {
public:
    bool begin();
    void process();

    // Called when USB receives a FIDO2 packet
    void onPacket(const uint8_t* data, size_t length);

    // Send response to computer
    bool sendPacket(const uint8_t* data, size_t length);
};
