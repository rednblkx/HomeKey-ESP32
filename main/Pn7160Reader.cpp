#include "Pn7160Reader.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32-hal.h"
#include "esp_log_buffer.h"
#include "esp_log_level.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "nci/constants.hpp"
#include "pn7160.hpp"
#include "portmacro.h"
#include <vector>

Pn7160Reader::Pn7160Reader(const std::array<uint8_t, 4>& gpioPins,
                           uint8_t irqPin,
                           uint8_t venPin,
                          const std::array<uint8_t, 18>& ecpData)
    : m_ecpData(ecpData),
      m_gpioPins(gpioPins), 
      m_irqPin(irqPin),
      m_venPin(venPin)
      {}

Pn7160Reader::~Pn7160Reader() {
    stop();
}

bool Pn7160Reader::init() {
    if (m_transport || m_nci) {
        stop();
    }

    PN7160_SPI_Config spiCfg = {
        .miso = static_cast<gpio_num_t>(m_gpioPins[2]),
        .mosi = static_cast<gpio_num_t>(m_gpioPins[3]),
        .sclk = static_cast<gpio_num_t>(m_gpioPins[1]),
        .cs = static_cast<gpio_num_t>(m_gpioPins[0]),
        .irq = static_cast<gpio_num_t>(m_irqPin),
        .ven = static_cast<gpio_num_t>(m_venPin),
    };

    m_transport = std::make_unique<PN7160_SPI>(SPI2_HOST, spiCfg, 4);
    m_nci = std::make_unique<PN7160_NCI>(*m_transport);

    esp_err_t ret = m_nci->initialize();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PN7160 initialization failed: 0x%X", ret);
        stop();
        return false;
    }
    uint8_t frameSize = m_ecpData.size() - 2;
    std::vector<uint8_t> CFG = { 0x01, 0xA0, 0x6C, 0x1E, frameSize };
    CFG.insert(CFG.end(), m_ecpData.begin(), m_ecpData.end() - 2);
    CFG.resize(34, 0x00);

    if (const esp_err_t err = m_nci->core_set_config(CFG); err != nci::STATUS_OK) {
        ESP_LOGE(TAG, "Failed to set initial ECP config (NCI Status=0x%02X)", err);
        stop();
        return false;
    }
    {
        NciMessage cfg;
        const uint8_t kPmuQuery[] = {0x1, 0xA0, 0x0E};
        ret = m_nci->core_get_config(kPmuQuery, cfg);
        if (ret != nci::STATUS_OK) {
            ESP_LOGW(TAG, "core_get_config(PMU) failed (NCI Status=0x%02X)", ret);
        }
        if (ret == nci::STATUS_OK && cfg.size() > 15 && cfg.get_payload_ptr()[12] != 0xff) {
            // UM11495 Section 13.1 - PMU_CFG (Tag 0xA00E)
            static const std::vector<uint8_t> PMU_CFG = {
                0x01,        // Number of parameters
                0xA0, 0x0E,  // ext. tag
                0x0b,        // length
                0x11,        // IRQ Enable: PVDD + temp sensor IRQs
                0x01,        // RFU
                0x01,        // Power and Clock Configuration, device on (CFG1)
                0x01,        // Power and Clock Configuration, device off (CFG1)
                0x00,        // RFU
                0x00,        // DC-DC 0
                0x00,        // DC-DC 1
                0xFF,        // TXLDO (5.0V / 5.0V)
                0x00,        // RFU
                0x90,        // TXLDO check
                0x0C,        // RFU
            };

            ret = m_nci->core_set_config(PMU_CFG);

            if (ret != nci::STATUS_OK) {
                ESP_LOGE(TAG, "Failed to set PMU config (NCI Status=0x%02X)", ret);
                stop();
                return false;
            }
            ESP_LOGI(TAG, "PMU Config set successfully");
        }
    }

    // NCI Core Spec v2.0 Section 6.1 - TOTAL_DURATION (Tag 0x00)
    static const std::vector<uint8_t> CORE_CONFIG_TOTAL_DURATION_SOLO = {
        0x01,  // Number of parameter fields
        0x00,  // config param identifier (TOTAL_DURATION)
        0x02,  // length of value
        0x32,  // TOTAL_DURATION (low)
        0x00   // TOTAL_DURATION (high)
    };

    ret = m_nci->core_set_config(CORE_CONFIG_TOTAL_DURATION_SOLO);

    if (ret == ESP_FAIL) return false;
    if (ret != nci::STATUS_OK) {
        ESP_LOGE(TAG, "Failed to set TOTAL_DURATION config (NCI Status=0x%02X)", ret);
    } else {
        ESP_LOGI(TAG, "TOTAL_DURATION Config set successfully");
    }

    m_connected = true;
    m_lastPresenceCheck = 0;
    m_lastHealthCheckTick = xTaskGetTickCount();
    const uint16_t fw_ver = m_nci->get_firmware_version();
    m_fwMajor = fw_ver >> 8;
    m_fwMinor = fw_ver & 0x0f;
    m_currentProtocol = 0;

    ret = m_nci->start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start PN7160 task runner.");
        stop();
        return false;
    }

    ESP_LOGI(TAG, "PN7160 reader initialized.");
    return true;
}

void Pn7160Reader::stop() {
    if (m_nci) {
        m_nci->stop();
    }
    m_nci.reset();
    if (m_transport) {
        m_transport->deinit();
    }
    m_transport.reset();
    m_connected = false;
    m_lastPresenceCheck = 0;
    m_lastHealthCheckTick = 0;
    m_fwMajor = 0;
    m_fwMinor = 0;
    m_currentProtocol = 0;
}

bool Pn7160Reader::isConnected() const {
    return m_connected;
}

bool Pn7160Reader::beginDiscovery() {
    if (!m_nci) return false;

    // Default discovery config: NFC-A passive poll only.
    std::vector<uint8_t> discoveryConfig = {
        1,                         // 1 configuration entry
        nci::MODE_PASSIVE_POLL_A,  // NFC-A passive poll
        1,                         // every discovery period
    };

    esp_err_t ret = m_nci->rf_start_discovery(discoveryConfig);
    if (ret != nci::STATUS_OK) {
        ESP_LOGE(TAG, "rf_start_discovery failed: 0x%X", ret);
        return false;
    }
    ESP_LOGI(TAG, "RF discovery started.");
    return true;
}

bool Pn7160Reader::pollForTag(std::vector<uint8_t>& uid,
                              std::array<uint8_t, 2>& atqa,
                              uint8_t& sak,
                              uint32_t timeoutMs) {
    if (!m_nci) return false;
    NciEvent event;
    esp_err_t ret = m_nci->get_event(event, timeoutMs);
    if (ret == ESP_ERR_TIMEOUT) {
        return false;
    }
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "get_event failed: 0x%X", ret);
        return false;
    }
    switch (event.type) {
        case NciEventType::RF_INTF_ACTIVATED: {
            const NciMessage& msg = event.msg;
            const uint8_t* p = msg.get_payload_ptr();
            uint8_t plen = msg.get_len();

            // NCI 2.0 Table 78 — RF_INTF_ACTIVATED_NTF payload:
            // [0] RF Discovery ID
            // [1] RF Interface
            // [2] RF Protocol
            // [3] Activation RF Technology and Mode
            // [4] Max Data Packet Payload Size
            // [5] Initial Number of Credits
            // [6] Length of RF Technology Specific Parameters (tlen)
            // [7..7+tlen-1] RF Technology Specific Parameters
            //
            // Table 68 — NFC-A Poll Mode Specific Parameters:
            //   SENS_RES Response        (2 bytes)
            //   NFCID1 Length              (1 byte)
            //   NFCID1                     (variable)
            //   SEL_RES Response Length    (1 byte)
            //   SEL_RES Response           (0 or 1 byte)
            //   HRx Length                 (1 byte)
            //   HRx                        (0 or 2 bytes)
            if (plen < 7) {
                ESP_LOGW(TAG, "RF_INTF_ACTIVATED payload too short (%u).", plen);
                ESP_LOG_BUFFER_HEXDUMP(TAG, p, plen, ESP_LOG_WARN);
                return false;
            }
            if(p[1] != nci::INTF_ISODEP){
                if((xTaskGetTickCount() - m_lastActivation) < pdMS_TO_TICKS(kActivationCooldownWindowMs)){
                    ESP_LOGD(TAG, "Tag activated within cooldown window, ignoring!");
                    m_lastActivation = xTaskGetTickCount();
                    releaseTag();
                    return false;
                }
                m_lastActivation = xTaskGetTickCount();
            }

            uint8_t tlen = p[6];
            if (plen < 7 + tlen) {
                ESP_LOGW(TAG, "RF_INTF_ACTIVATED tlen=%u exceeds payload (%u).", tlen, plen);
                ESP_LOG_BUFFER_HEXDUMP(TAG, p, plen, ESP_LOG_WARN);
                return false;
            }

            const uint8_t* t = p + 7;  // start of Technology Specific Parameters
            uint8_t tRem = tlen;
            size_t tOff = 0;

            auto need = [&](size_t n) -> bool {
                if (tOff + n > tRem) {
                    ESP_LOGW(TAG, "RF_INTF_ACTIVATED NFC-A params truncated at tOff=%zu, need=%zu, have=%u.",
                              tOff, n, tRem);
                    return false;
                }
                return true;
            };

            // SENS_RES (ATQA) — 2 bytes
            if (!need(2)) return false;
            atqa[0] = t[tOff + 0];
            atqa[1] = t[tOff + 1];
            tOff += 2;

            // NFCID1 Length
            if (!need(1)) return false;
            uint8_t nfcidLen = t[tOff++];

            // NFCID1
            if (!need(nfcidLen)) return false;
            uid.clear();
            for (uint8_t i = 0; i < nfcidLen; ++i) {
                uid.push_back(t[tOff + i]);
            }
            tOff += nfcidLen;

            // SEL_RES Response Length
            if (!need(1)) return false;
            uint8_t selResLen = t[tOff++];

            // SEL_RES Response (SAK)
            sak = 0;
            if (selResLen > 0) {
                if (!need(selResLen)) return false;
                sak = t[tOff];
                tOff += selResLen;
            }

            // HRx Length (consume, ignore)
            if (need(1)) {
                uint8_t hrLen = t[tOff++];
                if (hrLen > 0 && need(hrLen)) {
                    tOff += hrLen;
                }
            }

            uint8_t protocol = p[2];
            m_currentProtocol = protocol;

            ESP_LOGI(TAG, "Tag detected! UID len=%u, ATQA=%02X%02X, SAK=%02X, Proto=0x%02X",
                      nfcidLen, atqa[0], atqa[1], sak, protocol);
            return true;
        }

        case NciEventType::RF_DISCOVER: {
            if (event.msg.size() < 7) {
                ESP_LOGW(TAG, "RF_DISCOVER payload too short (%zu)", event.msg.size());
            }
            break;
        }

        case NciEventType::RF_DEACTIVATE: {
            if (event.msg.size() >= 4) {
                uint8_t deact_type = event.msg[3];
                if (deact_type != nci::DEACTIVATION_TYPE_DISCOVERY) {
                    ESP_LOGW(TAG, "RF_DEACTIVATE to type 0x%02X, restarting discovery.", deact_type);
                    beginDiscovery();
                }
            }
            break;
        }

        default:
            break;
    }
    return false;
}

bool Pn7160Reader::isTagStillPresent() {
    if (!m_nci) return false;

    if (m_currentProtocol == nci::PROT_ISODEP) {
        TickType_t now = xTaskGetTickCount();
        if ((now - m_lastPresenceCheck) * portTICK_PERIOD_MS >= kPresenceCheckIntervalMs) {
            (void)m_nci->rf_iso_dep_presence_check();
            m_lastPresenceCheck = now;
        }
        return m_nci->tag_in_field();
    }
    return false;
}

void Pn7160Reader::releaseTag() {
    if (!m_nci) return;

    esp_err_t ret = m_nci->rf_deactivate(nci::DEACTIVATION_TYPE_DISCOVERY);
    if (ret != nci::STATUS_OK) {
        ESP_LOGW(TAG, "rf_deactivate failed (0x%X), restarting discovery.", ret);
        beginDiscovery();
    }
}

void Pn7160Reader::endDiscovery() {
  if (m_nci) {
      (void)m_nci->rf_stop_discovery();
  }
}

bool Pn7160Reader::exchangeApdu(const std::vector<uint8_t>& send,
                                std::vector<uint8_t>& recv,
                                uint32_t timeoutMs) {
    if (!m_nci || send.empty()) return false;
    if (m_currentProtocol != nci::PROT_ISODEP) {
        return false;
    }
    recv.clear();
    if (const auto rsp = m_nci->send_apdu_sync(send, timeoutMs)) {
        recv = rsp.value();
        return true;
    }
    return false;
}

bool Pn7160Reader::healthCheck() {
    if (!m_nci) {
        m_connected = false;
        return false;
    }
    if (!m_nci->is_initialized()) {
        m_connected = false;
        m_fwMajor = 0;
        m_fwMinor = 0;
        return false;
    }

    TickType_t now = xTaskGetTickCount();
    if ((now - m_lastHealthCheckTick) * portTICK_PERIOD_MS >= kHealthCheckIntervalMs) {
        const uint8_t kHealthPayload[] = {0x01, 0x0};
        NciMessage cmd(nci::PKT_MT_CTRL_COMMAND, nci::CORE_GID, nci::CORE_GET_CONFIG_OID, kHealthPayload);
        NciMessage rsp;
        esp_err_t ret = m_nci->send_command_wait_response(cmd, rsp, nci::PN7160_DEFAULT_TIMEOUT_MS);
        if (ret != nci::STATUS_OK) {
            ESP_LOGE(TAG, "Health check failed: NCI Status=0x%02X", ret);
            m_connected = false;
            m_fwMajor = 0;
            m_fwMinor = 0;
            return false;
        }
        ESP_LOGD(TAG, "Health check OK");
        m_lastHealthCheckTick = now;
    }
    return true;
}

 bool Pn7160Reader::updateECP() {
    if (!m_nci || !m_connected) return false;

    if(auto ret = m_nci->rf_stop_discovery(); ret != nci::STATUS_OK){
        ESP_LOGE(TAG, "Failed to stop discovery (NCI Status=0x%02X)", ret);
        return false;
    }

    uint8_t frameSize = m_ecpData.size() - 2; // 16 bytes (Header + GID)
    std::vector<uint8_t> CFG = {
        0x01,        // Number of parameter fields
        0xA0, 0x6C,  // Tag: ECP Configuration
        0x1E,        // Length
        frameSize    // Frame size
    };
    CFG.insert(CFG.end(), m_ecpData.begin(), m_ecpData.end() - 2);
    CFG.resize(34, 0x00); // Pad to 34 bytes (5 header bytes + 29 data bytes)

    esp_err_t ret = m_nci->core_set_config(CFG);
    if (ret != nci::STATUS_OK) {
        ESP_LOGE(TAG, "Failed to update PN7160 ECP config (NCI Status=0x%02X)", ret);
        beginDiscovery();
        return false;
    }

    ESP_LOGI(TAG, "ECP Frame updated.");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, CFG.data(), CFG.size(), ESP_LOG_DEBUG);

    return beginDiscovery();
}
