#include "config.hpp"
#include "eventStructs.hpp"
#include "HomeKitLock.hpp"
#include "LockManager.hpp"
#include "ReaderDataManager.hpp"
#include "esp_mac.h"
#include "HK_HomeKit.h"

const std::array<std::array<uint8_t, 6>, 4> hk_color_vals = { {{0x01,0x04,0xce,0xd5,0xda,0x00}, {0x01,0x04,0xaa,0xd6,0xec,0x00}, {0x01,0x04,0xe3,0xe3,0xe3,0x00}, {0x01,0x04,0x00,0x00,0x00,0x00}} };

CUSTOM_CHAR(ConfigurationState, 263, PR+EV, UINT16, 0, 0, 1, true)
CUSTOM_CHAR(HardwareFinish, 26C, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessControlPoint, 264, PR+PW+WR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessSupportedConfiguration, 265, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(LockControlPoint, 19, PW, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)

Service::LockManagement::LockManagement() : SpanService{ "44","LockManagement",true } {
  req.push_back(&_CUSTOM_LockControlPoint);
  req.push_back(&hapChars.Version);
}
Service::NFCAccess::NFCAccess() : SpanService{ "266","NFCAccess",true } {
  req.push_back(&_CUSTOM_ConfigurationState);
  req.push_back(&_CUSTOM_NFCAccessControlPoint);
  req.push_back(&_CUSTOM_NFCAccessSupportedConfiguration);
}

HomeKitLock::NFCAIS::NFCAIS(const espConfig::misc_config_t& config) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring NFCAccessoryInformation");
    opt.push_back(&_CUSTOM_HardwareFinish);
    new Characteristic::Identify();
    new Characteristic::Manufacturer("rednblkx");
    new Characteristic::Model("HomeKey-ESP32");
    new Characteristic::Name(config.deviceName.c_str());
    const esp_app_desc_t* app_desc = esp_app_get_description();
    new Characteristic::FirmwareRevision(app_desc->version);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    std::string macStr = fmt::format("HK-{:02X}{:02X}{:02X}{:02X}", mac[0], mac[1], mac[2], mac[3]);
    new Characteristic::SerialNumber(macStr.c_str());

    size_t color_index = config.hk_key_color;
    if (color_index >= hk_color_vals.size()) {
        ESP_LOGW(HomeKitLock::TAG, "Invalid hk_key_color index %d. Defaulting to 0.", color_index);
        color_index = 0;
    }
    std::array<uint8_t, 6> color_buf = hk_color_vals[color_index];
    TLV8 hwfinish(nullptr, 0);
    hwfinish.unpack(color_buf.data(), color_buf.size());
    new Characteristic::HardwareFinish(hwfinish);
}

HomeKitLock::LockManagementService::LockManagementService() {
    ESP_LOGI(HomeKitLock::TAG, "Configuring LockManagement");
    new Characteristic::LockControlPoint();
    new Characteristic::Version();
};

HomeKitLock::LockMechanismService::LockMechanismService(HomeKitLock& bridge, LockManager& lockManager) : m_lockManager(lockManager) {
    espp::EventManager::get().add_publisher("lock/overrideState", "LockMechanismService");
    espp::EventManager::get().add_publisher("lock/targetStateChanged", "LockMechanismService");
    ESP_LOGI(HomeKitLock::TAG, "Configuring LockMechanism");
    m_lockCurrentState = bridge.m_lockCurrentState = new Characteristic::LockCurrentState(m_lockManager.getCurrentState(), true);
    m_lockTargetState = bridge.m_lockTargetState = new Characteristic::LockTargetState(m_lockManager.getTargetState(), true);
    EventLockState s{
      .currentState = static_cast<uint8_t>(m_lockCurrentState->getNewVal()),
      .targetState = static_cast<uint8_t>(m_lockTargetState->getNewVal()),
      .source = LockManager::HOMEKIT
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    espp::EventManager::get().publish("lock/overrideState", d);
}
boolean HomeKitLock::LockMechanismService::update() {
    if (m_lockTargetState->updated()) {
      EventLockState s{
        .currentState = static_cast<uint8_t>(m_lockCurrentState->getNewVal()),
        .targetState = static_cast<uint8_t>(m_lockTargetState->getNewVal()),
        .source = LockManager::HOMEKIT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/targetStateChanged", d);
    }
    return true;
}

HomeKitLock::NFCAccessService::NFCAccessService(ReaderDataManager& readerDataManager) : m_readerDataManager(readerDataManager) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring NFCAccess");
    new Characteristic::ConfigurationState();
    TLV8 conf(nullptr, 0);
    conf.add(0x01, 0x10); conf.add(0x02, 0x10);
    new Characteristic::NFCAccessSupportedConfiguration(conf);
    m_nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    espp::EventManager::get().add_publisher("nfc/updateECP", "NFCAccessService");
}
boolean HomeKitLock::NFCAccessService::update() {
    if (!m_nfcControlPoint->updated()) return true;
    TLV8 ctrlData(nullptr, 0);
    m_nfcControlPoint->getNewTLV(ctrlData);
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());
    if (tlvData.empty()) return true;
    auto saveCallback = [this](const readerData_t& data) { return m_readerDataManager.updateReaderData(data); };
    auto remove_key_cb = [this]() { return m_readerDataManager.eraseReaderKey(); };
    readerData_t readerDataCopy = m_readerDataManager.getReaderData();
    HK_HomeKit hkCtx(readerDataCopy, saveCallback, remove_key_cb, tlvData);
    std::vector<uint8_t> result = hkCtx.processResult();
    TLV8 res(nullptr, 0);
    res.unpack(result.data(), result.size());
    m_nfcControlPoint->setTLV(res, false);
    espp::EventManager::get().publish("nfc/updateECP", {});
    return true;
}

HomeKitLock::PhysicalLockBatteryService::PhysicalLockBatteryService(HomeKitLock& bridge) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring PhysicalLockBattery");
    bridge.m_statusLowBattery = new Characteristic::StatusLowBattery(0, true);
    bridge.m_batteryLevel = new Characteristic::BatteryLevel(100, true);
}
