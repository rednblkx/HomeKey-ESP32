---
title: "MQTT"
weight: 4
---

# Your HomeKey-ESP32's Secret Language

Welcome to the world of MQTT, where your HomeKey-ESP32 whispers secrets to your smart home! This project uses MQTT (Message Queuing Telemetry Transport) for reporting its status and receiving commands. Think of it as a super-efficient postal service for your smart devices.

## 1. Understanding Lock States

Your HomeKey-ESP32 manages the lock's state using a state machine. Here are the states you'll encounter:

*   `UNLOCKED` (0)
*   `LOCKED` (1)
*   `JAMMED` (2) - Uh oh, something's stuck!
*   `UNKNOWN` (3) - The lock's current state is a mystery.
*   `UNLOCKING` (4) - The lock is busy opening its heart (or door).
*   `LOCKING` (5) - The lock is diligently securing things.

The `UNLOCKING` and `LOCKING` states are used to indicate that the lock is in the process of changing state. The `JAMMED` state means the lock encountered an issue during an operation.

> [!IMPORTANT]
> Because the project doesn't actually implement a real lock, we cannot know what the current state should be, which means when the state is `UNLOCKING` or `LOCKING`, it is up to you to update the current state to one of the states seen above, see [Core MQTT Topics](#2-core-mqtt-topics-the-main-conversation) below

## 2. Core MQTT Topics: The Main Conversation

Your HomeKey-ESP32 communicates its status and listens for commands on these core MQTT topics. Remember, you can customize these topics in the device's [Web Interface Configuration](../configuration#core-topics)!

| Topic | Description | Payload Examples |
| :---- | :---------- | :--------------- |
| `<CLIENT_ID>/homekit/state` | **Publishes** the current state of the lock. This topic is retained, meaning new subscribers will immediately receive the last known state. | `0` (UNLOCKED), `1` (LOCKED), `2` (JAMMED), `3` (UNKNOWN), `4` (UNLOCKING), `5` (LOCKING) |
| `<CLIENT_ID>/homekit/set_state` | **Subscribes** to this topic to set both the current and target state of the lock. | To unlock: `0` To lock: `1` |
| `<CLIENT_ID>/homekit/set_current_state` | **Subscribes** to this topic to set the current state of the lock. Useful for external systems to update the device's perceived state. | `0` (UNLOCKED), `1` (LOCKED), `2` (JAMMED), `3` (UNKNOWN) |
| `<CLIENT_ID>/homekit/set_target_state` | **Subscribes** to this topic to set the target state of the lock. | To unlock: `0` To lock: `1` |
| `<CLIENT_ID>/homekit/set_battery_lvl` | **Subscribes** to this topic to set the battery level to be shown in HomeKit. This is used if "Proxy Battery Enabled" is active in the WebUI. | `85` (for 85% battery) |
| `<CLIENT_ID>/alt_action` | **Publishes** the status of the Alternate Action. | (Specific payload depends on configuration) |

> [!NOTE]
> You will notice all topics set by default are prefixed by the client id, this was done so the topics are nicely organized under a unique identifier, however, you can set the topics to whatever you wish from the [WebUI](../configuration#core-topics)

**Example: Manually Locking Your Door via MQTT**

Let's say you want to lock your door using a command-line MQTT client like `mosquitto_pub`.

```bash
mosquitto_pub -h your_mqtt_broker_address -p 1883 -t homekit/set_state -m "1"
```
(Replace `your_mqtt_broker_address` with your actual broker's IP or hostname.)

**Example: Subscribing to Lock State Updates**

To see your lock's current state in real-time:

```bash
mosquitto_sub -h your_mqtt_broker_address -p 1883 -t homekit/state
```

## 3. Custom States: Speaking Your Lock's Dialect

The project supports custom states, allowing you to map the lock's internal states to values that might be more familiar to your specific MQTT-based lock or system. This is super handy for seamless integration!

*   **`<CLIENT_ID>/homekit/custom_state`**: **Publishes** the custom lock state.
*   **`<CLIENT_ID>/homekit/set_custom_state`**: **Subscribes** to this topic to set the custom lock state.

You can enable and configure custom states, including defining your own custom lock actions and states, in the [Web Interface Configuration](../configuration/#322-custom-topics) under the "Custom Topics" section.

## 4. NFC Data: Who Just Tapped? 🕵️‍♀️

When a HomeKey is used to authenticate, or a generic NFC tag is scanned, the device publishes data to the `homekey/auth` topic in a neat JSON format.

### 4.1. HomeKey Authentication Data

If a HomeKey is used:

```json
{
  "endpointId": "000000000000",
  "homekey": true,
  "issuerId": "0000000000000000"
}
```

*   `endpointId`: A unique identifier for the Apple device (iPhone, Apple Watch) that was used to authenticate.
*   `issuerId`: A unique identifier for the Apple ID of the user who authenticated.

### 4.2. Generic NFC Tag Data

If a generic NFC tag (not a HomeKey) is scanned:

```json
{
  "atqa": "0004",
  "homekey": false,
  "sak": "08",
  "uid": "00000000"
}
```

*   `atqa`: The ATQA of the NFC tag.
*   `sak`: The SAK of the NFC tag.
*   `uid`: The UID of the NFC tag.

## 5. Home Assistant Integration: Making Friends with Your Hub

This project plays nicely with Home Assistant, allowing you to manage your lock and unleash the power of NFC tags!

### 5.1. Automatic Discovery - Magic! 🔮

Home Assistant has a feature called [MQTT Discovery](https://www.home-assistant.io/integrations/mqtt/) used to automatically configure a component.

If **HASS MQTT Discovery** is enabled in your device's [MQTT Configuration](../configuration#mqtt-settings), the project will automatically publish the necessary configuration for Home Assistant to discover and control the lock. No manual YAML configuration needed – Home Assistant will just *find* it!

*   **Important:** MQTT Discovery is disabled by default. Make sure to enable it in the device's WebUI configuration.
*   The lock's online/offline status is published to `<MQTT_CLIENTID>/status` via MQTT Last Will and Testament (LWT). This helps Home Assistant know if your device is alive and kicking.

### 5.2. Lock Control

Once discovered, you can control your lock directly from Home Assistant's UI, just like any other lock entity.

### 5.3. NFC Tag Integration: Automate All the Things!

This is where the real fun begins! Your HomeKey-ESP32 can integrate with Home Assistant's powerful Tag functionality, letting you trigger automations by simply scanning an NFC tag.

*   **To find tags:** Navigate to the 'Tags' section in your Home Assistant instance (usually under `Settings -> Devices & Services -> Tags`).
> [!NOTE]
> When tapping a HomeKey, two Tags will appear here.
> This is because HomeKey carries two IDs:
> * Issuer ID (16 characters) - A unique identifier for the Apple ID of the user who authenticated.
> * Endpoint ID (12 characters) - A unique identifier for the Apple device (iPhone, Apple Watch) that was used to authenticate.

*   **Prerequisites:** Ensure your Home Assistant MQTT integration is configured with the same broker as the ESP32, and the discovery prefix is the default `homeassistant`.
*   **Creating Automations:** Once a tag is discovered in Home Assistant, you can name it and create custom automations based on its scan. For example, scan a tag to:
    *   Unlock the door.
    *   Turn on lights.
    *   Disarm your alarm system.
    *   Brew coffee (if you have a smart coffee maker, of course!).

**Example Home Assistant Automation (Conceptual):**

Let's imagine you want to turn on a light when a specific NFC tag is scanned.

```yaml
automation:
  - alias: "NFC Tag Scanned - Turn on Living Room Light"
    trigger:
      platform: event
      event_type: tag_scanned
      event_data:
        tag_id: "your_nfc_tag_uid_here" # Replace with the UID of your NFC tag
    action:
      - service: light.turn_on
        target:
          entity_id: light.living_room_light
```
(You'll find the `tag_id` in Home Assistant's 'Tags' section after scanning your tag with the HomeKey-ESP32.)

---

By leveraging MQTT, your HomeKey-ESP32 becomes a powerful component in your smart home ecosystem, enabling seamless communication and automation!
