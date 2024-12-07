# Provisioning Application Examples

This primarily consists of two examples `wifi_prov` and `thread_prov`.

* wifi_prov
    Abstracts out most of the complexity of Wi-Fi provisioning and allows easy switching between the SoftAP (using HTTP) and BLE transports. It also demonstrates how applications can register and use additional custom data endpoints.

* thread_prov
    Abstracts out most of the complexity of Thread provisioning over BLE transport. It also demonstrates how applications can register and use additional custom data endpoints.

Provisioning applications are available for `Linux / Windows / macOS` platform as `esp_prov.py` [script](../tool/esp_prov/esp_prov.py)
