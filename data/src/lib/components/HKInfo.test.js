import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen, waitFor } from '@testing-library/svelte';
import { tick } from 'svelte';
import HKInfo from './HKInfo.svelte';
import { systemInfo, updateSystemInfo } from '$lib/stores/system.svelte.js';
import { loadingStates, setLoadingState } from '$lib/stores/loading.svelte.js';

// Mock the WebSocketTest component
vi.mock('$lib/components/WebSocketTest.svelte', () => ({
	default: vi.fn().mockImplementation(() => ({
		$$: { on_mount: [], on_destroy: [] }
	}))
}));

describe('HKInfo.svelte', () => {
	beforeEach(() => {
		// Reset system info
		updateSystemInfo({
			deviceName: 'TestDevice',
			version: '1.0.0',
			uptime: 3600,
			wifi_ssid: 'TestWiFi',
			wifi_rssi: -50,
			eth_enabled: false,
		});

		// Reset loading states
		setLoadingState('hkInfoLoading', false);
		setLoadingState('systemInfoLoading', false);
	});

	describe('prop handling', () => {
		it('should render HKInfo data when provided', () => {
			const mockHkInfo = {
				group_identifier: 'test-group-123',
				unique_identifier: 'test-unique-456',
				issuers: [
					{
						issuerId: 'issuer-1',
						endpoints: [
							{ endpointId: 'endpoint-1' },
							{ endpointId: 'endpoint-2' }
						]
					}
				]
			};

			render(HKInfo, { props: {
				hkInfo: mockHkInfo,
				error: null
			} });

			expect(screen.getByText('test-group-123')).toBeInTheDocument();
			expect(screen.getByText('test-unique-456')).toBeInTheDocument();
			expect(screen.getByText('1')).toBeInTheDocument(); // issuers count
		});

		it('should display "N/A" for missing data', () => {
			const mockHkInfo = {
				group_identifier: '',
				unique_identifier: '',
				issuers: []
			};

			render(HKInfo, { props: {
				hkInfo: mockHkInfo,
				error: null
			} });

			expect(screen.getAllByText('N/A')).toHaveLength(2); // group and unique identifier
			expect(screen.getByText('0')).toBeInTheDocument(); // issuers count
		});

		it('should render error message when error prop is provided', () => {
			const errorMessage = 'Failed to load HK info';

			render(HKInfo, { props: {
				error: errorMessage,
				hkInfo: null
			} });

			expect(screen.getByText(`Error: ${errorMessage}`)).toBeInTheDocument();
		});

		it('should handle undefined hkInfo prop', () => {
			render(HKInfo, { props: { hkInfo: null, error: null } });

		// Should not show HomeKey card when hkInfo is undefined
			expect(screen.queryByText('HomeKey')).not.toBeInTheDocument();
			// Should still show System info
			expect(screen.getByText('System')).toBeInTheDocument();
		});
	});

	describe('loading states', () => {
		it('should show loading spinner for HK info when loading', () => {
			setLoadingState('hkInfoLoading', true);

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			// Check for the loading spinner with aria-label
			const spinner = screen.getByLabelText('Loading HomeKey data');
			expect(spinner).toBeInTheDocument();
			expect(spinner).toHaveClass('loading', 'loading-spinner', 'loading-lg');
		});

		it('should show loading skeleton for system info when loading', () => {
			setLoadingState('systemInfoLoading', true);

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			// Should have skeleton elements
			const skeletons = document.querySelectorAll('.skeleton');
			expect(skeletons.length).toBeGreaterThan(0);
		});

		it('should render system info when not loading', async () => {
			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });
			await tick();
			setLoadingState('systemInfoLoading', false);
			await tick();

			expect(screen.getByText('TestDevice')).toBeInTheDocument();
			expect(screen.getByText('1.0.0')).toBeInTheDocument();
			expect(screen.getByText('3600')).toBeInTheDocument();
		});
	});

	describe('reactive derivations', () => {
		it('should calculate WiFi signal strength correctly', async () => {
			updateSystemInfo({ wifi_rssi: -30 });
			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });
			await tick();
			setLoadingState('systemInfoLoading', false);
			await tick();

			expect(screen.getByText('-30 (Excellent)')).toBeInTheDocument();
		});

		it('should handle undefined RSSI', async () => {
			updateSystemInfo({ wifi_rssi: undefined });
			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });
			await tick();
			setLoadingState('systemInfoLoading', false);
			await tick();

			expect(screen.getByText('N/A (Unknown)')).toBeInTheDocument();
		});

		it('should show Ethernet status correctly', async () => {
			updateSystemInfo({ eth_enabled: true });
			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });
			await tick();
			setLoadingState('systemInfoLoading', false);
			await tick();

			expect(screen.getByText('Yes')).toBeInTheDocument();
		});
	});

	describe('issuers display', () => {
		it('should render issuers with collapsible content', () => {
			const mockHkInfo = {
				group_identifier: 'test-group',
				unique_identifier: 'test-unique',
				issuers: [
					{
						issuerId: 'issuer-1',
						endpoints: [
							{ endpointId: 'endpoint-1' },
							{ endpointId: 'endpoint-2' }
						]
					},
					{
						issuerId: 'issuer-2',
						endpoints: []
					}
				]
			};

			render(HKInfo, { props: {
				hkInfo: mockHkInfo,
				error: null
			} });

			expect(screen.getByText('issuer-1')).toBeInTheDocument();
			expect(screen.getByText('issuer-2')).toBeInTheDocument();
			expect(screen.getByText('endpoint-1')).toBeInTheDocument();
			expect(screen.getByText('endpoint-2')).toBeInTheDocument();
		});

		it('should handle issuers without endpoints', () => {
			const mockHkInfo = {
				group_identifier: 'test-group',
				unique_identifier: 'test-unique',
				issuers: [
					{
						issuerId: 'issuer-no-endpoints',
						endpoints: []
					}
				]
			};

			render(HKInfo, { props: {
				hkInfo: mockHkInfo,
				error: null
			} });

			expect(screen.getByText('issuer-no-endpoints')).toBeInTheDocument();
			// Should not crash and should render the issuer
		});
	});

	describe('WebSocket test component', () => {
		it('should lazy load WebSocketTest in dev mode', async () => {
			// Mock import.meta.env.DEV
			const originalDev = import.meta.env.DEV;
			import.meta.env.DEV = true;

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			// Wait for the lazy loading promise to resolve
			await waitFor(() => {
				expect(screen.getByText('Loading WebSocket Test...')).toBeInTheDocument();
			});

			// Restore original value
			import.meta.env.DEV = originalDev;
		});

		it('should not load WebSocketTest in production', () => {
			const originalDev = import.meta.env.DEV;
			import.meta.env.DEV = false;

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			expect(screen.queryByText('Loading WebSocket Test...')).not.toBeInTheDocument();

			import.meta.env.DEV = originalDev;
		});
	});

	describe('accessibility', () => {
		it('should have proper ARIA labels for loading states', () => {
			setLoadingState('hkInfoLoading', true);

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			const loadingElement = screen.getByLabelText('Loading HomeKey information');
			expect(loadingElement).toBeInTheDocument();
		});

		it('should have proper ARIA labels for system information', () => {
			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			const systemElement = screen.getByLabelText('System information');
			expect(systemElement).toBeInTheDocument();
		});
	});

	describe('error handling', () => {
		it('should handle WebSocketTest component loading errors', async () => {
			const originalDev = import.meta.env.DEV;
			import.meta.env.DEV = true;

			// Mock a failed import
			const mockImport = vi.fn().mockRejectedValue(new Error('Import failed'));
			vi.doMock('$lib/components/WebSocketTest.svelte', () => mockImport);

			render(HKInfo, { props: {
				hkInfo: null,
				error: null
			} });

			await waitFor(() => {
				expect(screen.getByText(/Failed to load WebSocket Test component/)).toBeInTheDocument();
			});

			import.meta.env.DEV = originalDev;
		});
	});
});