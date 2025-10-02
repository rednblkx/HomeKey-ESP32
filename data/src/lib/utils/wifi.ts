/**
 * WiFi signal strength categories
 */
export type WifiSignalStrength = 'Excellent' | 'Good' | 'Fair' | 'Poor' | 'Unknown';

/**
 * Cache for WiFi signal strength calculations to avoid repeated computations
 */
const wifiSignalCache = new Map<number, WifiSignalStrength>();

/**
 * Calculate WiFi signal strength from RSSI value
 * @param rssi - RSSI value in dBm (can be undefined)
 * @returns Signal strength category
 */
export function calculateWifiSignal(rssi: number | undefined): WifiSignalStrength {
	if (rssi === undefined) return 'Unknown';

	if (wifiSignalCache.has(rssi)) {
		return wifiSignalCache.get(rssi)!;
	}

	let strength: WifiSignalStrength;
	if (rssi >= -70) {
		strength = 'Excellent';
	} else if (rssi >= -80) {
		strength = 'Good';
	} else if (rssi >= -90) {
		strength = 'Fair';
	} else if (rssi >= -100) {
		strength = 'Poor';
	} else {
		strength = 'Unknown';
	}

	wifiSignalCache.set(rssi, strength);
	return strength;
}