#pragma once

// ST25R3916 (I2C) implementation of HomeKey-ESP32's INfcReader.
//
// Drop-in alternative to Pn532Reader / Pn7160Reader, developed against an
// M5Stack Unit NFC (ST25R3916-AQWT, I2C address 0x50) on an M5Stack AtomS3 Lite
// over a Grove connector.
//
// Where this differs from the PN532 backend, and why it matters:
//
//   * The PN532 performs RATS inside InListPassiveTarget and handles ISO-DEP
//     block framing inside InDataExchange. The ST25R3916 is a bare transceiver,
//     so both are implemented here in software -- pollForTag() runs RATS after
//     anticollision, and exchangeApdu() does I-block framing with block-number
//     toggling and S(WTX) handling.
//   * The chip verifies a received CRC but leaves those bytes in the FIFO, so
//     they are trimmed before returning payloads to callers.
//   * The Grove connector does not break out the ST25R3916 IRQ pin, so all
//     waits poll the interrupt status registers over I2C rather than using an
//     interrupt line. Measured latencies make this comfortable: oscillator
//     stable in ~1.2 ms, ISO14443-A responses in ~0.6 ms.
//
// The ECP frame is used exactly as NfcManager builds it (18 bytes, CRC already
// appended by Utils::crc16a) and is transmitted with the chip's automatic CRC
// disabled. It is held by reference, so NfcManager::updateEcpData() is picked up
// without any further notification -- hence updateECP() is a no-op, matching
// Pn532Reader.

#include "NfcReader.hpp"

#include <driver/i2c_master.h>

#include <array>
#include <cstdint>
#include <vector>

class St25r3916Reader : public INfcReader {
public:
    /**
     * @param gpioPins  Reuses NfcManager's 4-pin array: [0] = SDA, [1] = SCL.
     *                  Entries [2] and [3] are unused (they are MISO/MOSI for
     *                  the SPI-based PN532 backend).
     * @param ecpData   18-byte ECP frame owned by NfcManager, held by reference.
     * @param i2cAddr   ST25R3916 I2C address. 0x50 on the M5Stack Unit NFC.
     * @param i2cHz     Bus speed. Must be hardware I2C at 400 kHz -- M5Stack
     *                  document that software I2C latency breaks RF timing.
     */
    St25r3916Reader(const std::array<uint8_t, 4>& gpioPins,
                    const std::array<uint8_t, 18>& ecpData,
                    uint8_t i2cAddr = 0x50,
                    uint32_t i2cHz = 400000);
    ~St25r3916Reader() override;

    St25r3916Reader(const St25r3916Reader&) = delete;
    St25r3916Reader& operator=(const St25r3916Reader&) = delete;
    St25r3916Reader(St25r3916Reader&&) = delete;
    St25r3916Reader& operator=(St25r3916Reader&&) = delete;

    bool init() override;
    void stop() override;
    bool isConnected() const override;

    uint8_t getFwMajor() const override { return m_icType; }
    uint8_t getFwMinor() const override { return m_icRev; }

    bool beginDiscovery() override;
    bool pollForTag(std::vector<uint8_t>& uid,
                    std::array<uint8_t, 2>& atqa,
                    uint8_t& sak,
                    uint32_t timeoutMs) override;
    bool isTagStillPresent() override;
    void releaseTag() override;
    void endDiscovery() override;

    bool exchangeApdu(const std::vector<uint8_t>& send,
                      std::vector<uint8_t>& recv,
                      uint32_t timeoutMs) override;
    bool healthCheck() override;

    // ECP data is held by reference and re-read on every poll, so there is
    // nothing to push to the chip when NfcManager regenerates it.
    bool updateECP() override { return true; }

private:
    // ---- low level bus access -------------------------------------------
    bool writeReg(uint8_t reg, uint8_t val);
    bool readReg(uint8_t reg, uint8_t& out);
    uint8_t readRegOr(uint8_t reg, uint8_t fallback = 0);
    bool modifyReg(uint8_t reg, uint8_t clearMask, uint8_t setMask);
    bool command(uint8_t cmd);
    bool writeFifo(const uint8_t* data, size_t len);
    size_t readFifo(uint8_t* out, size_t maxLen);
    uint16_t fifoLen();

    // ---- interrupts (polled; no IRQ line on the Grove connector) ---------
    uint32_t readInterrupts();
    void clearInterrupts();
    uint32_t waitInterrupt(uint32_t mask, uint32_t timeoutMs);

    // ---- RF --------------------------------------------------------------
    void setField(bool on);
    void setNumTxBytes(uint16_t bytes, uint8_t trailingBits = 0);

    struct RxResult {
        bool ok = false;
        size_t len = 0;
        uint32_t irqs = 0;
        const char* error = nullptr;
    };

    RxResult transceive(const uint8_t* tx, size_t txLen, uint8_t* rx, size_t rxMax,
                        bool txCrc, bool rxCrc, uint32_t timeoutMs = 20);
    RxResult shortFrame(uint8_t cmd, uint8_t* rx, size_t rxMax, uint32_t timeoutMs = 20);

    // ---- ISO14443-3 / -4 --------------------------------------------------
    bool cascadeLevel(uint8_t sel, uint8_t outUid[4], uint8_t& outSak);
    bool anticollision(std::vector<uint8_t>& uid, uint8_t& sak);
    bool rats(uint8_t fsdi);

    const std::array<uint8_t, 18>& m_ecpData;
    std::array<uint8_t, 4> m_gpioPins;
    uint8_t m_i2cAddr;
    uint32_t m_i2cHz;

    i2c_master_bus_handle_t m_bus = nullptr;
    i2c_master_dev_handle_t m_dev = nullptr;

    bool m_connected = false;
    bool m_fieldUp = false;
    uint8_t m_icType = 0;
    uint8_t m_icRev = 0;

    // ISO-DEP session state, valid between a successful pollForTag() and the
    // next releaseTag().
    bool m_isodepActive = false;
    uint8_t m_blockNum = 0;
    // Rate-limits the informational block-number mismatch notice to once per
    // ISO-DEP session; the presence check runs every ~65 ms.
    bool m_blockMismatchLogged = false;
    uint16_t m_fsc = 32;      // max frame the card accepts, from ATS
    uint32_t m_fwtMs = 5;     // frame waiting time, from ATS

    static constexpr const char* TAG = "St25r3916Reader";
};
