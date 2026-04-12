export type SystemInfo = {
  deviceName: string,
  version: string,
  uptime: number,
  free_heap: number,
  wifi_ssid: string,
  wifi_rssi: number,
  eth_enabled: boolean,
  log_level: number,
  chip_model: number
};

export const systemInfo : SystemInfo = $state({
  deviceName: '',
  version: '',
  uptime: 0,
  free_heap: 0,
  wifi_ssid: '',
  wifi_rssi: 0,
  eth_enabled: false,
  log_level: 2,
  chip_model: 0
});

/**
 * Update system information from API response
 */
export function updateSystemInfo(newInfo : Partial<SystemInfo>) {
  Object.assign(systemInfo, newInfo);
}
