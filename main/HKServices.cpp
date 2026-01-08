#include "config.hpp"
#include "eventStructs.hpp"
#include "HomeKitLock.hpp"
#include "LockManager.hpp"
#include "ReaderDataManager.hpp"
#include "esp_mac.h"
#include "HK_HomeKit.h"

static EventBus::Bus& event_bus = EventBus::Bus::instance();

const std::array<std::array<uint8_t, 6>, 4> hk_color_vals = { {{0x01,0x04,0xce,0xd5,0xda,0x00}, {0x01,0x04,0xaa,0xd6,0xec,0x00}, {0x01,0x04,0xe3,0xe3,0xe3,0x00}, {0x01,0x04,0x00,0x00,0x00,0x00}} };

CUSTOM_CHAR(ConfigurationState, 263, PR+EV, UINT16, 0, 0, 1, true)
CUSTOM_CHAR(HardwareFinish, 26C, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessControlPoint, 264, PR+PW+WR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessSupportedConfiguration, 265, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(LockControlPoint, 19, PW, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)

/**
 * @brief Constructs a LockManagement service and registers required characteristics.
 *
 * Initializes the service with ID "44" and name "LockManagement" (active) and adds
 * the LockControlPoint and Version characteristics to the service's required list.
 */
Service::LockManagement::LockManagement() : SpanService{ "44","LockManagement",true } {
  req.push_back(&_CUSTOM_LockControlPoint);
  req.push_back(&hapChars.Version);
}
/**
 * @brief Creates the NFCAccess service and registers its required characteristics.
 *
 * Registers ConfigurationState, NFCAccessControlPoint, and NFCAccessSupportedConfiguration as required characteristics for the service.
 */
Service::NFCAccess::NFCAccess() : SpanService{ "266","NFCAccess",true } {
  req.push_back(&_CUSTOM_ConfigurationState);
  req.push_back(&_CUSTOM_NFCAccessControlPoint);
  req.push_back(&_CUSTOM_NFCAccessSupportedConfiguration);
}

/**
 * @brief Configure the NFC accessory information accessory and its characteristics.
 *
 * Initializes accessory information characteristics for an NFC HomeKit accessory:
 * sets identify, manufacturer ("rednblkx"), model ("HomeKey-ESP32"), name (from config.deviceName),
 * firmware revision (trimmed app version), and serial number (derived from the device BT MAC as "HK-<MAC>");
 * selects a HardwareFinish TLV from hk_color_vals using config.hk_key_color (defaults to index 0 on out-of-range)
 * and applies it to the HardwareFinish characteristic.
 *
 * @param config Configuration values; only config.deviceName and config.hk_key_color are used:
 *               - deviceName: accessory Name characteristic value.
 *               - hk_key_color: index into hk_color_vals to choose the HardwareFinish TLV.
 */
HomeKitLock::NFCAIS::NFCAIS(const espConfig::misc_config_t& config) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring NFCAccessoryInformation");
    opt.push_back(&_CUSTOM_HardwareFinish);
    new Characteristic::Identify();
    new Characteristic::Manufacturer("rednblkx");
    new Characteristic::Model("HomeKey-ESP32");
    new Characteristic::Name(config.deviceName.c_str());
    std::string app_desc = esp_app_get_description()->version;
    app_desc.erase(0, 1);
    new Characteristic::FirmwareRevision(app_desc.c_str());
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    const std::string macStr = fmt::format("HK-{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
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

/**
 * @brief Configure the LockManagement service for the accessory.
 *
 * Creates and registers the Lock Control Point and Version characteristics required
 * by the Lock Management service.
 */
HomeKitLock::LockManagementService::LockManagementService() {
    ESP_LOGI(HomeKitLock::TAG, "Configuring LockManagement");
    new Characteristic::LockControlPoint();
    new Characteristic::Version();
};

/**
 * @brief Configure lock mechanism integration between HomeKit and the lock manager.
 *
 * Initializes the LockCurrentState and LockTargetState characteristics on the provided HomeKit bridge
 * and publishes an initial override-state event that contains the current and target lock states
 * with the source set to HOMEKIT.
 *
 * @param bridge HomeKit bridge whose characteristic pointers will be set to the created characteristics.
 * @param lockManager Lock manager that supplies the current and target lock state values and the HOMEKIT source identifier.
 */
HomeKitLock::LockMechanismService::LockMechanismService(HomeKitLock& bridge, LockManager& lockManager) : m_lockManager(lockManager) {
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
    event_bus.publish({event_bus.get_topic(LOCK_O_STATE_CHANGED).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
}
/**
 * @brief Publishes an internal EventLockState when the lock's target state changes.
 *
 * If the LockTargetState characteristic was updated, emits an EventLockState containing the current state, the new target state, and source set to HOMEKIT on the internal event bus.
 *
 * @return boolean `true` on completion.
 */
boolean HomeKitLock::LockMechanismService::update() {
    if (m_lockTargetState->updated()) {
      EventLockState s{
        .currentState = static_cast<uint8_t>(m_lockCurrentState->getNewVal()),
        .targetState = static_cast<uint8_t>(m_lockTargetState->getNewVal()),
        .source = LockManager::HOMEKIT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      event_bus.publish({event_bus.get_topic(LOCK_T_STATE_CHANGED).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
    }
    return true;
}

/**
 * @brief Configure the NFCAccess service by creating and registering its required characteristics.
 *
 * Initializes the ConfigurationState characteristic, creates the NFCAccessSupportedConfiguration
 * characteristic populated with default TLV entries (0x01 -> 0x10, 0x02 -> 0x10), and creates the
 * NFCAccessControlPoint characteristic storing it in m_nfcControlPoint.
 *
 * @param readerDataManager Reference to the ReaderDataManager used to manage NFC reader data. 
 */
HomeKitLock::NFCAccessService::NFCAccessService(ReaderDataManager& readerDataManager) : m_readerDataManager(readerDataManager) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring NFCAccess");
    new Characteristic::ConfigurationState();
    TLV8 conf(nullptr, 0);
    conf.add(0x01, 0x10); conf.add(0x02, 0x10);
    new Characteristic::NFCAccessSupportedConfiguration(conf);
    m_nfcControlPoint = new Characteristic::NFCAccessControlPoint();
}
/**
 * @brief Processes incoming TLV data from the NFC Access Control Point, applies any response TLV to the control point, and publishes an internal ACCESSDATA_CHANGED event on the HomeKit bus.
 *
 * If the control point contains new TLV data, this invokes the HomeKit NFC processing context (providing save/remove callbacks and a reader-data snapshot), writes the resulting TLV response back to the control point, and emits an internal access-data-changed event on HK_BUS_TOPIC.
 *
 * @return `true` if the update cycle completed, `false` otherwise.
 */
boolean HomeKitLock::NFCAccessService::update() {
    if (!m_nfcControlPoint->updated()) return true;
    TLV8 ctrlData(nullptr, 0);
    m_nfcControlPoint->getNewTLV(ctrlData);
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());
    if (tlvData.empty()) return true;
    auto saveCallback = [this](const readerData_t& data) { return m_readerDataManager.updateReaderData(data); };
    auto remove_key_cb = [this]() { return m_readerDataManager.eraseReaderKey(); };
    readerData_t readerDataCopy = m_readerDataManager.getReaderDataCopy();
    HK_HomeKit hkCtx(readerDataCopy, saveCallback, remove_key_cb, tlvData);
    std::vector<uint8_t> result = hkCtx.processResult();
    TLV8 res(nullptr, 0);
    res.unpack(result.data(), result.size());
    m_nfcControlPoint->setTLV(res, false);
    HomekitEvent event{.type=ACCESSDATA_CHANGED, .data={}};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    event_bus.publish({event_bus.get_topic(HK_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
    return true;
}

/**
 * @brief Configure battery-related HomeKit characteristics for the bridge.
 *
 * Initializes the bridge's StatusLowBattery and BatteryLevel characteristics and assigns them
 * to the HomeKitLock instance.
 *
 * @param bridge The HomeKitLock instance to configure; its `m_statusLowBattery` is set to 0
 *               and its `m_batteryLevel` is set to 100. 
 */
HomeKitLock::PhysicalLockBatteryService::PhysicalLockBatteryService(HomeKitLock& bridge) {
    ESP_LOGI(HomeKitLock::TAG, "Configuring PhysicalLockBattery");
    bridge.m_statusLowBattery = new Characteristic::StatusLowBattery(0, true);
    bridge.m_batteryLevel = new Characteristic::BatteryLevel(100, true);
}