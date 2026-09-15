---
title: "EthernetDriver"
---

## 1. Overview

The `EthernetDriver` namespace owns Ethernet bring-up and the Arduino Ethernet event handler. It was extracted from `HomeKitLock` so that network initialization, GPIO pin leasing, and lifecycle event handling live in a single dedicated module, decoupled from the HomeKit accessory logic.

The driver supports both SPI-based Ethernet modules (e.g., W5500, DM9051, KSZ8851) on any target, and RMII PHYs (e.g., LAN8720, TLK110) on chips with an internal Ethernet MAC (ESP32). Configuration comes either from a named board preset (`ethActivePreset`) or from custom pin arrays (`ethSpiConfig` / `ethRmiiConfig`), mirroring the options exposed in the WebUI.

### Key Responsibilities

*   **Bring-Up:** Validates the configuration, leases all required GPIO pins via the `GPIOAllocator`, and starts the Arduino `ETH` peripheral with the preset or custom parameters.
*   **Event Handling:** Registers the Arduino Ethernet event handler and mirrors every lifecycle event (start, link, IP assignment, loss, stop) onto the application event bus as `ETH_APP_EVENT`.
*   **Hostname Management:** Sets the Ethernet hostname (`ESP32_XXXXXX`, derived from the MAC) when the interface starts.
*   **SPI Bus Sharing:** When Ethernet shares `SPI2_HOST` with an NFC reader, validates that SCK/MISO/MOSI exactly match the NFC reader's bus pins before proceeding, preventing silent bus conflicts.

### Architecture

*   **Stateless Namespace:** The driver is a free-function namespace, not a class; internal state is limited to idempotency flags.
*   **Event-Driven Consumers:** Other components (e.g., the captive-portal save flow) subscribe to `ETH_APP_EVENT` and wait for `ETH_GOT_IP` instead of polling the driver.
*   **RAII Pin Leases:** All bus, CS, IRQ, and RST pins are held as `GPIOAllocator::GPIOLease` instances and released automatically on failure paths.

---

## 2. Public API

### `start()`

```cpp
namespace EthernetDriver {
bool start(const espConfig::misc_config_t& config);
}
```

Initializes Ethernet according to the supplied miscellaneous configuration. Returns `true` if the driver is running (or already running), `false` if Ethernet is disabled or bring-up failed.

**Behavior details:**

*   **Idempotent:** The driver can be started twice in one boot — once during captive-portal provisioning (to verify the module responds) and once on the normal boot path. A later `start()` with the driver already running is a no-op that reports success.
*   **Retry Semantics:** If a previous bring-up attempt failed (`ETH.begin()` error, missing pins, or pin conflicts), a subsequent `start()` call is the only way the driver re-attempts initialization; it logs a warning and retries.
*   **Disabled Configuration:** If `ethernetEnabled` is `false`, the call is a no-op returning `false` and HomeSpan manages Wi-Fi instead.
*   **Validation Failures:** Returns `false` (without starting) when the preset index is out of range, the custom PHY type is unsupported, required SPI pins are unset (255), or — when sharing `SPI2_HOST` with NFC — the bus pins do not match the NFC reader's SCK/MISO/MOSI.

---

## 3. Events (`ETH_APP_EVENT`)

The driver publishes lifecycle events on the application event loop under the `ETH_APP_EVENT` base (the plain `ETH_EVENT` base is taken by `esp_eth`, hence the `APP_` prefix). Event payloads, where present, are the IP address as a string.

| Event ID | Payload | Description |
|----------|---------|-------------|
| `ETH_STARTED` | none | Interface started; hostname set from the MAC. |
| `ETH_GOT_IP` | IP string (`char[16]`) | DHCP lease acquired; payload is the assigned address. |
| `ETH_LOST_IP` | none | IP address lost. |
| `ETH_DISCONNECTED` | none | Link lost. |
| `ETH_STOPPED` | none | Interface stopped. |

### Example: Waiting for an IP Assignment

```cpp
auto sub = AppEventLoop::subscribe(ETH_APP_EVENT, ETH_GOT_IP,
    [](const uint8_t* data, size_t size) {
        std::string ip(reinterpret_cast<const char*>(data), size);
        ESP_LOGI("Example", "Ethernet ready, IP: %s", ip.c_str());
    });
```

---

## 4. Configuration Fields

The driver reads the following members of `espConfig::misc_config_t`:

| Field | Meaning |
|-------|---------|
| `ethernetEnabled` | Master enable; when `false` the driver stays idle. |
| `ethActivePreset` | Index into `eth_config_ns::boardPresets`, or `255` for a custom configuration. |
| `ethPhyType` | PHY type identifier used for custom configurations (must exist in `supportedChips`). |
| `ethSpiBus` | SPI host (`SPI2_HOST` or `SPI3_HOST`); out-of-range values fall back to `SPI2_HOST`. |
| `ethSpiConfig` | `{freq_MHz, CS, IRQ, RST, SCK, MISO, MOSI}` for custom SPI Ethernet. |
| `ethRmiiConfig` | `{phy_addr, MDC, MDIO, power, clock_mode}` for custom RMII Ethernet. |

Board presets and supported PHY chips are defined in `main/include/eth_structs.hpp`.
