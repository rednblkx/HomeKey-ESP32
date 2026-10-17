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
class AccessCodeManager;
class KeypadManager;
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
    /// Retain managers and subscribe to lock, HomeKit, and doorbell events.
    /// Managers must outlive this instance; constructing a second instance restarts the device.
    HomeKitService(LockManager& lockManager, ConfigManager& configManager,
                   NvsCredentialStore& readerDataManager);
    /// Delete the HAP task and work queue and release owned server resources.
    ~HomeKitService();

    /// Initialize keypad/status LED, attempt legacy migration, and build the accessory tree.
    /// Spawn the HAP task and queue server startup; return without starting it if task creation fails.
    void begin();

    /// Return whether stored pairing-list data is longer than the empty array.
    /// Call after begin(); missing or unreadable storage returns false.
    bool isPaired();
    struct PairingInfo {
        std::string id;
        std::vector<uint8_t> ltpk;
        bool admin;
    };
    /// Read stored controller pairings after begin(); skip missing or non-32-byte keys.
    /// An unreadable list returns empty; missing permissions default to non-admin.
    std::vector<PairingInfo> listPairings();

    /// Queue a setup-code change. Caller has validated format; queue overflow drops the change.
    void setSetupCode(const std::string& code);

    /// Queue a server factory reset and reader-data deletion; queue overflow drops the request.
    void factoryReset();

    /// Read the accessory ID without colons ("AABBCCDDEEFF") for MQTT after begin().
    /// Return empty if storage is unavailable.
    std::string accessoryIdentifier();

    /// Dispatch console commands for reader/config reset, diagnostics, auth flow, and battery state.
    /// arg must be a non-null, NUL-terminated string; unknown commands are ignored.
    void processConsoleCommand(char cmd, const char* arg);

private:
    /// Add lock/NFC services and configured battery/keypad services, wire writes to app events,
    /// and publish the initial lock override state. The accessory owns the characteristic handles.
    void buildAccessoryTree(hap::core::Accessory& accessory);
    /// Initialize access codes and keypad callbacks if enabled; discard both managers if hardware startup fails.
    void initKeypad();
    /// Continuously drain queued work, tick the server, and update the status LED every 100 ms.
    void hapTaskLoop();
    /// Queue work after begin(); wait up to one second for space, then discard it if still full.
    void postToHapTask(std::function<void()> work);
    /// Reconcile HomeKey issuers against stored pairings after any pairing event.
    void onPairingsChanged(const hap::PairingEvent& event);
    /// Synchronize and save HomeKey issuers from pairings; if unpaired, wipe reader data and access codes.
    void reconcileIssuers();
    /// Queue current and target HAP lock states for any existing lock characteristics.
    void updateLockState(uint8_t currentState, uint8_t targetState);
    /// Queue battery percentage and low-battery flag updates if those characteristics exist.
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
    hap::core::Characteristic* m_accessCodeControlPoint = nullptr;
    hap::core::Characteristic* m_doorbellSwitchEvent = nullptr;

    std::unique_ptr<AccessCodeManager> m_accessCodeManager;
    std::unique_ptr<KeypadManager> m_keypad;

    AppEventLoop::SubscriptionHandle m_lock_state_changed;
    AppEventLoop::SubscriptionHandle m_hk_event;
    AppEventLoop::SubscriptionHandle m_keypad_event;
    AppEventLoop::SubscriptionHandle m_doorbell_event;

    static const char* TAG;
    static HomeKitService* s_instance;
};
