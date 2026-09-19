#pragma once
#include "HomeSpan.h"
#include "app_event_loop.hpp"
#include <memory>
#include "AccessCodeManager.hpp"
#include "KeypadManager.hpp"

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
  struct AccessCode : SpanService
  {
    AccessCode();
  };
}

class LockManager;
class ConfigManager;
class NvsCredentialStore;
class AccessCodeManager;
class KeypadManager;
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

    std::unique_ptr<AccessCodeManager> m_accessCodeManager;
    std::unique_ptr<KeypadManager> m_keypad;

    std::function<void(int)> &conn_cb;

    void initializeETH();
    void initKeypad();

    static void connectionEstablished(int status);
    static void apStarted();

    static void staticControllerCallback();
    void controllerCallback();
    void setupDebugCommands();

    static const char* TAG;
    AppEventLoop::SubscriptionHandle m_lock_state_changed;
    AppEventLoop::SubscriptionHandle m_hk_event;
    AppEventLoop::SubscriptionHandle m_keypad_event;

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
    struct AccessCodeService : Service::AccessCode {
        AccessCodeManager& m_accessCodeManager;
        SpanCharacteristic* m_controlPoint;
        AccessCodeService(AccessCodeManager& accessCodeManager);
        boolean update() override;
    };
};
