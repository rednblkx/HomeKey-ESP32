
#pragma once

#include <stdint.h>

class PasskeyGate {
public:
    void beginRequest(uint32_t nowMs) {
        pending = true;
        approved = false;
        startedAt = nowMs;
    }

    void onHomeKeyAuthenticated(uint32_t nowMs) {
        if (pending && (uint32_t)(nowMs - startedAt) <= 15000) {
            approved = true;
        }
    }

    bool consumeApproval(uint32_t nowMs) {
        bool allowed = pending && approved &&
            (uint32_t)(nowMs - startedAt) <= 15000;

        cancel();
        return allowed;
    }

    void cancel() {
        pending = false;
        approved = false;
    }

private:
    bool pending = false;
    bool approved = false;
    uint32_t startedAt = 0;
};
