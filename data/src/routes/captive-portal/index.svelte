<script lang="ts">
	import { route } from 'sv-router/generated';
	import { saveCaptivePortalConfig, rebootDevice, scanWiFi } from '$lib/services/api.js';
	import type { CaptivePortalConfig, WiFiNetwork, EthConfig, NfcGpioPinsPreset } from '$lib/types/api';
    import HardwareConfig from '$lib/components/HardwareConfig.svelte';

	const colorOptions = [
		{ value: 0, label: 'Tan', class: 'bg-[#ddd5cc]' },
		{ value: 1, label: 'Gold', class: 'bg-[#e6d1a8]' },
		{ value: 2, label: 'Silver', class: 'bg-[#cecece]' },
		{ value: 3, label: 'Black', class: 'bg-[#2a2a2a]' },
	];

	let config = $state<CaptivePortalConfig>({
		wifiSsid: '',
		wifiPassword: '',
		setupCode: '46637726',
		hk_key_color: 0,
		nfcPinsPreset: 0,
		nfcGpioPins: [5, 18, 19, 23],
		ethernetEnabled: false,
		ethActivePreset: 255,
		ethPhyType: 0,
		ethSpiBus: 1,
		ethRmiiConfig: [0, -1, -1, -1, 0],
		ethSpiConfig: [20, -1, -1, -1, -1, -1, -1],
	});
	let loading = $state(false);
	let saved = $state(false);
	let error = $state<string | null>(null);
	let networks = $state<WiFiNetwork[]>([]);
	let scanning = $state(false);
	let showNetworkList = $state(false);
	let activeTab = $state<'wifi' | 'hardware' | 'network'>('wifi');

	// Derived values for NFC/Ethernet presets
	let nfcPresets : NfcGpioPinsPreset = $derived(route.meta.captivePortalData?.nfcPresets ?? { presets: [] });
	let ethConfig : EthConfig = $derived(route.meta.captivePortalData?.ethConfig ?? { boardPresets: [], supportedChips: [], numSpiBuses: 1, ethEnabled: false });


	$effect(() => {
		if (route.meta.captivePortalData?.config) {
			config = { ...route.meta.captivePortalData.config };
		}
		if (route.meta.captivePortalData?.error) {
			error = route.meta.captivePortalData.error;
		}
	});

	function validateSetupCode(code: string): boolean {
		return /^\d{8}$/.test(code);
	}

	function validateForm(): string | null {
		// WiFi is only required if Ethernet is not enabled
		if (!config.ethernetEnabled) {
			if (!config.wifiSsid || config.wifiSsid.length === 0) {
				return 'WiFi SSID is required (or enable Ethernet)';
			}
			if (config.wifiSsid.length > 32) {
				return 'WiFi SSID must be 32 characters or less';
			}
			if (config.wifiPassword.length > 64) {
				return 'WiFi password must be 64 characters or less';
			}
		} else {
			// If WiFi is provided even with Ethernet, validate it
			if (config.wifiSsid && config.wifiSsid.length > 0) {
				if (config.wifiSsid.length > 32) {
					return 'WiFi SSID must be 32 characters or less';
				}
				if (config.wifiPassword.length > 64) {
					return 'WiFi password must be 64 characters or less';
				}
			}
		}
		if (!validateSetupCode(config.setupCode)) {
			return 'HomeKit Setup Code must be exactly 8 digits';
		}
		return null;
	}

	function handleNfcPresetChange(preset: number) {
		config.nfcPinsPreset = preset;
		if (preset !== 255 && nfcPresets) {
			const presetData = nfcPresets.presets[preset];
			if (presetData) {
				config.nfcGpioPins = [presetData.gpioPins[0], presetData.gpioPins[1], presetData.gpioPins[2], presetData.gpioPins[3]];
			}
		}
	}

	function handleEthPresetChange(preset: number) {
		config.ethActivePreset = preset;
		if (preset !== 255 && ethConfig?.boardPresets) {
			const presetData = ethConfig.boardPresets[preset];
			if (presetData) {
				config.ethPhyType = presetData.ethChip.phy_type;
				if (presetData.spi_conf) {
					config.ethSpiConfig = [
						presetData.spi_conf.spi_freq_mhz,
						presetData.spi_conf.pin_cs,
						presetData.spi_conf.pin_irq,
						presetData.spi_conf.pin_rst,
						presetData.spi_conf.pin_sck,
						presetData.spi_conf.pin_miso,
						presetData.spi_conf.pin_mosi,
					];
					config.ethRmiiConfig = config.ethRmiiConfig || [0, -1, -1, -1, 0];
				}
				if (presetData.rmii_conf) {
					config.ethRmiiConfig = [
						presetData.rmii_conf.phy_addr,
						presetData.rmii_conf.pin_mcd,
						presetData.rmii_conf.pin_mdio,
						presetData.rmii_conf.pin_power,
						presetData.rmii_conf.pin_rmii_clock,
					];
					config.ethSpiConfig = config.ethSpiConfig || [20, -1, -1, -1, -1, -1, -1];
				}
			}
		}
	}

	async function handleScan() {
		scanning = true;
		error = null;
		try {
			const result = await scanWiFi();
			if (result.success) {
				networks = result.data ?? [];
				showNetworkList = true;
			} else {
				error = 'error' in result ? result.error : 'Failed to scan WiFi networks';
			}
		} catch (err) {
			error = err instanceof Error ? err.message : 'Failed to scan WiFi networks';
		} finally {
			scanning = false;
		}
	}

	function selectNetwork(ssid: string) {
		config.wifiSsid = ssid;
		showNetworkList = false;
	}

	async function handleSubmit(e: Event) {
		e.preventDefault();
		error = null;

		const validationError = validateForm();
		if (validationError) {
			error = validationError;
			return;
		}

		loading = true;
		try {
			const result = await saveCaptivePortalConfig(config);
			if (result.success) {
				saved = true;
				// Trigger reboot after short delay
				setTimeout(async () => {
					await rebootDevice();
				}, 1000);
			} else {
				error = result.error || 'Failed to save configuration';
			}
		} catch (err) {
			error = err instanceof Error ? err.message : 'An unexpected error occurred';
		} finally {
			loading = false;
		}
	}
</script>

<div class="card bg-base-200 shadow-xl border border-base-300">
	<div class="card-body">
		{#if saved}
		<div class="alert alert-success mb-4">
			<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 shrink-0 stroke-current" fill="none" viewBox="0 0 24 24">
				<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
			</svg>
			<div>
				<span class="font-bold">WiFi connection successful!</span>
				<div class="text-sm">Configuration saved. The device is now rebooting and will connect to your network...</div>
			</div>
		</div>
		{:else}
			<h2 class="card-title text-2xl justify-center mb-2">Configure Your Device</h2>
			<p class="text-base-content/70 text-center mb-6">
				Enter your WiFi credentials and device settings to get started.
			</p>
			<form onsubmit={handleSubmit} class="space-y-4">
				<!-- Tabs -->
				<div class="flex bg-base-300 p-1 rounded-xl gap-1 mb-4">
					<button
						type="button"
						class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'wifi' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
						onclick={() => activeTab = 'wifi'}
					>
						<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<path stroke-linecap="round" stroke-linejoin="round" d="M8.111 16.404a5.5 5.5 0 017.778 0M12 20h.01m-7.08-7.071c3.904-3.905 10.236-3.905 14.141 0M1.394 9.393c5.857-5.857 15.355-5.857 21.213 0" />
						</svg>
						<span class="text-[10px] sm:text-xs mt-0.5">WiFi & HomeKit</span>
					</button>
					<button
						type="button"
						class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'hardware' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
						onclick={() => activeTab = 'hardware'}
					>
						<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<rect x="7" y="7" width="10" height="10" rx="1"/>
							<path d="M9 7V5" stroke-linecap="round"/>
							<path d="M12 7V5" stroke-linecap="round"/>
							<path d="M15 7V5" stroke-linecap="round"/>
							<path d="M9 19v-2" stroke-linecap="round"/>
							<path d="M12 19v-2" stroke-linecap="round"/>
							<path d="M15 19v-2" stroke-linecap="round"/>
							<path d="M7 9H5" stroke-linecap="round"/>
							<path d="M7 12H5" stroke-linecap="round"/>
							<path d="M7 15H5" stroke-linecap="round"/>
							<path d="M19 9h-2" stroke-linecap="round"/>
							<path d="M19 12h-2" stroke-linecap="round"/>
							<path d="M19 15h-2" stroke-linecap="round"/>
						</svg>
						<span class="text-[10px] sm:text-xs mt-0.5">Hardware</span>
					</button>
				</div>

				<!-- WiFi Tab -->
				{#if activeTab === 'wifi'}
				<div class="space-y-4">
					<!-- WiFi SSID -->
					<div class="form-control">
						<label class="label" for="wifiSsid">
							<span class="label-text font-medium">WiFi Network Name (SSID)</span>
						</label>
						<div class="flex gap-2">
							<input
								type="text"
								id="wifiSsid"
								bind:value={config.wifiSsid}
								placeholder="Enter or select your WiFi name"
								class="input input-bordered flex-1"
								maxlength="32"
								disabled={loading}
							/>
							<button
								type="button"
								class="btn btn-outline btn-square"
								onclick={handleScan}
								disabled={scanning || loading}
								title="Scan for networks"
							>
								{#if scanning}
									<span class="loading loading-spinner loading-sm"></span>
								{:else}
									<svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
										<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z" />
									</svg>
								{/if}
							</button>
						</div>
						{#if showNetworkList && networks.length > 0}
							<div class="mt-2 border border-base-300 rounded-lg overflow-hidden max-h-48 overflow-y-auto bg-base-100">
								<div class="px-3 py-2 bg-base-200 text-xs font-medium text-base-content/70 border-b border-base-300">
									Found {networks.length} network{networks.length === 1 ? '' : 's'} - Click to select
								</div>
								{#each networks as network (network.ssid)}
									<button
										type="button"
										class="w-full text-left px-3 py-2 hover:bg-base-200 flex items-center justify-between gap-2"
										onclick={() => selectNetwork(network.ssid)}
									>
										<div class="flex items-center gap-2 min-w-0">
											<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor">
												<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M8.111 16.404a5.5 5.5 0 017.778 0M12 20h.01m-7.08-7.071c3.904-3.905 10.236-3.905 14.141 0M1.394 9.393c5.857-5.857 15.355-5.857 21.213 0" />
											</svg>
											<span class="truncate">{network.ssid}</span>
											{#if network.auth === 'OPEN'}
												<span class="badge badge-sm badge-ghost">Open</span>
											{/if}
										</div>
										<div class="flex items-center gap-1 shrink-0 text-xs text-base-content/60">
											{#if network.rssi > -50}
												<span class="text-success">●●●●</span>
											{:else if network.rssi > -60}
												<span class="text-success">●●●</span><span class="text-base-300">●</span>
											{:else if network.rssi > -70}
												<span class="text-warning">●●</span><span class="text-base-300">●●</span>
											{:else}
												<span class="text-error">●</span><span class="text-base-300">●●●</span>
											{/if}
											<span class="text-xs">{network.rssi}dBm</span>
										</div>
									</button>
								{/each}
								<button
									type="button"
									class="w-full text-center px-3 py-2 text-sm text-base-content/60 hover:bg-base-200 border-t border-base-300"
									onclick={() => showNetworkList = false}
								>
									Close list
								</button>
							</div>
						{:else if showNetworkList && networks.length === 0}
							<div class="mt-2 alert alert-warning alert-sm">
								<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0 stroke-current" fill="none" viewBox="0 0 24 24">
									<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
								</svg>
								<span>No networks found</span>
							</div>
						{/if}
					</div>

					<!-- WiFi Password -->
					<div class="form-control">
						<label class="label" for="wifiPassword">
							<span class="label-text font-medium">WiFi Password</span>
						</label>
						<input
							type="password"
							id="wifiPassword"
							bind:value={config.wifiPassword}
							placeholder="Enter your WiFi password"
							class="input input-bordered w-full"
							maxlength="64"
							disabled={loading}
						/>
					</div>

					<!-- HomeKit Setup Code -->
					<div class="form-control">
						<label class="label" for="setupCode">
							<span class="label-text font-medium">HomeKit Setup Code</span>
						</label>
						<input
							type="text"
							id="setupCode"
							bind:value={config.setupCode}
							placeholder="8-digit code"
							class="input input-bordered w-full font-mono tracking-wider"
							maxlength="8"
							disabled={loading}
						/>
						<div class="label">
							<span class="label-text-alt text-base-content/60">Exactly 8 digits required</span>
						</div>
					</div>

					<!-- HomeKey Color -->
					<div class="form-control">
						<label class="label" for="hkColor">
							<span class="label-text font-medium">HomeKey Card Color</span>
						</label>
						<input type="hidden" id="hkColor" value={config.hk_key_color} />
						<div class="grid grid-cols-2 gap-2">
							{#each colorOptions as color}
								<button
									type="button"
									class="btn btn-outline {config.hk_key_color === color.value ? 'btn-primary' : ''}"
									onclick={() => config.hk_key_color = color.value}
									disabled={loading}
								>
									<span class="w-4 h-4 rounded-full {color.class} inline-block mr-2 border border-base-300"></span>
									{color.label}
								</button>
							{/each}
						</div>
					</div>
				</div>
				{/if}

				<!-- Hardware Tab -->
				{#if activeTab === 'hardware'}
					<div class="space-y-4">
						<div>
							<h3 class="text-sm font-semibold">Hardware Configuration</h3>
							<p class="text-xs text-base-content/60">Configure GPIO pins for PN532 NFC reader and optional Ethernet connectivity.</p>
						</div>

						<HardwareConfig
							nfcGpioPins={config.nfcGpioPins}
							nfcPinsPreset={config.nfcPinsPreset}
							nfcPresets={nfcPresets}
							ethernetEnabled={config.ethernetEnabled}
							ethActivePreset={config.ethActivePreset}
							ethPhyType={config.ethPhyType}
							ethSpiBus={config.ethSpiBus}
							ethRmiiConfig={config.ethRmiiConfig}
							ethSpiConfig={config.ethSpiConfig}
							ethConfig={ethConfig}
							loading={loading}
							onNfcPresetChange={handleNfcPresetChange}
							onEthPresetChange={handleEthPresetChange}
							onNfcPinsChange={(pins) => config.nfcGpioPins = pins}
							onEthernetToggle={(enabled) => config.ethernetEnabled = enabled}
							onEthPhyTypeChange={(phyType) => config.ethPhyType = phyType}
							onEthSpiBusChange={(bus) => config.ethSpiBus = bus}
							onEthRmiiConfigChange={(cfg) => config.ethRmiiConfig = cfg}
							onEthSpiConfigChange={(cfg) => config.ethSpiConfig = cfg}
						/>
					</div>
				{/if}

			<!-- Error Message -->
				{#if error}
					<div class="alert alert-error">
						<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 shrink-0 stroke-current" fill="none" viewBox="0 0 24 24">
							<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 14l2-2m0 0l2-2m-2 2l-2-2m2 2l2 2m7-2a9 9 0 11-18 0 9 9 0 0118 0z" />
						</svg>
						<span>{error}</span>
					</div>
				{/if}

				<!-- Submit Button -->
				<button
					type="submit"
					class="btn btn-primary w-full"
					disabled={loading || saved}
				>
					{#if loading}
						<span class="loading loading-spinner loading-sm"></span>
						Attempting connection...
					{:else if saved}
						<svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5" viewBox="0 0 20 20" fill="currentColor">
							<path fill-rule="evenodd" d="M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z" clip-rule="evenodd" />
						</svg>
						Connected! Rebooting...
					{:else}
						Connect & Save
					{/if}
				</button>
			</form>
		{/if}
	</div>
</div>
