/**
 * System information interface
 * @typedef {Object} SystemInfo
 * @property {string} deviceName - Device name
 * @property {string} version - Firmware version
 * @property {number} uptime - System uptime in seconds
 * @property {number} free_heap - Free heap size
 * @property {string} wifi_ssid - WiFi network SSID
 * @property {number} wifi_rssi - WiFi signal strength (RSSI)
 * @property {boolean} eth_enabled - Whether Ethernet is enabled
 * @property {number} log_level - Whether Ethernet is enabled
 */

/**
 * System information store using Svelte 5 runes
 * @type {SystemInfo}
 */
export const systemInfo = $state({
  deviceName: '',
  version: '',
  uptime: 0,
  free_heap: 0,
  wifi_ssid: '',
  wifi_rssi: 0,
  eth_enabled: false,
  log_level: 2
});

/**
 * Update system information from API response
 * @param {Partial<SystemInfo>} newInfo - Partial system information to update
 */
export function updateSystemInfo(newInfo) {
  Object.assign(systemInfo, newInfo);
}
