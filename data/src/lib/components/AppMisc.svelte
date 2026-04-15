<script lang="ts">
	import {
		rebootDevice,
		resetPairings,
		resetWifi,
		saveConfig,
		startConfigAP,
	} from "$lib/services/api.js";
	import type {
		EthConfig,
		MiscConfig,
		NfcGpioPinsPreset,
	} from "$lib/types/api";
	import { diff } from "$lib/utils/objDiff";
	import SpiEthernetNote from "$lib/components/SpiEthernetNote.svelte";

	let { misc, eth, nfcPresets, nfcConnected = $bindable(false), error } = $props();

	let activeTab = $state<'homekit' | 'hardware' | 'network' | 'auth'>('homekit');

	// svelte-ignore state_referenced_locally
	let miscConfig = $state<MiscConfig>($state.snapshot(misc));
	// svelte-ignore state_referenced_locally
	let ethConfig = $state<EthConfig>($state.snapshot(eth));
	// svelte-ignore state_referenced_locally
	let nfcPresetsList = $state<NfcGpioPinsPreset>($state.snapshot(nfcPresets));

	let currentEthChip = $derived(() => {
		if (miscConfig.ethPhyType) {
			return ethConfig!.supportedChips?.[miscConfig.ethPhyType];
		}
		return null;
	});

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

	const handleNfcPresetChange = () => {
		if (
			miscConfig.nfcPinsPreset !== undefined &&
			miscConfig.nfcPinsPreset !== 255 &&
			nfcPresets
		) {
			const preset = nfcPresets.presets[miscConfig.nfcPinsPreset];
			if (preset) {
				miscConfig.nfcGpioPins = [
					preset.gpioPins[0],
					preset.gpioPins[1],
					preset.gpioPins[2],
					preset.gpioPins[3],
				];
			}
		} else if (miscConfig.nfcPinsPreset === 255) {
			miscConfig.nfcGpioPins = misc.nfcGpioPins;
		}
	};

	const handleEthPresetChange = () => {
		if (
			miscConfig.ethActivePreset !== undefined &&
			miscConfig.ethActivePreset !== 255 &&
			ethConfig!.boardPresets
		) {
			const preset = ethConfig!.boardPresets[miscConfig.ethActivePreset];
			if (preset) {
				miscConfig.ethPhyType = preset.ethChip.phy_type;
				if (preset.spi_conf) {
					miscConfig.ethSpiConfig = [
						preset.spi_conf.spi_freq_mhz,
						preset.spi_conf.pin_cs,
						preset.spi_conf.pin_irq,
						preset.spi_conf.pin_rst,
						preset.spi_conf.pin_sck,
						preset.spi_conf.pin_miso,
						preset.spi_conf.pin_mosi,
					];
					miscConfig.ethRmiiConfig = misc?.ethRmiiConfig || [
						0, -1, -1, -1, 0,
					];
				}
				if (preset.rmii_conf) {
					miscConfig.ethRmiiConfig = [
						preset.rmii_conf.phy_addr,
						preset.rmii_conf.pin_mcd,
						preset.rmii_conf.pin_mdio,
						preset.rmii_conf.pin_power,
						preset.rmii_conf.pin_rmii_clock,
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
		if (misc?.ethActivePreset !== 255) {
			handleEthPresetChange();
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
					class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'network' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
					onclick={() => activeTab = 'network'}
				>
          <svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="currentColor" class="size-4">
            <path d="M11.0001 14V11H13.0001V14H18.0001C18.5524 14 19.0001 14.4477 19.0001 15V21C19.0001 21.5523 18.5524 22 18.0001 22H6.00015C5.44786 22 5.00015 21.5523 5.00015 21V15C5.00015 14.4477 5.44786 14 6.00015 14H11.0001ZM2.51074 8.83686C3.83432 4.86424 7.58275 2 12.0001 2C16.4176 2 20.166 4.86424 21.4896 8.83686L19.5917 9.46949C18.5328 6.29139 15.5341 4 12.0001 4C8.46623 4 5.46749 6.29139 4.40862 9.46949L2.51074 8.83686ZM6.3065 10.1021C7.10065 7.71854 9.34971 6 12.0001 6C14.6506 6 16.8996 7.71854 17.6938 10.1021L15.7959 10.7347C15.2665 9.1457 13.7671 8 12.0001 8C10.2332 8 8.73382 9.1457 8.20439 10.7347L6.3065 10.1021ZM7.00015 16V20H17.0001V16H7.00015Z">
            </path>
          </svg>
					<span class="text-[10px] sm:text-xs mt-0.5">
            Network
          </span>
				</button>
				<button
					type="button"
					class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'auth' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
					onclick={() => activeTab = 'auth'}
				>
					<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
						<path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
					</svg>
					<span class="text-[10px] sm:text-xs mt-0.5">Auth</span>
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
								<p class="text-xs text-base-content/60">Configure GPIO pins for PN532 NFC reader and HomeSpan.</p>
							</div>

							<!-- PN532 -->
							<div class="py-2 px-3 bg-base-100 rounded-lg">
								<div class="flex items-center justify-between mb-2">
									<p class="text-sm font-medium">PN532 NFC Reader</p>
									<div class="flex items-center gap-2">
										<span class="relative flex h-2.5 w-2.5">
											{#if nfcConnected}
												<span class="animate-ping absolute inline-flex h-full w-full rounded-full bg-success opacity-75"></span>
												<span class="relative inline-flex rounded-full h-2.5 w-2.5 bg-success"></span>
											{:else}
												<span class="relative inline-flex rounded-full h-2.5 w-2.5 bg-error"></span>
											{/if}
										</span>
										<span class="text-xs font-medium {nfcConnected ? 'text-success' : 'text-error'}">
											{nfcConnected ? 'Connected' : 'Disconnected'}
										</span>
									</div>
								</div>
								{#if miscConfig.ethernetEnabled}
									<SpiEthernetNote spiNumBuses={ethConfig.numSpiBuses} selectedBus={miscConfig.ethSpiBus}/>
								{/if}
								<div class="form-control mb-2">
									<label class="label">
										<span class="label-text text-xs">Preset</span>
									</label>
									<select
										bind:value={miscConfig.nfcPinsPreset}
										onchange={handleNfcPresetChange}
										class="select select-sm select-bordered w-full"
									>
										{#each nfcPresetsList.presets as preset, i}
											<option value={i}>{preset.name}</option>
										{/each}
										<option value={255}>Custom</option>
									</select>
								</div>
								<div class="grid grid-cols-4 gap-2">
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">SS Pin</span>
										</label>
										<input
											type="number"
											disabled={miscConfig.nfcPinsPreset !== 255}
											bind:value={miscConfig.nfcGpioPins![0]}
											class="input input-sm input-bordered w-full"
										/>
									</div>
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">SCK Pin</span>
										</label>
										<input
											type="number"
											disabled={miscConfig.nfcPinsPreset !== 255}
											bind:value={miscConfig.nfcGpioPins![1]}
											class="input input-sm input-bordered w-full"
										/>
									</div>
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">MISO Pin</span>
										</label>
										<input
											type="number"
											disabled={miscConfig.nfcPinsPreset !== 255}
											bind:value={miscConfig.nfcGpioPins![2]}
											class="input input-sm input-bordered w-full"
										/>
									</div>
									<div class="form-control">
										<label class="label">
											<span class="label-text text-xs">MOSI Pin</span>
										</label>
										<input
											type="number"
											disabled={miscConfig.nfcPinsPreset !== 255}
											bind:value={miscConfig.nfcGpioPins![3]}
											class="input input-sm input-bordered w-full"
										/>
									</div>
								</div>
							</div>

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

					{#if activeTab === 'network'}
						<div class="space-y-4">
							<div>
								<h3 class="text-sm font-semibold">Network Configuration</h3>
								<p class="text-xs text-base-content/60">Configure Ethernet connectivity options.</p>
							</div>

							<div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
								<div>
									<p class="text-sm font-medium">Enable Ethernet</p>
									<p class="text-xs text-base-content/60">Use wired Ethernet instead of WiFi</p>
								</div>
								<input
									type="checkbox"
									bind:checked={miscConfig.ethernetEnabled}
									class="toggle toggle-primary toggle-sm"
								/>
							</div>

							{#if miscConfig.ethernetEnabled}
								<SpiEthernetNote spiNumBuses={ethConfig.numSpiBuses} selectedBus={miscConfig.ethSpiBus}/>
								<div class="form-control">
									<label class="label">
										<span class="label-text text-xs">Board Preset</span>
									</label>
									<select
										bind:value={miscConfig.ethActivePreset}
										onchange={handleEthPresetChange}
										class="select select-sm select-bordered w-full"
									>
										{#each ethConfig!.boardPresets || [] as preset, i}
											<option value={i}>{preset.name}</option>
										{/each}
										<option value={255}>Custom</option>
									</select>
								</div>

								<div class="form-control">
									<label class="label">
										<span class="label-text text-xs">PHY Type</span>
									</label>
									<select
										bind:value={miscConfig.ethPhyType}
										disabled={miscConfig.ethActivePreset !== 255}
										class="select select-sm select-bordered w-full"
									>
										{#each ethConfig!.supportedChips as chip}
											<option value={chip.phy_type}>{chip.name}</option>
										{/each}
									</select>
								</div>

								{#if currentEthChip()?.emac}
									<div class="py-2 px-3 bg-base-100 rounded-lg">
										<p class="text-sm font-medium mb-2">RMII Configuration</p>
										<div class="grid grid-cols-2 md:grid-cols-3 gap-2">
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">PHY Address</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethRmiiConfig![0]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">Pin MDC</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethRmiiConfig![1]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">Pin MDIO</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethRmiiConfig![2]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">Pin Power</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethRmiiConfig![3]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">RMII Clock</span>
												</label>
												<select
													bind:value={miscConfig.ethRmiiConfig![4]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="select select-sm select-bordered w-full"
												>
													<option value={0}>GPIO0_IN</option>
													<option value={1}>GPIO0_OUT</option>
													<option value={2}>GPIO16_OUT</option>
													<option value={3}>GPIO17_OUT</option>
												</select>
											</div>
										</div>
									</div>
								{:else}
									<div class="py-2 px-3 bg-base-100 rounded-lg">
										<p class="text-sm font-medium mb-2">SPI Configuration</p>
										<div class="grid grid-cols-4 gap-2">
											<div class="form-control">
												<!-- svelte-ignore a11y_label_has_associated_control -->
												<label class="label">
													<span class="label-text">SPI Bus</span>
												</label>
												<select
														bind:value={miscConfig.ethSpiBus}
														disabled={miscConfig.ethActivePreset !== 255 || ethConfig.numSpiBuses === 1}
														class="select select-bordered w-full">
														<option value={1}>SPI2</option>
													{#if ethConfig.numSpiBuses === 2}
														<option value={2}>SPI3</option>
													{/if}
												</select>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">Freq (MHz)</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethSpiConfig![0]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">CS Pin</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethSpiConfig![1]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">IRQ Pin</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethSpiConfig![2]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
											<div class="form-control">
												<label class="label">
													<span class="label-text text-xs">RST Pin</span>
												</label>
												<input
													type="number"
													bind:value={miscConfig.ethSpiConfig![3]}
													disabled={miscConfig.ethActivePreset !== 255}
													class="input input-sm input-bordered w-full"
												/>
											</div>
										</div>
									</div>
								{/if}
							{/if}
						</div>
					{/if}

					{#if activeTab === 'auth'}
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
