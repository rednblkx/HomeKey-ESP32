<script lang="ts">
	import { saveConfig } from '$lib/services/api.js';
	import HKFinish0 from '$lib/assets/hk-finish-0.webp?inline';
	import HKFinish1 from '$lib/assets/hk-finish-1.webp?inline';
	import HKFinish2 from '$lib/assets/hk-finish-2.webp?inline';
	import HKFinish3 from '$lib/assets/hk-finish-3.webp?inline';
    import type { EthConfig, MiscConfig } from '$lib/types/api';

	let { misc, eth, error } : { misc: MiscConfig | null; eth: EthConfig | null; error?: string | null } = $props();

	let miscConfig = $state<MiscConfig>(misc ?? {
		deviceName: "",
		otaPasswd: "",
		hk_key_color: 0,
		setupCode: "",
		lockAlwaysUnlock: false,
		lockAlwaysLock: false,
		proxBatEnabled: false,
		btrLowStatusThreshold: 10,
		nfcNeopixelPin: 8,
		neoPixelType: 0,
		neopixelSuccessColor: [[0, 0], [1, 255], [2, 0]],
		neopixelFailureColor: [[0, 255], [1, 0], [2, 0]], 
		neopixelSuccessTime: 1000,
		neopixelFailTime: 1000,
		nfcSuccessPin: 2,
		nfcSuccessTime: 1000,
		nfcSuccessHL: false,
		nfcFailPin: 2,
		nfcFailTime: 1000,
		nfcFailHL: false,
		gpioActionPin: 2,
		gpioActionLockState: false,
		gpioActionUnlockState: false,
		gpioActionMomentaryEnabled: false,
		hkGpioControlledState: false,
		gpioActionMomentaryTimeout: 5000,
		webAuthEnabled: false,
		webUsername: "",
		webPassword: "",
		nfcGpioPins: [5, 18, 19, 23],
		hkDumbSwitchMode: false,
		hkAltActionInitPin: 255,
		hkAltActionInitLedPin: 255,
		hkAltActionInitTimeout: 5000,
		hkAltActionPin: 255,
		hkAltActionTimeout: 5000,
		hkAltActionGpioState: 0,
		ethernetEnabled: false,
		ethActivePreset: 255,
		ethPhyType: 0,
		ethRmiiConfig: [0, 0, 0, 0, 0],
		ethSpiConfig: [20, -1, -1, -1, -1, -1, -1],
		controlPin: 26,
		hsStatusPin: 2
	});
	let ethConfig = $state<EthConfig>(eth ?? {
		supportedChips: [],
		boardPresets: [],
		ethEnabled: false
	});

	const hkFinishColors = [
		{ name: 'Tan', value: 0, image: HKFinish0 },
		{ name: 'Gold', value: 1, image: HKFinish1 },
		{ name: 'Silver', value: 2, image: HKFinish2 },
		{ name: 'Black', value: 3, image: HKFinish3 },
	];

	let hkFinishImage = $derived(() => {
		const color = hkFinishColors.find(c => c.value === miscConfig.hk_key_color);
		return color?.image || HKFinish0;
	});

	let currentEthChip = $derived(() => {
		if (miscConfig.ethPhyType) {
			return ethConfig!.supportedChips?.[miscConfig.ethPhyType];
		}
		return null;
	});

	const saveMiscConfig = async () => {
		try {
			if (!miscConfig) return;
			const result = await saveConfig('misc', miscConfig);
		} catch (e) {
			const message = e instanceof Error ? e.message : String(e);
			error = message;
			alert(`Error saving config: ${message}`);
		}
	};

	const handleEthPresetChange = () => {
		if (miscConfig.ethActivePreset !== undefined && miscConfig.ethActivePreset !== 255 && ethConfig!.boardPresets) {
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
					miscConfig.ethRmiiConfig = misc?.ethRmiiConfig || [0, -1, -1, -1, 0];
				}
				if (preset.rmii_conf) {
					miscConfig.ethRmiiConfig = [
						preset.rmii_conf.phy_addr,
						preset.rmii_conf.pin_mcd,
						preset.rmii_conf.pin_mdio,
						preset.rmii_conf.pin_power,
						preset.rmii_conf.pin_rmii_clock,
					];
					miscConfig.ethSpiConfig = misc?.ethSpiConfig || [20, -1, -1, -1, -1, -1, -1];
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

<div class="flex flex-col py-6">
	<h1 class="md:text-3xl text-3xl font-bold mb-8 self-start">
		System Settings
		<span class="max-sm:before:transform-[translateX(-80%)!important] tooltip tooltip-bottom sm:tooltip-right tooltip-info sm:ml-2" data-tip="Reboot required to apply!">
			<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor"
				class="size-6">
				<path stroke-linecap="round" stroke-linejoin="round"
					d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z" />
			</svg>
		</span>
	</h1>
	{#if !misc && error}
		<div class="text-center text-error">
			<p>Error: {error}</p>
		</div>
	{:else if misc}
		<form onsubmit={saveMiscConfig} class="self-center">
			<div class="card bg-base-200 shadow-xl">
				<div class="card-body p-4">
					<div class="space-y-2">
						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">HomeKit</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<label class="label">
										<span class="label-text">Device Name</span>
									</label>
									<input type="text" bind:value={miscConfig.deviceName} placeholder="HK"
										class="input input-bordered w-full" required />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Setup Code</span>
									</label>
									<input type="number" bind:value={miscConfig.setupCode} placeholder="46637726" maxlength="8"
										class="input input-bordered w-full" required inputmode="numeric" />
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text">Always Lock on HomeKey</span>
										<input type="checkbox" bind:checked={miscConfig.lockAlwaysLock}
											class="toggle toggle-primary" />
									</label>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text">Always Unlock on HomeKey</span>
										<input type="checkbox" bind:checked={miscConfig.lockAlwaysUnlock}
											class="toggle toggle-primary" />
									</label>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text">SmartLock battery reporting</span>
										<input type="checkbox" bind:checked={miscConfig.proxBatEnabled}
											class="toggle toggle-primary" />
									</label>
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Battery low status Threshold</span>
									</label>
									<input type="number" bind:value={miscConfig.btrLowStatusThreshold} placeholder="10" min="0"
										max="100" class="input input-bordered w-full" />
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">HomeKey</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<label class="label">
										<span class="label-text">Alt action Initiator Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.hkAltActionInitPin} placeholder="255" min="0"
										max="255" class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Alt action Initiator Timeout (ms)</span>
									</label>
									<input type="number" bind:value={miscConfig.hkAltActionInitTimeout} placeholder="5000" min="0"
										max="10000" class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Feedback LED Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.hkAltActionInitLedPin} placeholder="255" min="0"
										max="255" class="input input-bordered w-full" />
								</div>
								<div class="divider">HomeKey Card Finish</div>
								<div class="flex flex-col items-center">
									<img src={hkFinishImage()} alt="HomeKey Finish" class="w-36 h-auto" />
									<div class="grid grid-cols-2 gap-2">
										{#each hkFinishColors as color}
											<label class="label cursor-pointer">
												<span class="label-text">{color.name}</span>
												<input type="radio" name="hk_key_color" bind:group={miscConfig.hk_key_color}
													value={color.value} class="radio radio-primary" />
											</label>
										{/each}
									</div>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">PN532</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<label class="label">
										<span class="label-text">SS Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.nfcGpioPins![0]} placeholder="5" min="0" max="255"
										class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">SCK Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.nfcGpioPins![1]} placeholder="18" min="0" max="255"
										class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">MISO Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.nfcGpioPins![2]} placeholder="19" min="0" max="255"
										class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">MOSI Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.nfcGpioPins![3]} placeholder="23" min="0" max="255"
										class="input input-bordered w-full" />
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">HomeSpan</div>
							<div class="collapse-content flex flex-col gap-4">
								<a href="https://github.com/HomeSpan/HomeSpan/blob/master/docs/GettingStarted.md#adding-a-control-button-and-status-led-optional"
									class="link link-primary block" target="_blank">HomeSpan Documentation</a>
								<div class="form-control">
									<label class="label">
										<span class="label-text">OTA Password</span>
									</label>
									<input type="password" bind:value={miscConfig.otaPasswd} placeholder="homespan-ota"
										class="input input-bordered w-full" required />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Control GPIO Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.controlPin} placeholder="26"
										class="input input-bordered w-full" />
								</div>
								<div class="form-control">
									<label class="label">
										<span class="label-text">Status LED GPIO Pin</span>
									</label>
									<input type="number" bind:value={miscConfig.hsStatusPin} placeholder="2"
										class="input input-bordered w-full" />
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">Ethernet</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text">Ethernet Enabled</span>
										<input type="checkbox" bind:checked={miscConfig.ethernetEnabled}
											class="toggle toggle-primary" />
									</label>
								</div>

								{#if miscConfig.ethernetEnabled}
									<div class="flex flex-col gap-4">
										<div class="form-control">
											<label class="label">
												<span class="label-text">Active Ethernet Preset</span>
											</label>
											<select bind:value={miscConfig.ethActivePreset} onchange={handleEthPresetChange}
												class="select select-bordered w-full">
												{#each ethConfig!.boardPresets || [] as preset, i}
													<option value={i}>{preset.name}</option>
												{/each}
												<option value={255}>Custom</option>
											</select>
										</div>

										<div class="form-control">
											<label class="label">
												<span class="label-text">Ethernet PHY Type</span>
											</label>
											<select bind:value={miscConfig.ethPhyType} disabled={miscConfig.ethActivePreset !== 255}
												class="select select-bordered w-full">
												{#each ethConfig!.supportedChips || [] as chip, i}
													<option value={i}>{chip.name}</option>
												{/each}
											</select>
										</div>

										{#if currentEthChip()?.emac}
											<div class="flex flex-col gap-4">
												<h3 class="text-lg font-bold">RMII Configuration</h3>
												<div class="flex flex-col gap-4">
													<div class="form-control">
														<label class="label">
															<span class="label-text">PHY Address</span>
														</label>
														<input type="number" bind:value={miscConfig.ethRmiiConfig![0]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin MCD</span>
														</label>
														<input type="number" bind:value={miscConfig.ethRmiiConfig![1]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin MDIO</span>
														</label>
														<input type="number" bind:value={miscConfig.ethRmiiConfig![2]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin Power</span>
														</label>
														<input type="number" bind:value={miscConfig.ethRmiiConfig![3]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin RMII Clock</span>
														</label>
														<select bind:value={miscConfig.ethRmiiConfig![4]}
															disabled={miscConfig.ethActivePreset !== 255} class="select select-bordered w-full">
															<option value={0}>GPIO0_IN</option>
															<option value={1}>GPIO0_OUT</option>
															<option value={2}>GPIO16_OUT</option>
															<option value={3}>GPIO17_OUT</option>
														</select>
													</div>
												</div>
											</div>
										{:else if !currentEthChip()?.emac}
											<div class="flex flex-col gap-4">
												<h3 class="text-lg font-bold">SPI Configuration</h3>
												<div class="flex flex-col gap-4">
													<div class="form-control">
														<label class="label">
															<span class="label-text">SPI Freq MHz</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![0]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin CS</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![1]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin IRQ</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![2]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin RST</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![3]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin SCK</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![4]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin MISO</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![5]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
													<div class="form-control">
														<label class="label">
															<span class="label-text">Pin MOSI</span>
														</label>
														<input type="number" bind:value={miscConfig.ethSpiConfig![6]}
															disabled={miscConfig.ethActivePreset !== 255} class="input input-bordered w-full" />
													</div>
												</div>
											</div>
										{/if}
									</div>
								{/if}
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">WebUI</div>
							<div class="collapse-content">
								<div class="form-control my-4">
									<label class="label cursor-pointer">
										<span class="label-text">Authentication Enabled</span>
										<input type="checkbox" bind:checked={miscConfig.webAuthEnabled}
											class="toggle toggle-primary" />
									</label>
								</div>
								<div class="form-control mb-4">
									<label class="label">
										<span class="label-text">Username</span>
									</label>
									<input type="text" bind:value={miscConfig.webUsername} placeholder="admin"
										class="input input-bordered w-full" required />
								</div>
								<div class="form-control mb-2">
									<label class="label">
										<span class="label-text">Password</span>
									</label>
									<input type="password" bind:value={miscConfig.webPassword} placeholder="password"
										class="input input-bordered w-full" required />
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
			<div class="card-actions justify-end mt-6 px-2">
				<button type="submit" class="btn btn-primary">Save & Apply</button>
				<button type="button" class="btn btn-ghost" onclick={resetForm}>Reset Form</button>
			</div>
		</form>
	{/if}
</div>
