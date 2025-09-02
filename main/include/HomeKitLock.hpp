#pragma once
#include "HomeSpan.h"

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

    static void connectionEstablished(int status);

    static void staticControllerCallback();
    void controllerCallback();
    void setupDebugCommands();

    static const char* TAG;

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

