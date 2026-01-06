<script lang="ts">
	import {
		rebootDevice,
		resetPairings,
		resetWifi,
		saveConfig,
		startConfigAP,
	} from "$lib/services/api.js";
	import HKFinish0 from "$lib/assets/hk-finish-0.webp?inline";
	import HKFinish1 from "$lib/assets/hk-finish-1.webp?inline";
	import HKFinish2 from "$lib/assets/hk-finish-2.webp?inline";
	import HKFinish3 from "$lib/assets/hk-finish-3.webp?inline";
	import type {
		EthConfig,
		MiscConfig,
		NfcGpioPinsPreset,
	} from "$lib/types/api";
	import { diff } from "$lib/utils/objDiff";

	let { misc, eth, nfcPresets, error } = $props();

	let miscConfig = $state<MiscConfig>(
		misc ?? {
			deviceName: "",
			otaPasswd: "",
			hk_key_color: 0,
			setupCode: "",
			lockAlwaysUnlock: false,
			lockAlwaysLock: false,
			hkAuthPrecomputeEnabled: false,
			proxBatEnabled: false,
			btrLowStatusThreshold: 10,
			webAuthEnabled: false,
			webUsername: "",
			webPassword: "",
			nfcPinsPreset: 255,
			nfcGpioPins: [5, 18, 19, 23],
			ethernetEnabled: false,
			ethActivePreset: 255,
			ethPhyType: 0,
			ethRmiiConfig: [0, 0, 0, 0, 0],
			ethSpiConfig: [20, -1, -1, -1, -1, -1, -1],
			controlPin: 26,
			hsStatusPin: 2,
			doorSensorPin: 32,
			doorSensorInvert: false,
		},
	);
	let ethConfig = $state<EthConfig>(
		eth ?? {
			supportedChips: [],
			boardPresets: [],
			ethEnabled: false,
		},
	);
	let nfcPresetsList = $state<NfcGpioPinsPreset>(
		nfcPresets ?? {
			presets: [],
		},
	);

	const hkFinishColors = [
		{ name: "Tan", value: 0, image: HKFinish0 },
		{ name: "Gold", value: 1, image: HKFinish1 },
		{ name: "Silver", value: 2, image: HKFinish2 },
		{ name: "Black", value: 3, image: HKFinish3 },
	];

	let hkFinishImage = $derived(() => {
		const color = hkFinishColors.find(
			(c) => c.value === miscConfig.hk_key_color,
		);
		return color?.image || HKFinish0;
	});

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

<div class="flex flex-col py-6">
	<h1 class="md:text-3xl text-3xl font-bold mb-4 self-start">
		System Settings
		<span
			class="max-sm:before:transform-[translateX(-80%)!important] tooltip tooltip-bottom sm:tooltip-right tooltip-info sm:ml-2"
			data-tip="Reboot required to apply!"
		>
			<svg
				xmlns="http://www.w3.org/2000/svg"
				fill="none"
				viewBox="0 0 24 24"
				stroke-width="1.5"
				stroke="currentColor"
				class="size-6"
			>
				<path
					stroke-linecap="round"
					stroke-linejoin="round"
					d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z"
				/>
			</svg>
		</span>
	</h1>
	{#if !misc && error}
		<div class="text-center text-error">
			<p>Error: {error}</p>
		</div>
	{:else if misc}
		<div class="flex gap-2 justify-center mb-4">
			<div class="flex flex-wrap justify-center gap-2 max-w-sm">
				<button
					class="btn btn-soft btn-warning"
					aria-label="Reboot Device"
					onclick={async () => {
						if (
							confirm(
								"This will restart the device, please confirm.",
							)
						) {
							await rebootDevice();
						}
					}}
				>
					<svg
						xmlns="http://www.w3.org/2000/svg"
						fill="none"
						viewBox="0 0 24 24"
						stroke-width="1.5"
						stroke="currentColor"
						class="size-6"
					>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="M16.023 9.348h4.992v-.001M2.985 19.644v-4.992m0 0h4.992m-4.993 0 3.181 3.183a8.25 8.25 0 0 0 13.803-3.7M4.031 9.865a8.25 8.25 0 0 1 13.803-3.7l3.181 3.182m0-4.991v4.99"
						/>
					</svg>
					Reboot
				</button>
				<button
					class="btn btn-soft btn-accent"
					onclick={async () => {
						if (
							confirm(
								"This will start the configuration AP, please confirm.\n\n Note that the WiFi connection will be dropped as it can only be in access point mode or station mode, not both.",
							)
						) {
							await startConfigAP();
						}
					}}
				>
					<svg
						xmlns="http://www.w3.org/2000/svg"
						fill="none"
						viewBox="0 0 24 24"
						stroke-width="1.5"
						stroke="currentColor"
						class="size-6"
					>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="M9.594 3.94c.09-.542.56-.94 1.11-.94h2.593c.55 0 1.02.398 1.11.94l.213 1.281c.063.374.313.686.645.87.074.04.147.083.22.127.325.196.72.257 1.075.124l1.217-.456a1.125 1.125 0 0 1 1.37.49l1.296 2.247a1.125 1.125 0 0 1-.26 1.431l-1.003.827c-.293.241-.438.613-.43.992a7.723 7.723 0 0 1 0 .255c-.008.378.137.75.43.991l1.004.827c.424.35.534.955.26 1.43l-1.298 2.247a1.125 1.125 0 0 1-1.369.491l-1.217-.456c-.355-.133-.75-.072-1.076.124a6.47 6.47 0 0 1-.22.128c-.331.183-.581.495-.644.869l-.213 1.281c-.09.543-.56.94-1.11.94h-2.594c-.55 0-1.019-.398-1.11-.94l-.213-1.281c-.062-.374-.312-.686-.644-.87a6.52 6.52 0 0 1-.22-.127c-.325-.196-.72-.257-1.076-.124l-1.217.456a1.125 1.125 0 0 1-1.369-.49l-1.297-2.247a1.125 1.125 0 0 1 .26-1.431l1.004-.827c.292-.24.437-.613.43-.991a6.932 6.932 0 0 1 0-.255c.007-.38-.138-.751-.43-.992l-1.004-.827a1.125 1.125 0 0 1-.26-1.43l1.297-2.247a1.125 1.125 0 0 1 1.37-.491l1.216.456c.356.133.751.072 1.076-.124.072-.044.146-.086.22-.128.332-.183.582-.495.644-.869l.214-1.28Z"
						/>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="M15 12a3 3 0 1 1-6 0 3 3 0 0 1 6 0Z"
						/>
					</svg>
					Start AP
				</button>
				<button
					class="btn btn-soft btn-error"
					onclick={async () => {
						if (
							confirm(
								"This will reset the HomeKit pairings, please confirm.",
							)
						) {
							await resetPairings();
						}
					}}
				>
					<svg
						xmlns="http://www.w3.org/2000/svg"
						fill="none"
						viewBox="0 0 24 24"
						stroke-width="1.5"
						stroke="currentColor"
						class="size-6"
					>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0"
						/>
					</svg>
					Reset HomeKit
				</button>
				<button
					class="btn btn-soft btn-error"
					onclick={async () => {
						if (
							confirm(
								"This will reset the WiFi credentials, please confirm.",
							)
						) {
							await resetWifi();
						}
					}}
				>
					<svg
						xmlns="http://www.w3.org/2000/svg"
						fill="none"
						viewBox="0 0 24 24"
						stroke-width="1.5"
						stroke="currentColor"
						class="size-6"
					>
						<path
							stroke-linecap="round"
							stroke-linejoin="round"
							d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0"
						/>
					</svg>
					Reset WiFi
				</button>
			</div>
		</div>
		<form onsubmit={saveMiscConfig} class="self-center">
			<div class="card bg-base-200 shadow-xl">
				<div class="card-body p-4">
					<div class="space-y-2">
						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">
								HomeKit
							</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>Device Name</span
										>
									</label>
									<input
										type="text"
										bind:value={miscConfig.deviceName}
										placeholder="HK"
										class="input input-bordered w-full"
										required
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>Setup Code</span
										>
									</label>
									<input
										type="text"
										bind:value={miscConfig.setupCode}
										placeholder="46637726"
										maxlength="8"
										class="input input-bordered w-full"
										required
										inputmode="numeric"
									/>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>Always Lock on HomeKey</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.lockAlwaysLock
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>Always Unlock on HomeKey</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.lockAlwaysUnlock
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>SmartLock battery reporting</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.proxBatEnabled
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>Battery low status Threshold</span
										>
									</label>
									<input
										type="number"
										bind:value={
											miscConfig.btrLowStatusThreshold
										}
										placeholder="10"
										min="0"
										max="100"
										class="input input-bordered w-full"
									/>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">
								HomeKey
							</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="divider">HomeKey Card Finish</div>
								<div class="flex flex-col items-center">
									<img
										src={hkFinishImage()}
										alt="HomeKey Finish"
										class="w-36 h-auto mb-2"
									/>
									<div class="grid grid-cols-2 gap-2">
										{#each hkFinishColors as color}
											<label class="label cursor-pointer">
												<span class="label-text"
													>{color.name}</span
												>
												<input
													type="radio"
													name="hk_key_color"
													bind:group={
														miscConfig.hk_key_color
													}
													value={color.value}
													class="radio radio-primary"
												/>
											</label>
										{/each}
									</div>
								</div>
								<div class="divider">Performance</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>Auth Precompute Cache</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.hkAuthPrecomputeEnabled
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">PN532</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">Preset</span>
									</label>
									<select
										bind:value={miscConfig.nfcPinsPreset}
										onchange={handleNfcPresetChange}
										class="select select-bordered w-full"
									>
										{#each nfcPresetsList.presets as preset, i}
											<option value={i}
												>{preset.name}</option
											>
										{/each}
										<option value={255}>Custom</option>
									</select>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">SS Pin</span>
									</label>
									<input
										type="number"
										disabled={miscConfig.nfcPinsPreset !==
											255}
										bind:value={miscConfig.nfcGpioPins![0]}
										placeholder="5"
										min="0"
										max="255"
										class="input input-bordered w-full"
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">SCK Pin</span>
									</label>
									<input
										type="number"
										disabled={miscConfig.nfcPinsPreset !==
											255}
										bind:value={miscConfig.nfcGpioPins![1]}
										placeholder="18"
										min="0"
										max="255"
										class="input input-bordered w-full"
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">MISO Pin</span>
									</label>
									<input
										type="number"
										disabled={miscConfig.nfcPinsPreset !==
											255}
										bind:value={miscConfig.nfcGpioPins![2]}
										placeholder="19"
										min="0"
										max="255"
										class="input input-bordered w-full"
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">MOSI Pin</span>
									</label>
									<input
										type="number"
										disabled={miscConfig.nfcPinsPreset !==
											255}
										bind:value={miscConfig.nfcGpioPins![3]}
										placeholder="23"
										min="0"
										max="255"
										class="input input-bordered w-full"
									/>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">
								HomeSpan
							</div>
							<div class="collapse-content flex flex-col gap-4">
								<a
									href="https://github.com/HomeSpan/HomeSpan/blob/master/docs/GettingStarted.md#adding-a-control-button-and-status-led-optional"
									class="link link-primary block"
									target="_blank">HomeSpan Documentation</a
								>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>OTA Password</span
										>
									</label>
									<input
										type="password"
										bind:value={miscConfig.otaPasswd}
										placeholder="homespan-ota"
										class="input input-bordered w-full"
										required
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>Control GPIO Pin</span
										>
									</label>
									<input
										type="number"
										bind:value={miscConfig.controlPin}
										placeholder="26"
										class="input input-bordered w-full"
									/>
								</div>
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>Status LED GPIO Pin</span
										>
									</label>
									<input
										type="number"
										bind:value={miscConfig.hsStatusPin}
										placeholder="2"
										class="input input-bordered w-full"
									/>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">
								Door Sensor
							</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text"
											>GPIO Pin</span
										>
									</label>
									<input
										type="number"
										bind:value={miscConfig.doorSensorPin}
										placeholder="32"
										min="0"
                                        max="255"
										class="input input-bordered w-full"
									/>
								</div>
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>Invert Logic (HIGH = Closed)</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.doorSensorInvert
											}
											class="toggle toggle-primary"
										/>
									</label>
									<label class="label">
										<span class="label-text-alt text-xs opacity-70">
											Default: LOW = Closed, HIGH = Open<br />
											When inverted: HIGH = Closed, LOW = Open
										</span>
									</label>
								</div>
							</div>
						</div>

						<div class="collapse collapse-arrow bg-base-100">
							<input type="checkbox" name="misc-collapse" />
							<div class="collapse-title font-medium">
								Ethernet
							</div>
							<div class="collapse-content flex flex-col gap-4">
								<div class="form-control">
									<label class="label cursor-pointer">
										<span class="label-text"
											>Ethernet Enabled</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.ethernetEnabled
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>

								{#if miscConfig.ethernetEnabled}
									<div class="flex flex-col gap-4">
										<div class="form-control">
											<!-- svelte-ignore a11y_label_has_associated_control -->
											<label class="label">
												<span class="label-text"
													>Active Ethernet Preset</span
												>
											</label>
											<select
												bind:value={
													miscConfig.ethActivePreset
												}
												onchange={handleEthPresetChange}
												class="select select-bordered w-full"
											>
												{#each ethConfig!.boardPresets || [] as preset, i}
													<option value={i}
														>{preset.name}</option
													>
												{/each}
												<option value={255}
													>Custom</option
												>
											</select>
										</div>

										<div class="form-control">
											<!-- svelte-ignore a11y_label_has_associated_control -->
											<label class="label">
												<span class="label-text"
													>Ethernet PHY Type</span
												>
											</label>
											<select
												bind:value={
													miscConfig.ethPhyType
												}
												disabled={miscConfig.ethActivePreset !==
													255}
												class="select select-bordered w-full"
											>
												{#each ethConfig!.supportedChips as chip}
													<option
														value={chip.phy_type}
														>{chip.name}</option
													>
												{/each}
											</select>
										</div>

										{#if currentEthChip()?.emac}
											<div class="flex flex-col gap-4">
												<h3 class="text-lg font-bold">
													RMII Configuration
												</h3>
												<div
													class="flex flex-col gap-4"
												>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>PHY Address</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethRmiiConfig![0]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin MCD</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethRmiiConfig![1]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin MDIO</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethRmiiConfig![2]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin Power</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethRmiiConfig![3]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin RMII Clock</span
															>
														</label>
														<select
															bind:value={
																miscConfig
																	.ethRmiiConfig![4]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="select select-bordered w-full"
														>
															<option value={0}
																>GPIO0_IN</option
															>
															<option value={1}
																>GPIO0_OUT</option
															>
															<option value={2}
																>GPIO16_OUT</option
															>
															<option value={3}
																>GPIO17_OUT</option
															>
														</select>
													</div>
												</div>
											</div>
										{:else if !currentEthChip()?.emac}
											<div class="flex flex-col gap-4">
												<h3 class="text-lg font-bold">
													SPI Configuration
												</h3>
												<div
													class="flex flex-col gap-4"
												>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>SPI Freq MHz</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![0]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin CS</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![1]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin IRQ</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![2]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin RST</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![3]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin SCK</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![4]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin MISO</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![5]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
													</div>
													<div class="form-control">
														<!-- svelte-ignore a11y_label_has_associated_control -->
														<label class="label">
															<span
																class="label-text"
																>Pin MOSI</span
															>
														</label>
														<input
															type="number"
															bind:value={
																miscConfig
																	.ethSpiConfig![6]
															}
															disabled={miscConfig.ethActivePreset !==
																255}
															class="input input-bordered w-full"
														/>
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
										<span class="label-text"
											>Authentication Enabled</span
										>
										<input
											type="checkbox"
											bind:checked={
												miscConfig.webAuthEnabled
											}
											class="toggle toggle-primary"
										/>
									</label>
								</div>
								<div class="form-control mb-4">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">Username</span>
									</label>
									<input
										type="text"
										bind:value={miscConfig.webUsername}
										placeholder="admin"
										class="input input-bordered w-full"
										required
									/>
								</div>
								<div class="form-control mb-2">
									<!-- svelte-ignore a11y_label_has_associated_control -->
									<label class="label">
										<span class="label-text">Password</span>
									</label>
									<input
										type="password"
										bind:value={miscConfig.webPassword}
										placeholder="password"
										class="input input-bordered w-full"
										required
									/>
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
			<div class="card-actions justify-end mt-6 px-2">
				<button type="submit" class="btn btn-primary"
					>Save & Apply</button
				>
				<button type="button" class="btn btn-ghost" onclick={resetForm}
					>Reset Form</button
				>
			</div>
		</form>
	{/if}
</div>
