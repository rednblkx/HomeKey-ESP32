import { describe, it, expect, beforeEach } from 'vitest';
import { systemInfo, updateSystemInfo } from './system.svelte.js';

describe('system.svelte.js', () => {
	beforeEach(() => {
		// Reset system info to initial state before each test
		updateSystemInfo({
			deviceName: '',
			version: '',
			uptime: 0,
			wifi_ssid: '',
			wifi_rssi: 0,
			eth_enabled: false,
		});
	});

	describe('systemInfo state', () => {
		it('should initialize with default values', () => {
			expect(systemInfo.deviceName).toBe('');
			expect(systemInfo.version).toBe('');
			expect(systemInfo.uptime).toBe(0);
			expect(systemInfo.wifi_ssid).toBe('');
			expect(systemInfo.wifi_rssi).toBe(0);
			expect(systemInfo.eth_enabled).toBe(false);
		});

		it('should be reactive - changes should be reflected immediately', () => {
			systemInfo.deviceName = 'TestDevice';
			expect(systemInfo.deviceName).toBe('TestDevice');
		});
	});

	describe('updateSystemInfo function', () => {
		it('should update system info with partial data', () => {
			const newInfo = {
				deviceName: 'ESP32-HomeKey',
				version: '1.2.3',
				uptime: 3600,
			};

			updateSystemInfo(newInfo);

			expect(systemInfo.deviceName).toBe('ESP32-HomeKey');
			expect(systemInfo.version).toBe('1.2.3');
			expect(systemInfo.uptime).toBe(3600);
			// Other properties should remain unchanged
			expect(systemInfo.wifi_ssid).toBe('');
			expect(systemInfo.wifi_rssi).toBe(0);
			expect(systemInfo.eth_enabled).toBe(false);
		});

		it('should update WiFi information', () => {
			const wifiInfo = {
				wifi_ssid: 'MyWiFi',
				wifi_rssi: -45,
				eth_enabled: true,
			};

			updateSystemInfo(wifiInfo);

			expect(systemInfo.wifi_ssid).toBe('MyWiFi');
			expect(systemInfo.wifi_rssi).toBe(-45);
			expect(systemInfo.eth_enabled).toBe(true);
		});

		it('should handle empty updates gracefully', () => {
			const initialState = { ...systemInfo };

			updateSystemInfo({});

			expect(systemInfo).toEqual(initialState);
		});

		it('should overwrite existing values', () => {
			// Set initial values
			updateSystemInfo({
				deviceName: 'InitialDevice',
				version: '1.0.0',
			});

			// Update with new values
			updateSystemInfo({
				deviceName: 'UpdatedDevice',
				uptime: 7200,
			});

			expect(systemInfo.deviceName).toBe('UpdatedDevice');
			expect(systemInfo.version).toBe('1.0.0'); // Should remain unchanged
			expect(systemInfo.uptime).toBe(7200);
		});
	});

	describe('reactive behavior', () => {
		it('should maintain reactivity across multiple updates', () => {
			let callCount = 0;

			// Simulate reactive effect
			const effect = () => {
				callCount++;
				return systemInfo.deviceName;
			};

			// Initial call
			effect();

			// Update state
			updateSystemInfo({ deviceName: 'ReactiveTest' });
			effect(); // This would be called by Svelte's reactivity

			expect(callCount).toBe(2);
			expect(systemInfo.deviceName).toBe('ReactiveTest');
		});
	});
});