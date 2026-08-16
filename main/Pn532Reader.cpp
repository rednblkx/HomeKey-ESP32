#include "Pn532Reader.hpp"
#include "esp_log.h"
#include "pn532_cxx/transaction.hpp"
#include <array>

Pn532Reader::Pn532Reader(const std::array<uint8_t, 4>& gpioPins, const std::array<uint8_t, 18>& ecpData)
    : m_ecpData(ecpData),
      m_gpioPins(gpioPins) {}

Pn532Reader::~Pn532Reader() {
    stop();
}

bool Pn532Reader::init() {
    if (!m_transport) {
        m_transport = new pn532::SpiTransport(
            GPIO_NUM_NC,
            static_cast<gpio_num_t>(m_gpioPins[2]),  // MISO
            static_cast<gpio_num_t>(m_gpioPins[3]),  // MOSI
            static_cast<gpio_num_t>(m_gpioPins[1]),  // SCK
            static_cast<gpio_num_t>(m_gpioPins[0])   // SS
        );
    }
    if (!m_frontend) {
        m_frontend = new pn532::Frontend(*m_transport);
    }

    if (auto status = m_frontend->begin(); status != pn532::Status::SUCCESS){
        ESP_LOGE(TAG, "Error establishing PN532 connection. (err=%d)", status);
    }
    if (auto versiondata = m_frontend->GetFirmwareVersion()) {
      ESP_LOGI(TAG, "Found chip PN532, Firmware ver. %d.%d",
              (versiondata.value() >> 24) & 0xFF, (versiondata.value() >> 16) & 0xFF);
      m_connected = true;
      m_fwMajor = static_cast<uint8_t>((versiondata.value() >> 24) & 0xFF);
      m_fwMinor = static_cast<uint8_t>((versiondata.value() >> 16) & 0xFF);
    } else {
        ESP_LOGE(TAG, "Error establishing PN532 connection.");
        stop();
        return false;
    }

    if (m_frontend->RFConfiguration(0x01, {0x03}) != pn532::Status::SUCCESS) {
        return false;
    }
    if (m_frontend->setPassiveActivationRetries(0) != pn532::Status::SUCCESS) {
        return false;
    }
    if (m_frontend->RFConfiguration(0x02, {0x00, 0x0B, 0x10}) != pn532::Status::SUCCESS) {
        return false;
    }
    if (m_frontend->RFConfiguration(0x04, {0xFF}) != pn532::Status::SUCCESS) {
        return false;
    }

    ESP_LOGI(TAG, "Reader initialized. Waiting for tags...");
    return true;
}

void Pn532Reader::stop() {
    if (m_frontend) {
        delete m_frontend;
        m_frontend = nullptr;
    }
    if (m_transport) {
        delete m_transport;
        m_transport = nullptr;
    }
    m_connected = false;
    m_fwMajor = 0;
    m_fwMinor = 0;
}

bool Pn532Reader::isConnected() const {
    return m_connected;
}

bool Pn532Reader::beginDiscovery() {
    // PN532 discovery is implicitly started by pollForTag.
    // We just ensure passive activation retries are cleared.
    if (m_frontend) {
        (void)m_frontend->setPassiveActivationRetries(0);
    }
    return true;
}

bool Pn532Reader::pollForTag(std::vector<uint8_t>& uid,
                             std::array<uint8_t, 2>& atqa,
                             uint8_t& sak,
                             uint32_t timeoutMs) {
    if (!m_frontend) return false;
    uint8_t sel_res = 0;
    std::vector<uint8_t> res;
    (void)m_frontend->InCommunicateThru(m_ecpData, res, 50);
    const pn532::Status status = m_frontend->InListPassiveTarget(
        0x0, uid, atqa, sel_res, timeoutMs);
    sak = sel_res;
    return status == pn532::Status::SUCCESS;
}

bool Pn532Reader::isTagStillPresent() {
    if (!m_frontend) return false;
    releaseTag();
    std::vector<uint8_t> uid;
    std::array<uint8_t, 2> atqa;
    uint8_t sak;
    pn532::Status status = m_frontend->InListPassiveTarget(0x00, uid, atqa, sak);
    return status == pn532::Status::SUCCESS;
}

void Pn532Reader::releaseTag() {
    if (m_frontend) {
        (void)m_frontend->InRelease(1);
        (void)m_frontend->setPassiveActivationRetries(0);
    }
}

void Pn532Reader::endDiscovery() {
    // No explicit discovery stop required for PN532.
}

bool Pn532Reader::exchangeApdu(const std::vector<uint8_t>& send,
                               std::vector<uint8_t>& recv,
                               uint32_t timeoutMs) {
    if (!m_frontend || send.size() > 255) return false;
    recv.clear();
    pn532::Status status = m_frontend->InDataExchange(send, recv, timeoutMs);
    if (status != pn532::Status::SUCCESS) return false;
    // Strip PN532 status bytes (first 2 bytes of response)
    if (recv.size() >= 2) {
        recv.erase(recv.begin(), recv.begin() + 2);
    }
    return true;
}

bool Pn532Reader::healthCheck() {
    if (!m_frontend) {
        m_connected = false;
        return false;
    }
    pn532::Status status = m_frontend->WriteRegister({0x63, 0x3d, 0x0});
    if (status != pn532::Status::SUCCESS) {
        m_connected = false;
        m_fwMajor = 0;
        m_fwMinor = 0;
        return false;
    }
    return true;
}
