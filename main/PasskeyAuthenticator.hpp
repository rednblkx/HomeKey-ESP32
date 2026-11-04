
#pragma once

#include <stdint.h>
#include <stddef.h>

class PasskeyAuthenticator {
public:
    bool begin();

    // Called when a FIDO2 request arrives
    void onRequest(const uint8_t* data, size_t length);

    // Process pending authentication
    void process();

    // Cancel pending request
    void cancel();
};
