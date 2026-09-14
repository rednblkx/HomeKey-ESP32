#pragma once
#include "app_event_loop.hpp"
#include "hap/AccessoryServer.hpp"
#include "hap/core/Accessory.hpp"
#include "hap/core/TLV8.hpp"
#include "hap/types/ServiceTypes.hpp"
#include "hap/types/CharacteristicTypes.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <functional>
#include <memory>
#include <string>

class LockManager;
class ConfigManager;
class NvsCredentialStore;
class GPIOAllocator;
class GPIOAllocatorGPIOLease;
namespace espConfig { struct misc_config_t; }

/**
 * @brief HAP server front-end built on YAHAP, replacing HomeKitLock/HKServices.
 *
 * Owns the PAL instances and the hap::AccessoryServer. YAHAP has a strict
 * single-thread contract (start/stop/tick/callbacks from one thread), so all
 * server interaction happens on a dedicated HAP task; other subsystems talk to
 * it via the event bus (inbound) and posted command lambdas (outbound).
 */
class HomeKitService {
public:
    HomeKitService(LockManager& lockManager, ConfigManager& configManager,
                   NvsCredentialStore& readerDataManager);
    ~HomeKitService();

    /// Build the accessory tree, spawn the HAP task, start the server.
    void begin();

    /// Pairing-state queries for the web UI.
    bool isPaired();
    struct PairingInfo {
        std::string id;
        std::vector<uint8_t> ltpk;
        bool admin;
    };
    std::vector<PairingInfo> listPairings();

    /// Change the setup code at runtime (web UI). Caller has validated format.
    void setSetupCode(const std::string& code);

    /// Factory reset: clears pairings (web UI "HK Reset"). Also wipes reader data.
    void factoryReset();

    /// Accessory ID without colons ("AABBCCDDEEFF") for MQTT identifier.
    std::string accessoryIdentifier();

    /// Runtime serial-console commands (old SpanUserCommand set).
    void processConsoleCommand(char cmd, const char* arg);

private:
    void buildAccessoryTree(hap::core::Accessory& accessory);
    void hapTaskLoop();
    void postToHapTask(std::function<void()> work);
    void onPairingsChanged(const hap::PairingEvent& event);
    void reconcileIssuers();
    void updateLockState(uint8_t currentState, uint8_t targetState);
    void updateBatteryStatus(uint8_t level, bool low);

    LockManager& m_lockManager;
    ConfigManager& m_configManager;
    NvsCredentialStore& m_readerDataManager;

    // PAL instances must outlive the server; declared before it.
    struct Pal;
    std::unique_ptr<Pal> m_pal;
    std::unique_ptr<hap::AccessoryServer> m_server;

    TaskHandle_t m_hapTask = nullptr;
    QueueHandle_t m_workQueue = nullptr;

    // Characteristic handles (raw pointers; owned by the accessory tree).
    hap::core::Characteristic* m_lockCurrentState = nullptr;
    hap::core::Characteristic* m_lockTargetState = nullptr;
    hap::core::Characteristic* m_statusLowBattery = nullptr;
    hap::core::Characteristic* m_batteryLevel = nullptr;
    hap::core::Characteristic* m_nfcControlPoint = nullptr;

    AppEventLoop::SubscriptionHandle m_lock_state_changed;
    AppEventLoop::SubscriptionHandle m_hk_event;

    static const char* TAG;
    static HomeKitService* s_instance;
};
