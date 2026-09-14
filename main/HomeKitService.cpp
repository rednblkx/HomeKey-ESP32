#include "HomeKitService.hpp"
#include "ConfigManager.hpp"
#include "GPIOAllocator.hpp"
#include "HapMigration.hpp"
#include "LockManager.hpp"
#include "ReaderDataManager.hpp"
#include "SharedLed.hpp"
#include "config.hpp"
#include "eventStructs.hpp"
#include "utils.hpp"
#include "HK_HomeKit.h"

#include "Esp32Crypto.hpp"
#include "Esp32Network.hpp"
#include "Esp32Platform.hpp"
#include "Esp32Storage.hpp"

#include "esp_app_desc.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "fmt/ranges.h"

#include <sodium/crypto_sign.h>

#include <atomic>
#include <cctype>

const char* HomeKitService::TAG = "HomeKit";
HomeKitService* HomeKitService::s_instance = nullptr;

namespace {

// Same TLV color values the old HKServices.cpp used for HardwareFinish.
const std::array<std::array<uint8_t, 6>, 4> hk_color_vals = {
    {{0x01, 0x04, 0xce, 0xd5, 0xda, 0x00},
     {0x01, 0x04, 0xaa, 0xd6, 0xec, 0x00},
     {0x01, 0x04, 0xe3, 0xe3, 0xe3, 0x00},
     {0x01, 0x04, 0x00, 0x00, 0x00, 0x00}}};

// Status LED blink patterns, mirroring HomeSpan's Blinker behavior:
// unpaired = fast blink, AP mode = slow blink, paired = steady off with a
// brief pulse on activity. Driven from the HAP task tick (100 ms period).

static SharedLed* s_statusLed = nullptr;

} // namespace

/**
 * @brief PAL bundle for the YAHAP server.
 *
 * Instances are held by unique_ptr and wired into AccessoryServer::Config as
 * raw pointers; declaration order guarantees they outlive the server.
 */
/// Format an 8-digit setup code as HAP expects: "XXX-XX-XXX".
static std::string hapSetupCode(const std::string& digits) {
  std::string clean;
  for (char c : digits) {
    if (isdigit(static_cast<unsigned char>(c))) clean.push_back(c);
  }
  if (clean.size() != 8) {
    ESP_LOGW("HomeKit", "Setup code '%s' is not 8 digits; passing through unformatted",
             digits.c_str());
    return digits;
  }
  return clean.substr(0, 3) + "-" + clean.substr(3, 2) + "-" + clean.substr(5, 3);
}

struct HomeKitService::Pal {
  Esp32System system;
  Esp32Storage storage;
  Esp32Crypto crypto;
  Esp32Network network;
};

HomeKitService::HomeKitService(LockManager& lockManager,
                               ConfigManager& configManager,
                               NvsCredentialStore& readerDataManager)
    : m_lockManager(lockManager),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager) {
  if (s_instance != nullptr) {
    ESP_LOGE(TAG, "More than one HomeKitService created!");
    esp_restart();
  }
  s_instance = this;

  m_lock_state_changed = AppEventLoop::subscribe(
      LOCK_EVENT, LOCK_STATE_CHANGED, [&](const uint8_t* data, size_t size) {
        if (size == 0 || data == nullptr) return;
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
        if (ec) {
          ESP_LOGE(TAG, "Failed to deserialize EventLockState: %s",
                   ec.message().c_str());
          return;
        }
        updateLockState(s.currentState, s.targetState);
      });

  m_hk_event = AppEventLoop::subscribe(
      HK_EVENT, HK_INTERNAL_EVENT, [&](const uint8_t* data, size_t size) {
        if (size == 0 || data == nullptr) return;
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        HomekitEvent hk_event = alpaca::deserialize<HomekitEvent>(payload, ec);
        if (ec) {
          ESP_LOGE(TAG, "Failed to deserialize HomekitEvent: %s",
                   ec.message().c_str());
          return;
        }
        switch (hk_event.type) {
        case HomekitEventType::SETUP_CODE_CHANGED: {
          EventValueChanged s =
              alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
          if (ec) return;
          setSetupCode(s.str);
          break;
        }
        case HomekitEventType::BTR_PROP_CHANGED: {
          EventValueChanged s =
              alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
          if (ec) return;
          if (s.name == "btrLevel") {
            uint8_t low = 0;
            if (m_statusLowBattery) {
              auto v = m_statusLowBattery->get_value();
              if (std::holds_alternative<hap::core::Value>(v)) {
                low = std::get<uint8_t>(std::get<hap::core::Value>(v));
              }
            }
            updateBatteryStatus(s.newValue, low);
          } else if (s.name == "btrLowThreshold") {
            uint8_t level = 100;
            if (m_batteryLevel) {
              auto v = m_batteryLevel->get_value();
              if (std::holds_alternative<hap::core::Value>(v)) {
                level = std::get<uint8_t>(std::get<hap::core::Value>(v));
              }
            }
            updateBatteryStatus(level, s.newValue);
          }
          break;
        }
        default:
          break;
        }
      });
}

HomeKitService::~HomeKitService() {
  if (m_hapTask) vTaskDelete(m_hapTask);
  if (m_workQueue) vQueueDelete(m_workQueue);
  s_instance = nullptr;
}

void HomeKitService::buildAccessoryTree(hap::core::Accessory& accessory) {
  const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
  const std::string app_version = esp_app_get_description()->version;

  namespace chr = hap::characteristic;
  namespace svc = hap::service;

  // --- Accessory Information ---
  {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    const std::string serial = fmt::format("HK-{:02X}{:02X}{:02X}{:02X}",
                                           mac[2], mac[3], mac[4], mac[5]);
    std::string version = app_version;
    if (!version.empty() && version[0] == 'v') version.erase(0, 1);

    size_t color_index = miscConfig.hk_key_color;
    if (color_index >= hk_color_vals.size()) {
      ESP_LOGW(TAG, "Invalid hk_key_color index %u. Defaulting to 0.", color_index);
      color_index = 0;
    }
    std::vector<uint8_t> hwfinish(hk_color_vals[color_index].begin(),
                                  hk_color_vals[color_index].end());

    accessory.add_service(
        svc::ServiceBuilder(svc::kType_AccessoryInformation, "Accessory Information")
            .add(chr::CharId::Identify)
            .on_write_bool([] { ESP_LOGI(TAG, "Identify!"); })
            .add(chr::CharId::Manufacturer, "rednblkx")
            .add(chr::CharId::Model, "HomeKey-ESP32")
            .add(chr::CharId::Name, miscConfig.deviceName.c_str())
            .add(chr::CharId::FirmwareRevision, version.c_str())
            .add(chr::CharId::SerialNumber, serial.c_str())
            .add(chr::CharId::HardwareFinish, hwfinish)
            .build());
  }

  // --- HAP Protocol Information ---
  accessory.add_service(
      svc::ServiceBuilder(svc::kType_HAPProtocolInformation, "Protocol Information")
          .add(chr::CharId::Version)
          .build());

  // --- Lock Management ---
  accessory.add_service(
      svc::ServiceBuilder(svc::kType_LockManagement, "Lock Management")
          .add(chr::CharId::LockControlPoint)
          .add(chr::CharId::Version)
          .build());

  // --- Lock Mechanism (primary) ---
  {
    auto builder = std::make_unique<svc::ServiceBuilder>(
        svc::kType_LockMechanism, "Lock Mechanism", true);
    m_lockCurrentState = builder->add(chr::CharId::LockCurrentStateChar,
                                      static_cast<uint8_t>(m_lockManager.getCurrentState())).get();
    m_lockTargetState = builder->add(chr::CharId::LockTargetStateChar,
                                     static_cast<uint8_t>(m_lockManager.getTargetState()))
                            .on_write([this](const hap::core::Value& v)
                                          -> hap::core::WriteResponse {
                              if (auto* target = std::get_if<uint8_t>(&v)) {
                                uint8_t current = 0;
                                if (m_lockCurrentState) {
                                  auto cv = m_lockCurrentState->get_value();
                                  if (std::holds_alternative<hap::core::Value>(cv)) {
                                    current = std::get<uint8_t>(std::get<hap::core::Value>(cv));
                                  }
                                }
                                EventLockState s{
                                    .currentState = current,
                                    .targetState = *target,
                                    .source = LockManager::HOMEKIT};
                                std::vector<uint8_t> d;
                                alpaca::serialize(s, d);
                                AppEventLoop::publish(LOCK_EVENT,
                                                      LOCK_TARGET_STATE_CHANGED,
                                                      d.data(), d.size());
                              }
                              return std::nullopt;
                            })
                            .get();
    accessory.add_service(builder->build());
  }

  // Announce the initial state (same LOCK_OVERRIDE_STATE event as before).
  {
    EventLockState s{.currentState = static_cast<uint8_t>(m_lockManager.getCurrentState()),
                     .targetState = static_cast<uint8_t>(m_lockManager.getTargetState()),
                     .source = LockManager::HOMEKIT};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    AppEventLoop::publish(LOCK_EVENT, LOCK_OVERRIDE_STATE, d.data(), d.size());
  }

  // --- NFC Access ---
  {
    auto builder = std::make_unique<svc::ServiceBuilder>(svc::kType_NFCAccess,
                                                         "NFC Access", true);
    builder->add(chr::CharId::ConfigurationState, static_cast<uint8_t>(0));
    // Supported configuration: {0x01: 0x10, 0x02: 0x10} TLV8, as before.
    std::vector<uint8_t> supported =
        hap::core::TLV8::encode({{0x01, static_cast<uint8_t>(0x10)},
                                 {0x02, static_cast<uint8_t>(0x10)}});
    builder->add(chr::CharId::NFCAccessSupportedConfiguration, supported);
    m_nfcControlPoint =
        builder->add(chr::CharId::NFCAccessControlPoint)
            .on_write_response_tlv([this](const std::vector<uint8_t>& tlv)
                                       -> std::optional<hap::core::HAPResponse<hap::core::Value>> {
              auto remove_key_cb = [this]() { m_readerDataManager.eraseReaderKey(); };
              std::vector<uint8_t> request = tlv;
              HK_HomeKit hkCtx(m_readerDataManager, remove_key_cb, request);
              std::vector<uint8_t> result = hkCtx.processResult();

              if (!result.empty()) {
                auto parsed = hap::core::TLV8::parse(result);
                if (auto it = hap::core::TLV8::find(parsed, 0x07);
                    it && it->size() == 3) {
                  HomekitEvent event{.type = HomekitEventType::ACCESSDATA_CHANGED,
                                     .data = {}};
                  std::vector<uint8_t> event_data;
                  alpaca::serialize(event, event_data);
                  AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT,
                                        event_data.data(), event_data.size());
                }
              }
              if (result.empty()) return std::nullopt;
              return hap::core::Value(result);
            })
            .get();
    accessory.add_service(builder->build());
  }

  // --- Battery Service (optional) ---
  if (miscConfig.proxBatEnabled) {
    auto builder = std::make_unique<svc::ServiceBuilder>(svc::kType_BatteryService,
                                                         "Battery Service");
    m_batteryLevel = builder->add(chr::CharId::BatteryLevel, static_cast<uint8_t>(100)).get();
    m_statusLowBattery = builder->add(chr::CharId::StatusLowBatteryChar,
                                      static_cast<uint8_t>(0)).get();
    accessory.add_service(builder->build());
  }
}

void HomeKitService::begin() {
  const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();

  // Optional status LED through the shared-pin arbiter.
  if (miscConfig.hsStatusPin != 255) {
    static auto lease = GPIOAllocator::instance().acquire(
        gpio_num_t(miscConfig.hsStatusPin), GPIO_MODE_OUTPUT,
        GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::HomeKit,
        "HK_STATUS_LED");
    if (lease.has_value()) {
      static SharedLed statusLed{std::move(lease.value())};
      s_statusLed = &statusLed;
    } else {
      ESP_LOGW(TAG, "Could not acquire status LED pin, error: %d",
               lease.error());
    }
  }

  // One-time NVS migration from HomeSpan, before the server reads hap_storage.
  auto stats = hap_migration::run();
  if (stats.migrated) {
    ESP_LOGI(TAG, "Migrated %d controller(s) from HomeSpan", stats.controllers);
  } else if (stats.failed) {
    ESP_LOGE(TAG, "NVS migration failed; running with unmigrated state");
  }

  m_pal = std::make_unique<Pal>();

  hap::AccessoryServer::Config config;
  config.system = &m_pal->system;
  config.storage = &m_pal->storage;
  config.crypto = &m_pal->crypto;
  config.network = &m_pal->network;
  config.ble = nullptr; // IP-only
  config.device_name = miscConfig.deviceName;
  config.port = 1201;
  config.category_id = hap::core::AccessoryCategory::DoorLock;
  // HAP's SRP password is "Pair-Setup:XXX-XX-XXX" — the code with dashes,
  // which the Home app uses when computing its proof. Config stores the raw
  // 8 digits (no dashes), so format it here. HomeSpan did the same via
  // asprintf("%s:%.3s-%.2s-%.3s", I, code, code+3, code+5).
  config.setup_code = hapSetupCode(miscConfig.setupCode);
  config.on_identify = [] {};
  config.on_pairings_changed = [this](const hap::PairingEvent& event) {
    // Runs on the HAP task; marshal the (potentially slow) issuer sync onto
    // the HAP task itself — we are already on it.
    onPairingsChanged(event);
  };

  // The accessory_id is generated/persisted by the server (or migrated from
  // HomeSpan); leave config.accessory_id empty so the server loads it.
  m_server = std::make_unique<hap::AccessoryServer>(std::move(config));

  auto accessory = std::make_unique<hap::core::Accessory>(1);
  buildAccessoryTree(*accessory);
  if (!m_server->add_accessory(std::move(accessory))) {
    ESP_LOGE(TAG, "add_accessory rejected");
  }

  m_workQueue = xQueueCreate(16, sizeof(std::function<void()>*));

  if (xTaskCreate([](void* arg) {
                    static_cast<HomeKitService*>(arg)->hapTaskLoop();
                  },
                  "hap_task", 12288, this, 5, &m_hapTask) != pdPASS) {
    ESP_LOGE(TAG, "Failed to create HAP task");
    return;
  }

  // Server start must run on the HAP task.
  postToHapTask([this] { m_server->start(); });
}

void HomeKitService::hapTaskLoop() {
  TickType_t lastWake = xTaskGetTickCount();
  int ledPhase = 0;
  while (true) {
    // Drain posted work.
    std::function<void()>* work = nullptr;
    while (xQueueReceive(m_workQueue, &work, 0) == pdTRUE) {
      if (work) {
        (*work)();
        delete work;
      }
    }

    m_server->tick();

    // Status LED blink (~2 Hz unpaired, off when paired).
    if (s_statusLed && *s_statusLed) {
      const bool paired = m_server != nullptr && isPaired();
      ++ledPhase;
      if (!paired) {
        if (ledPhase % 5 == 0) s_statusLed->on();
        if (ledPhase % 5 == 2) s_statusLed->off();
      } else if (ledPhase % 100 == 0) {
        s_statusLed->off();
      }
    }

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(100));
  }
}

void HomeKitService::postToHapTask(std::function<void()> work) {
  auto* heapWork = new std::function<void()>(std::move(work));
  if (xQueueSend(m_workQueue, &heapWork, pdMS_TO_TICKS(1000)) != pdTRUE) {
    ESP_LOGE(TAG, "HAP work queue full, dropping command");
    delete heapWork;
  }
}

void HomeKitService::onPairingsChanged(const hap::PairingEvent& event) {
  ESP_LOGI(TAG, "HomeKit pairing list changed (type %d)", static_cast<int>(event.type));
  reconcileIssuers();
}

void HomeKitService::reconcileIssuers() {
  if (!isPaired()) {
    ESP_LOGW(TAG, "Last controller unpaired. Wiping HomeKey data.");
    m_readerDataManager.deleteAllReaderData();
    return;
  }

  auto pairings = listPairings();
  std::vector<std::vector<uint8_t>> currentIssuerIds;
  bool dataChanged = false;
  for (const auto& p : pairings) {
    std::vector<uint8_t> issuerId = Utils::getHashIdentifier(p.ltpk.data(), 32);
    currentIssuerIds.push_back(issuerId);
    if (m_readerDataManager.addIssuerIfNotExists(issuerId, p.ltpk.data())) {
      ESP_LOGI(TAG, "New controller paired, issuer added.");
      dataChanged = true;
    }
  }

  auto snapshot = m_readerDataManager.issuers();
  for (const auto& issuer : snapshot) {    bool stillPaired = std::any_of(currentIssuerIds.begin(), currentIssuerIds.end(),
                                   [&issuer](const std::vector<uint8_t>& id) {
                                     return issuer.id.size() == id.size() &&
                                            std::equal(issuer.id.begin(), issuer.id.end(), id.begin());
                                   });
    if (!stillPaired) {
      if (m_readerDataManager.removeIssuerIfExists(issuer.id)) {
        ESP_LOGI(TAG, "Controller unpaired, issuer removed.");
        dataChanged = true;
      }
    }
  }

  if (dataChanged) {
    ESP_LOGI(TAG, "Issuer list changed, saving reader data to NVS.");
    m_readerDataManager.save();
  }
}

bool HomeKitService::isPaired() {
  auto data = m_pal->storage.get("pairing_list");
  return data && data->size() > 2; // "[]" is the empty list
}

std::vector<HomeKitService::PairingInfo> HomeKitService::listPairings() {
  std::vector<PairingInfo> out;
  auto list = m_pal->storage.get("pairing_list");
  if (!list) return out;
  std::string_view json(reinterpret_cast<const char*>(list->data()), list->size());
  // Minimal parse of a JSON string array: ["uuid","uuid"]
  size_t pos = 0;
  while ((pos = json.find('"', pos)) != std::string_view::npos) {
    size_t end = json.find('"', pos + 1);
    if (end == std::string_view::npos) break;
    std::string id(json.substr(pos + 1, end - pos - 1));
    pos = end + 1;
    auto ltpk = m_pal->storage.get("pairing_" + id);
    if (!ltpk || ltpk->size() != 32) continue;
    PairingInfo info;
    info.id = std::move(id);
    info.ltpk = *ltpk;
    auto perm = m_pal->storage.get("pairing_" + info.id + "_perm");
    info.admin = perm && !perm->empty() && perm->at(0) == 0x01;
    out.push_back(std::move(info));
  }
  return out;
}

void HomeKitService::setSetupCode(const std::string& code) {
  std::string formatted = hapSetupCode(code);
  postToHapTask([this, formatted] { m_server->set_setup_code(formatted); });
}

void HomeKitService::factoryReset() {
  postToHapTask([this] {
    m_server->factory_reset();
    m_readerDataManager.deleteAllReaderData();
  });
}

std::string HomeKitService::accessoryIdentifier() {
  auto data = m_pal->storage.get("accessory_id");
  if (!data) return "";
  std::string id(data->begin(), data->end());
  std::string out;
  for (char c : id) {
    if (c != ':') out.push_back(c);
  }
  return out;
}

void HomeKitService::updateLockState(uint8_t currentState, uint8_t targetState) {
  postToHapTask([this, currentState, targetState] {
    if (m_lockCurrentState) {
      m_lockCurrentState->set_value(currentState, hap::core::EventSource{});
    }
    if (m_lockTargetState) {
      m_lockTargetState->set_value(targetState, hap::core::EventSource{});
    }
  });
}

void HomeKitService::updateBatteryStatus(uint8_t level, bool low) {
  postToHapTask([this, level, low] {
    if (m_batteryLevel) m_batteryLevel->set_value(level, hap::core::EventSource{});
    if (m_statusLowBattery) m_statusLowBattery->set_value(low ? 1 : 0, hap::core::EventSource{});
  });
}

void HomeKitService::processConsoleCommand(char cmd, const char* arg) {
  switch (cmd) {
  case 'D':
    m_readerDataManager.deleteAllReaderData();
    ESP_LOGI(TAG, "HomeKey Reader Data deleted.");
    break;
  case 'L': {
    esp_log_level_t level = esp_log_level_get("*");
    switch (arg[0]) {
    case 'E': level = ESP_LOG_ERROR; ESP_LOGI(TAG, "ERROR"); break;
    case 'W': level = ESP_LOG_WARN; ESP_LOGI(TAG, "WARNING"); break;
    case 'I': level = ESP_LOG_INFO; ESP_LOGI(TAG, "INFO"); break;
    case 'D': level = ESP_LOG_DEBUG; ESP_LOGI(TAG, "DEBUG"); break;
    case 'V': level = ESP_LOG_VERBOSE; ESP_LOGI(TAG, "VERBOSE"); break;
    case 'N': level = ESP_LOG_NONE; ESP_LOGI(TAG, "NONE"); break;
    default: return;
    }
    esp_log_level_set("*", level);
    break;
  }
  case 'F': {
    ddk::KeyFlow hkFlow = ddk::KeyFlow::kFlowFAST;
    switch (arg[0]) {
    case '0': hkFlow = ddk::KeyFlow::kFlowFAST; ESP_LOGI(TAG, "FAST Flow"); break;
    case '1': hkFlow = ddk::KeyFlow::kFlowSTANDARD; ESP_LOGI(TAG, "STANDARD Flow"); break;
    case '2': hkFlow = ddk::KeyFlow::kFlowATTESTATION; ESP_LOGI(TAG, "ATTESTATION Flow"); break;
    default:
      ESP_LOGI(TAG, "0 = FAST flow, 1 = STANDARD Flow, 2 = ATTESTATION Flow");
      return;
    }
    EventValueChanged s{.newValue = static_cast<uint8_t>(hkFlow)};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    HomekitEvent event{.type = HomekitEventType::DEBUG_AUTH_FLOW, .data = d};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT, event_data.data(),
                          event_data.size());
    break;
  }
  case 'M':
    m_configManager.deleteConfig<espConfig::mqttConfig_t>();
    break;
  case 'N':
    if (m_statusLowBattery) {
      auto battery_level = [this]() -> uint8_t {
        if (!m_batteryLevel) return 100;
        auto v = m_batteryLevel->get_value();
        if (std::holds_alternative<hap::core::Value>(v)) {
          return std::get<uint8_t>(std::get<hap::core::Value>(v));
        }
        return 100;
      };
      if (arg[0] == '0') {
        updateBatteryStatus(battery_level(), false);
        ESP_LOGI(TAG, "Low status set to NORMAL");
      } else if (arg[0] == '1') {
        updateBatteryStatus(battery_level(), true);
        ESP_LOGI(TAG, "Low status set to LOW");
      }
    } else {
      ESP_LOGW(TAG, "Battery service is disabled");
    }
    break;
  case 'B':
    if (m_batteryLevel) {
      updateBatteryStatus(static_cast<uint8_t>(atoi(arg)), false);
    } else {
      ESP_LOGW(TAG, "Battery service is disabled");
    }
    break;
  case 'P': {
    auto issuer_span = m_readerDataManager.issuers();
    ESP_LOGI(TAG, "--- Registered HomeKey Issuers ---");
    if (issuer_span.empty()) ESP_LOGI(TAG, "None");
    for (const auto& issuer : issuer_span) {
      ESP_LOGI(TAG, "ID: %s, PK: %s",
               fmt::format("{:02X}", fmt::join(issuer.id, "")).c_str(),
               fmt::format("{:02X}", fmt::join(issuer.public_key, "")).c_str());
    }
    ESP_LOGI(TAG, "------------------------------------");
    break;
  }
  case 'G': {
    uint8_t i = atoi(arg);
    auto s = GPIOAllocator::instance().owner_of(i);
    ESP_LOGI(TAG, "Owner: %s", s.has_value() ? s->c_str() : "Not allocated");
    break;
  }
  default:
    break;
  }
}
