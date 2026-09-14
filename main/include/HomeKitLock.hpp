#pragma once
#include "HomeSpan.h"
#include "app_event_loop.hpp"

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
class NvsCredentialStore;
namespace espConfig { struct misc_config_t; };

class HomeKitLock {
public:
    HomeKitLock(std::function<void(int)> &conn_cb, LockManager& lockManager, ConfigManager& configManager, NvsCredentialStore& readerDataManager);
    /**
 * @brief Unsubscribes HomeKitLock from its EventBus topics.
 *
 * Ensures the instance's subscriptions for lock state changes and HomeKit events are removed from the global EventBus when the object is destroyed.
 */
~HomeKitLock() = default;
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
    NvsCredentialStore& m_readerDataManager;

    std::function<void(int)> &conn_cb;

    void initializeETH();

    static void connectionEstablished(int status);
    static void apStarted();

    static void staticControllerCallback();
    void controllerCallback();
    void setupDebugCommands();

    static const char* TAG;
    AppEventLoop::SubscriptionHandle m_lock_state_changed;
    AppEventLoop::SubscriptionHandle m_hk_event;

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
        NvsCredentialStore& m_readerDataManager;
        SpanCharacteristic* m_nfcControlPoint;
        NFCAccessService(NvsCredentialStore& readerDataManager);
        boolean update() override;
    };
    struct PhysicalLockBatteryService : Service::BatteryService {
        PhysicalLockBatteryService(HomeKitLock& bridge);
    };
};
