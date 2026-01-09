#pragma once
#include "HomeSpan.h"
#include "event_bus.hpp"

namespace Service
{
  struct LockManagement : SpanService
  {
    LockManagement();
  };
  struct NFCAccess : SpanService
  {
    NFCAccess();
  };
}

class LockManager;
class ConfigManager;
class ReaderDataManager;
namespace espConfig { struct misc_config_t; };

class HomeKitLock {
public:
    HomeKitLock(std::function<void(int)> &conn_cb, LockManager& lockManager, ConfigManager& configManager, ReaderDataManager& readerDataManager);
    /**
 * @brief Unsubscribes HomeKitLock from its EventBus topics.
 *
 * Ensures the instance's subscriptions for lock state changes and HomeKit events are removed from the global EventBus when the object is destroyed.
 */
~HomeKitLock() {EventBus::Bus::instance().unsubscribe(m_lock_state_changed);EventBus::Bus::instance().unsubscribe(m_hk_event);}
    void begin();
    void updateLockState(int currentState, int targetState);
    void updateBatteryStatus(uint8_t batteryLevel, bool isLow);

private:
    SpanCharacteristic* m_lockCurrentState;
    SpanCharacteristic* m_lockTargetState;
    SpanCharacteristic* m_statusLowBattery;
    SpanCharacteristic* m_batteryLevel;

    LockManager& m_lockManager;
    ConfigManager& m_configManager;
    ReaderDataManager& m_readerDataManager;

    std::function<void(int)> &conn_cb;

    void initializeETH();
    static void ethEventHandler(arduino_event_id_t event, arduino_event_info_t info);

    static void connectionEstablished(int status);

    static void staticControllerCallback();
    void controllerCallback();
    void setupDebugCommands();

    static const char* TAG;
    const EventBus::TopicHandle bus_topic;
    EventBus::SubscriberHandle m_lock_state_changed;
    EventBus::SubscriberHandle m_hk_event;

    struct NFCAIS : Service::AccessoryInformation {
      NFCAIS(const espConfig::misc_config_t& config);
    };
    struct LockManagementService : Service::LockManagement {
      LockManagementService();
    };
    struct LockMechanismService : Service::LockMechanism {
      LockManager& m_lockManager;
      SpanCharacteristic* m_lockTargetState;
      SpanCharacteristic* m_lockCurrentState;
      LockMechanismService(HomeKitLock& bridge, LockManager& lockManager);
      boolean update() override;
    };
    struct NFCAccessService : Service::NFCAccess {
        ReaderDataManager& m_readerDataManager;
        SpanCharacteristic* m_nfcControlPoint;
        NFCAccessService(ReaderDataManager& readerDataManager);
        boolean update() override;
    };
    struct PhysicalLockBatteryService : Service::BatteryService {
        PhysicalLockBatteryService(HomeKitLock& bridge);
    };
};
