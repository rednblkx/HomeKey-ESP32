#pragma once

#include "NfcReader.hpp"
#include "pn532_cxx/pn532.hpp"
#include "pn532_hal/spi.hpp"
#include "soc/gpio_num.h"

#include <array>
#include <cstdint>
#include <vector>

/**
 * @brief PN532 SPI implementation of the INfcReader interface.
 *
 * Wraps pn532::SpiTransport and pn532::Frontend, preserving the existing
 * command/response polling model.
 */
class Pn532Reader : public INfcReader {
public:
    Pn532Reader(const std::array<uint8_t, 4>& gpioPins, const std::array<uint8_t, 18>& ecpData);
    ~Pn532Reader() override;
		Pn532Reader(const Pn532Reader&) = delete;
		Pn532Reader& operator=(const Pn532Reader&) = delete;
		Pn532Reader(Pn532Reader&&) = delete;
		Pn532Reader& operator=(Pn532Reader&&) = delete;


    bool init() override;
    void stop() override;
    bool isConnected() const override;

    uint8_t getFwMajor() const override { return m_fwMajor; }
    uint8_t getFwMinor() const override { return m_fwMinor; }

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
    bool updateECP() override { return true;};
private:
    const std::array<uint8_t, 18> &m_ecpData;
    std::array<uint8_t, 4> m_gpioPins;
    pn532::SpiTransport* m_transport = nullptr;
    pn532::Frontend* m_frontend = nullptr;

    bool m_connected = false;
    uint8_t m_fwMajor = 0;
    uint8_t m_fwMinor = 0;

    std::vector<uint8_t> m_pollRes;
    std::vector<uint8_t> m_pollUid;

    static constexpr const char* TAG = "Pn532Reader";
};
