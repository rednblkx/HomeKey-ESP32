<script lang="ts">
	import {
		rebootDevice,
		resetPairings,
		resetWifi,
		saveConfig,
		startConfigAP
	} from "$lib/services/api";
	import {
		CertificateType,
		type EthConfig,
		type MiscConfig,
		type NfcGpioPinsPreset,
	} from "$lib/types/api";
	import { diff } from "$lib/utils/objDiff";
	import HardwareConfig from "$lib/components/HardwareConfig.svelte";
	import CertManager from "$lib/components/CertManager.svelte";

	let { misc, eth, nfcPresets, nfcConnected = $bindable(false), error } = $props();

	let activeTab = $state<'homekit' | 'hardware' | 'network' | 'security'>('homekit');

	// svelte-ignore state_referenced_locally
	let miscConfig = $state<MiscConfig>($state.snapshot(misc));
	// svelte-ignore state_referenced_locally
	let ethConfig = $state<EthConfig>($state.snapshot(eth));
	// svelte-ignore state_referenced_locally
	let nfcPresetsList = $state<NfcGpioPinsPreset>($state.snapshot(nfcPresets));

	const saveMiscConfig = async (e: any) => {
		e.preventDefault();
		try {
			if (!miscConfig || !misc) return;
			const result = await saveConfig("misc", diff(misc, miscConfig));
			if (result.success) {
				miscConfig = result.data;
				misc = result.data;
			}
		} catch (e) {
			const message = e instanceof Error ? e.message : String(e);
			error = message;
			alert(`Error saving config: ${message}`);
		}
	};

	const handleNfcPresetChange = (preset: number) => {
		miscConfig.nfcPinsPreset = preset;
		if (preset !== 255 && nfcPresets) {
			const presetData = nfcPresets.presets[preset];
			if (presetData) {
				miscConfig.nfcGpioPins = [
					presetData.gpioPins[0],
					presetData.gpioPins[1],
					presetData.gpioPins[2],
					presetData.gpioPins[3],
				];
			}
		} else if (preset === 255 && misc) {
			miscConfig.nfcGpioPins = misc.nfcGpioPins;
		}
	};

	const handleEthPresetChange = (preset: number) => {
		miscConfig.ethActivePreset = preset;
		if (preset !== 255 && ethConfig!.boardPresets) {
			const presetData = ethConfig!.boardPresets[preset];
			if (presetData) {
				miscConfig.ethPhyType = presetData.ethChip.phy_type;
				if (presetData.spi_conf) {
					miscConfig.ethSpiConfig = [
						presetData.spi_conf.spi_freq_mhz,
						presetData.spi_conf.pin_cs,
						presetData.spi_conf.pin_irq,
						presetData.spi_conf.pin_rst,
						presetData.spi_conf.pin_sck,
						presetData.spi_conf.pin_miso,
						presetData.spi_conf.pin_mosi,
					];
					miscConfig.ethRmiiConfig = misc?.ethRmiiConfig || [
						0, -1, -1, -1, 0,
					];
				}
				if (presetData.rmii_conf) {
					miscConfig.ethRmiiConfig = [
						presetData.rmii_conf.phy_addr,
						presetData.rmii_conf.pin_mcd,
						presetData.rmii_conf.pin_mdio,
						presetData.rmii_conf.pin_power,
						presetData.rmii_conf.pin_rmii_clock,
					];
					miscConfig.ethSpiConfig = misc?.ethSpiConfig || [
						20, -1, -1, -1, -1, -1, -1,
					];
				}
			}
		}
	};

	const resetForm = () => {
		if (misc) {
			miscConfig = misc;
		}
	};

	$effect(() => {
		if (misc?.ethActivePreset !== 255 && misc) {
			handleEthPresetChange(misc.ethActivePreset);
		}
	});
</script>

<div class="w-full py-6">
	<!-- Header -->
	<div class="mb-6">
		<h1 class="text-2xl font-bold text-base-content flex items-center gap-2">
			System Settings
			<div class="tooltip tooltip-bottom tooltip-info" data-tip="Device will reboot to apply changes!">
				<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-5 text-info">
				<path stroke-linecap="round" stroke-linejoin="round" d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z" />
				</svg>
			</div>
		</h1>
		<p class="text-base-content/60">
			Configure device settings, HomeKit options, and hardware pins.
		</p>
	</div>

	{#if !misc && error}
		<div class="text-center text-error">
			<p>Error: {error}</p>
		</div>
	{:else if misc}
		<form onsubmit={saveMiscConfig} class="space-y-4 max-w-4xl">
			<!-- Quick Actions -->
			<div class="card bg-base-200 shadow-xl">
				<div class="card-body p-4">
					<h2 class="text-lg font-semibold mb-1">Quick Actions</h2>
					<p class="text-sm text-base-content/60 mb-4">Device management and reset options</p>
					<div class="flex flex-wrap gap-2">
						<button
							type="button"
							class="btn btn-sm btn-outline"
							onclick={async () => {
								if (confirm("This will restart the device, please confirm.")) {
									await rebootDevice();
								}
							}}
						>
							<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 mr-1" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
								<path stroke-linecap="round" stroke-linejoin="round" d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
							</svg>
							Reboot
						</button>
						<button
							type="button"
							class="btn btn-sm btn-outline"
							onclick={async () => {
								if (confirm("This will start the configuration AP, please confirm.\n\nNote that the WiFi connection will be dropped.")) {
									await startConfigAP();
								}
							}}
						>
							<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="currentColor" class="size-4">
							<path fill-rule="evenodd" d="M14.188 7.063a8.75 8.75 0 0 0-12.374 0 .75.75 0 0 1-1.061-1.06c4.003-4.004 10.493-4.004 14.496 0a.75.75 0 1 1-1.061 1.06Zm-2.121 2.121a5.75 5.75 0 0 0-8.132 0 .75.75 0 0 1-1.06-1.06 7.25 7.25 0 0 1 10.252 0 .75.75 0 0 1-1.06 1.06Zm-2.122 2.122a2.75 2.75 0 0 0-3.889 0 .75.75 0 1 1-1.06-1.061 4.25 4.25 0 0 1 6.01 0 .75.75 0 0 1-1.06 1.06Zm-2.828 1.06a1.25 1.25 0 0 1 1.768 0 .75.75 0 0 1 0 1.06l-.355.355a.75.75 0 0 1-1.06 0l-.354-.354a.75.75 0 0 1 0-1.06Z" clip-rule="evenodd" />
							</svg>
							Start AP
						</button>
						<button
							type="button"
							class="btn btn-sm btn-outline btn-error"
							onclick={async () => {
								if (confirm("This will reset the HomeKit pairings, please confirm.")) {
									await resetPairings();
								}
							}}
						>
							<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 mr-1" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
								<path stroke-linecap="round" stroke-linejoin="round" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16" />
							</svg>
							Reset HomeKit
						</button>
						<button
							type="button"
							class="btn btn-sm btn-outline btn-error"
							onclick={async () => {
								if (confirm("This will reset the WiFi credentials, please confirm.")) {
									await resetWifi();
								}
							}}
						>
							<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="currentColor" class="size-4">
							<path fill-rule="evenodd" d="M14.188 7.063a8.75 8.75 0 0 0-12.374 0 .75.75 0 0 1-1.061-1.06c4.003-4.004 10.493-4.004 14.496 0a.75.75 0 1 1-1.061 1.06Zm-2.121 2.121a5.75 5.75 0 0 0-8.132 0 .75.75 0 0 1-1.06-1.06 7.25 7.25 0 0 1 10.252 0 .75.75 0 0 1-1.06 1.06Zm-2.122 2.122a2.75 2.75 0 0 0-3.889 0 .75.75 0 1 1-1.06-1.061 4.25 4.25 0 0 1 6.01 0 .75.75 0 0 1-1.06 1.06Zm-2.828 1.06a1.25 1.25 0 0 1 1.768 0 .75.75 0 0 1 0 1.06l-.355.355a.75.75 0 0 1-1.06 0l-.354-.354a.75.75 0 0 1 0-1.06Z" clip-rule="evenodd" />
							</svg>
							Reset WiFi
						</button>
					</div>
				</div>
			</div>

			<!-- Tabs -->
			<div class="flex bg-base-300 p-1 rounded-xl gap-1">
				<button
					type="button"
					class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'homekit' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
					onclick={() => activeTab = 'homekit'}
				>
					<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
						<path stroke-linecap="round" stroke-linejoin="round" d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 001 1m-6 0h6" />
					</svg>
					<span class="text-[10px] sm:text-xs mt-0.5">HomeKit</span>
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
				<button
					type="button"
					class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'security' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
					onclick={() => activeTab = 'security'}
				>
					<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
						<path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
					</svg>
					<span class="text-[10px] sm:text-xs mt-0.5">Security</span>
				</button>
			</div>

			<!-- Tab Content -->
			<div class="card bg-base-200 shadow-xl">
				<div class="card-body p-4">
					{#if activeTab === 'homekit'}
						<div class="space-y-4">
							<div>
								<h3 class="text-sm font-semibold">HomeKit Configuration</h3>
								<p class="text-xs text-base-content/60">Configure HomeKit device settings and behavior.</p>
							</div>

							<div class="grid grid-cols-1 md:grid-cols-2 gap-4">
								<div class="form-control">
									<label class="label">
										<span class="label-text text-xs">Device Name</span>
									</label>
									<input
										type="text"
										bind:value={miscConfig.deviceName}
										placeholder="HomeKey-Lock"
										class="input input-sm input-bordered w-full"
										required
									/>
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text text-xs">Setup Code</span>
									</label>
									<input
										type="text"
										bind:value={miscConfig.setupCode}
										placeholder="111-11-111"
										maxlength="8"
										class="input input-sm input-bordered w-full"
										required
										inputmode="numeric"
									/>
								</div>
							</div>

							<!-- Toggles -->
							<div class="space-y-2">
								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">Always Lock on HomeKey</p>
										<p class="text-xs text-base-content/60">Lock the device when HomeKey is tapped regardless of current state</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.lockAlwaysLock}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>
								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">Always Unlock on HomeKey</p>
										<p class="text-xs text-base-content/60">Unlock the device when HomeKey is tapped regardless of current state</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.lockAlwaysUnlock}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>
								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">SmartLock Battery Reporting</p>
										<p class="text-xs text-base-content/60">Enable battery level reporting to HomeKit</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.proxBatEnabled}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>
								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">Auth Precompute Cache</p>
										<p class="text-xs text-base-content/60">Enable faster HomeKey authentication (higher CPU/RAM usage)</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.hkAuthPrecomputeEnabled}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>
							</div>
							<!-- Card Finish -->
							<div class="p-2">
								<p class="text-sm font-medium mb-2">HomeKey Card Finish</p>
								<div class="grid grid-cols-4 gap-2">
									<button
										type="button"
										class="btn btn-sm border-1 {miscConfig.hk_key_color === 0 ? 'border-primary ring-1 ring-primary' : 'border-transparent'}"
										style="background-color: #ddd5cc; color: #3E2723;"
										onclick={() => miscConfig.hk_key_color = 0}
									>
										Tan
									</button>
									<button
										type="button"
										class="btn btn-sm border-1 {miscConfig.hk_key_color === 1 ? 'border-primary ring-1 ring-primary' : 'border-transparent'}"
										style="background-color: #e6d1a8; color: #3E2723;"
										onclick={() => miscConfig.hk_key_color = 1}
									>
										Gold
									</button>
									<button
										type="button"
										class="btn btn-sm border-1 {miscConfig.hk_key_color === 2 ? 'border-primary ring-1 ring-primary' : 'border-transparent'}"
										style="background-color: #cecece; color: #212121;"
										onclick={() => miscConfig.hk_key_color = 2}
									>
										Silver
									</button>
									<button
										type="button"
										class="btn btn-sm border-1 {miscConfig.hk_key_color === 3 ? 'border-primary ring-1 ring-primary' : 'border-transparent'}"
										style="background-color: #2a2a2a; color: #FFFFFF;"
										onclick={() => miscConfig.hk_key_color = 3}
									>
										Black
									</button>
								</div>
							</div>
						</div>
					{/if}

					{#if activeTab === 'hardware'}
						<div class="space-y-4">
							<div>
								<h3 class="text-sm font-semibold">Hardware Configuration</h3>
								<p class="text-xs text-base-content/60">Configure GPIO pins for PN532 NFC reader and optional Ethernet connectivity.</p>
							</div>

							<HardwareConfig
								nfcGpioPins={miscConfig.nfcGpioPins}
								nfcPinsPreset={miscConfig.nfcPinsPreset}
								nfcPresets={nfcPresetsList}
								ethernetEnabled={miscConfig.ethernetEnabled}
								ethActivePreset={miscConfig.ethActivePreset}
								ethPhyType={miscConfig.ethPhyType}
								ethSpiBus={miscConfig.ethSpiBus}
								ethRmiiConfig={miscConfig.ethRmiiConfig}
								ethSpiConfig={miscConfig.ethSpiConfig}
								ethConfig={ethConfig}
								nfcConnected={nfcConnected}
								onNfcPresetChange={handleNfcPresetChange}
								onEthPresetChange={handleEthPresetChange}
								onNfcPinsChange={(pins) => miscConfig.nfcGpioPins = pins}
								onEthernetToggle={(enabled) => miscConfig.ethernetEnabled = enabled}
								onEthPhyTypeChange={(phyType) => miscConfig.ethPhyType = phyType}
								onEthSpiBusChange={(bus) => miscConfig.ethSpiBus = bus}
								onEthRmiiConfigChange={(cfg) => miscConfig.ethRmiiConfig = cfg}
								onEthSpiConfigChange={(cfg) => miscConfig.ethSpiConfig = cfg}
							/>

							<!-- HomeSpan -->
							<div class="py-2 px-3 bg-base-100 rounded-lg">
								<p class="text-sm font-medium mb-2">HomeSpan - <a class="text-xs text-base-content/60 underline mb-2" href="https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#device-configuration-mode">User Guide</a></p>
								<div class="form-control mb-2">
									<label class="label">
										<span class="label-text text-xs">OTA Password</span>
									</label>
									<input
										type="password"
										bind:value={miscConfig.otaPasswd}
										class="input input-sm input-bordered w-full"
									/>
								</div>
								<div class="grid grid-cols-2 gap-2">
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">Control GPIO Pin</span>
										</label>
										<input
											type="number"
											bind:value={miscConfig.controlPin}
											class="input input-sm input-bordered w-full"
										/>
									</div>
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">Status LED GPIO Pin</span>
										</label>
										<input
											type="number"
											bind:value={miscConfig.hsStatusPin}
											class="input input-sm input-bordered w-full"
										/>
									</div>
								</div>
							</div>
						</div>
					{/if}

					{#if activeTab === 'security'}
						<div class="space-y-6">
							<!-- Web Authentication Section -->
							<div class="space-y-4">
								<div>
									<h3 class="text-sm font-semibold">Web Authentication</h3>
									<p class="text-xs text-base-content/60">Configure web interface authentication settings.</p>
								</div>

								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">Enable Web Authentication</p>
										<p class="text-xs text-base-content/60">Require login to access the web interface</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.webAuthEnabled}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>

								{#if miscConfig.webAuthEnabled}
									<div class="grid grid-cols-2 gap-4">
										<div class="form-control">
											<label class="label">
												<span class="label-text text-xs">Username</span>
											</label>
											<input
												type="text"
												bind:value={miscConfig.webUsername}
												placeholder="admin"
												class="input input-sm input-bordered w-full"
											/>
										</div>
										<div class="form-control">
											<label class="label">
												<span class="label-text text-xs">Password</span>
											</label>
											<input
												type="password"
												bind:value={miscConfig.webPassword}
												placeholder="••••••••"
												class="input input-sm input-bordered w-full"
											/>
										</div>
									</div>
								{/if}
							</div>

							<!-- HTTPS Section -->
							<div class="space-y-4 border-t border-base-300 pt-4">
								<div>
									<h3 class="text-sm font-semibold">HTTPS Encryption</h3>
									<p class="text-xs text-base-content/60">Secure the web interface with SSL/TLS encryption.</p>
								</div>

								<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
									<div>
										<p class="text-sm font-medium">Enable HTTPS</p>
										<p class="text-xs text-base-content/60">Access the web interface securely via HTTPS (Port 443)</p>
									</div>
									<input
										type="checkbox"
										bind:checked={miscConfig.webHttpsEnabled}
										class="toggle toggle-primary toggle-sm"
									/>
								</div>

								{#if miscConfig.webHttpsEnabled}
									<div class="flex items-center justify-between py-2 px-3 bg-warning/10 rounded-lg">
										<div class="flex items-start gap-2">
											<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-warning mt-0.5 flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
												<path stroke-linecap="round" stroke-linejoin="round" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
											</svg>
											<div>
												<p class="text-sm font-medium text-warning">Device Reboot Required</p>
												<p class="text-xs text-base-content/60">HTTPS settings require a device reboot to take effect.</p>
											</div>
										</div>
									</div>
									<CertManager
										certType={CertificateType.HTTPS_SERVER_CERT}
										keyType={CertificateType.HTTPS_PRIVATE_KEY}
										caType={CertificateType.HTTPS_CA_CERT}
										mode="https"
									/>
								{/if}
							</div>
						</div>
					{/if}
				</div>
			</div>

			<!-- Actions -->
			<div class="flex gap-2">
				<button type="submit" class="btn btn-primary">Save & Apply changes</button>
				<button type="button" class="btn btn-outline" onclick={resetForm}>Reset</button>
			</div>
		</form>
	{/if}
</div>
