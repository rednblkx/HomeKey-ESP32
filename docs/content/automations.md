---
title: "HASS Automations"
weight: 6
---

## Examples

Here you can find some example on how to setup simple automations using MQTT Topics or the Tags.

### ~ Synchronizing Physical Lock State with HomeKey-ESP32 ~

```yaml
alias: NFC Lock State change
description: ""
trigger:
  - platform: state
    entity_id:
      - lock.hk_lock_2
condition:
  - condition: template
    value_template: >-
      {{ not( now() - state_attr('automation.nfc_lock', 'last_triggered') <
      timedelta(seconds=10)) }}
action:
  - if:
      - condition: state
        entity_id: lock.hk_lock_2
        state: unlocked
    then:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
    else:
      - if:
          - condition: state
            entity_id: lock.hk_lock_2
            state: locked
        then:
          - action: lock.lock
            metadata: {}
             data: {}
            target:
              entity_id: lock.l600137
mode: single
```

### ~ Synchronizing HomeKey-ESP32 with the physical lock ~

```yaml
alias: NFC Lock State change
description: ""
trigger:
  - platform: state
    entity_id:
      - lock.hk_lock
    from: null
    to: null
condition:
  - condition: template
    value_template: >-
      {{ not( now() - state_attr('automation.nfc_lock', 'last_triggered') <
      timedelta(seconds=10)) }}
action:
  - if:
      - condition: state
        entity_id: lock.hk_lock
        state: unlocked
    then:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
    else:
      - if:
          - condition: state
            entity_id: lock.hk_lock
            state: locked
        then:
          - action: lock.lock
            metadata: {}
            data: {}
            target:
              entity_id: lock.l600137
mode: single
```

### ~ HomeKey MQTT Topic ~

```yaml
alias: Homekey
description: ""
trigger:
  - platform: mqtt
    topic: topic/homekey/auth
condition:
  - condition: template
    value_template: "{{trigger.payload_json[\"issuerId\"] == \"0123456789ABCDEF\"}}"
action:
  - if:
      - condition: state
        entity_id: lock.l600137
        state: locked
    then:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
    else:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
mode: restart
```

### ~ HomeKey HASS Tag ~

```yaml
description: ""
mode: single
trigger:
  - platform: tag
    tag_id: 0123456789ABC
condition: []
action:
  - if:
      - condition: state
        entity_id: lock.l600137
        state: locked
    then:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
    else:
      - action: lock.unlock
        metadata: {}
        data: {}
        target:
          entity_id: lock.l600137
alias: Tag Red's Watch is scanned
```
