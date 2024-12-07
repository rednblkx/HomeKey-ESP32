# 19-June-2024

- Change the proto files to make the network provisioning component stay backward compatible with the wifi_provisioing

# 23-April-2024

- Add `wifi_prov` or `thread_prov` in provision capabilities in the network provisioning manager for the provisioner to distinguish Thread or Wi-Fi devices

# 16-April-2024

- Move wifi_provisioning component from ESP-IDF at commit 5a40bb8746 and rename it to network_provisioning with the addition of Thread provisioning support.
- Update esp_prov tool to support both Wi-Fi provisioning and Thread provisioning.
- Create thread_prov and wifi_prov examples
