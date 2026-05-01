<script lang="ts">
	import type { EthConfig, NfcGpioPinsPreset } from '$lib/types/api';
    import { derived } from 'svelte/store';
	import SpiEthernetNote from './SpiEthernetNote.svelte';
	import { route } from 'sv-router/generated';

	interface Props {
		nfcGpioPins: [number, number, number, number];
		nfcPinsPreset: number;
		nfcPresets: NfcGpioPinsPreset | null;
		ethernetEnabled: boolean;
		ethActivePreset: number;
		ethPhyType: number;
		ethSpiBus: number;
		ethRmiiConfig: [number, number, number, number, number];
		ethSpiConfig: [number, number, number, number, number, number, number];
		ethConfig: EthConfig | null;
		nfcConnected?: boolean;
		loading?: boolean;
    nfcFastPollingEnabled: boolean;
		onNfcPresetChange: (preset: number) => void;
		onEthPresetChange: (preset: number) => void;
		onNfcPinsChange: (pins: [number, number, number, number]) => void;
		onEthernetToggle: (enabled: boolean) => void;
		onEthPhyTypeChange: (phyType: number) => void;
		onEthSpiBusChange: (bus: number) => void;
		onEthRmiiConfigChange: (config: [number, number, number, number, number]) => void;
		onEthSpiConfigChange: (config: [number, number, number, number, number, number, number]) => void;
	}

	let {
		nfcGpioPins,
		nfcPinsPreset,
		nfcPresets,
		ethernetEnabled,
		ethActivePreset,
		ethPhyType,
		ethSpiBus,
		ethRmiiConfig,
		ethSpiConfig,
		ethConfig,
    nfcFastPollingEnabled,
		nfcConnected = false,
		loading = false,
		onNfcPresetChange,
		onEthPresetChange,
		onNfcPinsChange,
		onEthernetToggle,
		onEthPhyTypeChange,
		onEthSpiBusChange,
		onEthRmiiConfigChange,
		onEthSpiConfigChange,
	}: Props = $props();

	const isCaptivePortal = $derived(route.pathname.startsWith('/captive-portal'));

	let currentEthChip = $derived(() => {
		if (ethPhyType !== undefined && ethConfig?.supportedChips) {
			return ethConfig.supportedChips[ethPhyType];
		}
		return null;
	});

	function handleNfcPresetChange(event: Event) {
		const select = event.target as HTMLSelectElement;
		const preset = parseInt(select.value, 10);
		onNfcPresetChange(preset);
	}

	function handleEthPresetChange(event: Event) {
		const select = event.target as HTMLSelectElement;
		const preset = parseInt(select.value, 10);
		onEthPresetChange(preset);
	}

	function updateNfcPin(index: number, value: number) {
		const newPins: [number, number, number, number] = [...nfcGpioPins] as [number, number, number, number];
		newPins[index] = value;
		onNfcPinsChange(newPins);
	}

	function updateRmiiConfig(index: number, value: number) {
		const newConfig: [number, number, number, number, number] = [...ethRmiiConfig] as [number, number, number, number, number];
		newConfig[index] = value;
		onEthRmiiConfigChange(newConfig);
	}

	function updateSpiConfig(index: number, value: number) {
		const newConfig: [number, number, number, number, number, number, number] = [...ethSpiConfig] as [number, number, number, number, number, number, number];
		newConfig[index] = value;
		onEthSpiConfigChange(newConfig);
	}
</script>

<div class="space-y-4">
  {#if ethernetEnabled && !currentEthChip()?.emac}
    <SpiEthernetNote spiNumBuses={ethConfig?.numSpiBuses ?? 1} selectedBus={ethSpiBus} />
  {/if}
	<!-- PN532 NFC Reader -->
	<div class="py-2 px-3 bg-base-100 rounded-lg">
		<div class="flex items-center justify-between mb-2">
			<p class="text-sm font-medium">PN532 NFC Reader</p>
			{#if nfcConnected !== undefined && !isCaptivePortal}
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
			{/if}
		</div>
		<div class="form-control mb-2">
			<label class="label" for="nfcPreset">
				<span class="label-text text-xs">Preset</span>
			</label>
			<select
				id="nfcPreset"
				value={nfcPinsPreset}
				onchange={handleNfcPresetChange}
				class="select select-sm select-bordered w-full"
				disabled={loading}
			>
				{#if nfcPresets?.presets}
					{#each nfcPresets.presets as preset, i}
						<option value={i}>{preset.name}</option>
					{/each}
				{/if}
				<option value={255}>Custom</option>
			</select>
		</div>
		<div class="grid grid-cols-4 gap-2 mb-2">
			<div class="form-control">
				<label class="label" for="nfcSsPin">
					<span class="label-text text-xs">SS Pin</span>
				</label>
				<input
					id="nfcSsPin"
					type="number"
					disabled={nfcPinsPreset !== 255 || loading}
					value={nfcGpioPins[0]}
					onchange={(e) => updateNfcPin(0, parseInt((e.target as HTMLInputElement).value, 10))}
					class="input input-sm input-bordered w-full"
				/>
			</div>
			<div class="form-control">
				<label class="label" for="nfcSckPin">
					<span class="label-text text-xs">SCK Pin</span>
				</label>
				<input
					id="nfcSckPin"
					type="number"
					disabled={nfcPinsPreset !== 255 || loading}
					value={nfcGpioPins[1]}
					onchange={(e) => updateNfcPin(1, parseInt((e.target as HTMLInputElement).value, 10))}
					class="input input-sm input-bordered w-full"
				/>
			</div>
			<div class="form-control">
				<label class="label" for="nfcMisoPin">
					<span class="label-text text-xs">MISO Pin</span>
				</label>
				<input
					id="nfcMisoPin"
					type="number"
					disabled={nfcPinsPreset !== 255 || loading}
					value={nfcGpioPins[2]}
					onchange={(e) => updateNfcPin(2, parseInt((e.target as HTMLInputElement).value, 10))}
					class="input input-sm input-bordered w-full"
				/>
			</div>
			<div class="form-control">
				<label class="label" for="nfcMosiPin">
					<span class="label-text text-xs">MOSI Pin</span>
				</label>
				<input
					id="nfcMosiPin"
					type="number"
					disabled={nfcPinsPreset !== 255 || loading}
					value={nfcGpioPins[3]}
					onchange={(e) => updateNfcPin(3, parseInt((e.target as HTMLInputElement).value, 10))}
					class="input input-sm input-bordered w-full"
				/>
			</div>
		</div>
    <div class="flex items-center justify-between py-2 px-3 bg-base-200 rounded-lg">
      <div>
        <p class="text-sm font-medium">Fast NFC Polling</p>
        <p class="text-xs text-base-content/60">{"Reduces the delay (100ms -> 5ms) after each PN532 poll cycle for quicker follow-up detection."}</p>
      </div>
      <input
        type="checkbox"
        bind:checked={nfcFastPollingEnabled}
        class="toggle toggle-primary toggle-sm"
      />
    </div>
	</div>

	<!-- Ethernet Configuration -->
	<div class="py-2 px-3 bg-base-100 rounded-lg">
		<p class="text-sm font-medium mb-2">Ethernet Configuration</p>

		<div class="flex items-center justify-between mb-3">
			<div>
				<p class="text-sm font-medium">Enable Ethernet</p>
				<p class="text-xs text-base-content/60">Use wired Ethernet instead of WiFi</p>
			</div>
			<input
				type="checkbox"
				checked={ethernetEnabled}
				onchange={(e) => onEthernetToggle((e.target as HTMLInputElement).checked)}
				class="toggle toggle-primary toggle-sm"
				disabled={loading}
			/>
		</div>

		{#if ethernetEnabled}
			<div class="form-control">
				<label class="label" for="ethPreset">
					<span class="label-text text-xs">Board Preset</span>
				</label>
				<select
					id="ethPreset"
					value={ethActivePreset}
					onchange={handleEthPresetChange}
					class="select select-sm select-bordered w-full"
					disabled={loading}
				>
					{#if ethConfig?.boardPresets}
						{#each ethConfig.boardPresets as preset, i}
							<option value={i}>{preset.name}</option>
						{/each}
					{/if}
					<option value={255}>Custom</option>
				</select>
			</div>

			<div class="form-control">
				<label class="label" for="ethPhyType">
					<span class="label-text text-xs">PHY Type</span>
				</label>
				<select
					id="ethPhyType"
					value={ethPhyType}
					onchange={(e) => onEthPhyTypeChange(parseInt((e.target as HTMLSelectElement).value, 10))}
					disabled={ethActivePreset !== 255 || loading}
					class="select select-sm select-bordered w-full"
				>
					{#if ethConfig?.supportedChips}
						{#each ethConfig.supportedChips as chip}
							<option value={chip.phy_type}>{chip.name}</option>
						{/each}
					{/if}
				</select>
			</div>

			{#if currentEthChip()?.emac}
				<div class="py-2 px-3 bg-base-100 rounded-lg">
					<p class="text-sm font-medium mb-2">RMII Configuration</p>
					<div class="grid grid-cols-2 md:grid-cols-3 gap-2">
						<div class="form-control">
							<label class="label" for="ethPhyAddr">
								<span class="label-text text-xs">PHY Address</span>
							</label>
							<input
								id="ethPhyAddr"
								type="number"
								value={ethRmiiConfig[0]}
								onchange={(e) => updateRmiiConfig(0, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethPinMdc">
								<span class="label-text text-xs">Pin MDC</span>
							</label>
							<input
								id="ethPinMdc"
								type="number"
								value={ethRmiiConfig[1]}
								onchange={(e) => updateRmiiConfig(1, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethPinMdio">
								<span class="label-text text-xs">Pin MDIO</span>
							</label>
							<input
								id="ethPinMdio"
								type="number"
								value={ethRmiiConfig[2]}
								onchange={(e) => updateRmiiConfig(2, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethPinPower">
								<span class="label-text text-xs">Pin Power</span>
							</label>
							<input
								id="ethPinPower"
								type="number"
								value={ethRmiiConfig[3]}
								onchange={(e) => updateRmiiConfig(3, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethRmiiClock">
								<span class="label-text text-xs">RMII Clock</span>
							</label>
							<select
								id="ethRmiiClock"
								value={ethRmiiConfig[4]}
								onchange={(e) => updateRmiiConfig(4, parseInt((e.target as HTMLSelectElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
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
					<div class="grid grid-cols-2 md:grid-cols-4 gap-2">
						<div class="form-control">
							<label class="label" for="ethSpiBus">
								<span class="label-text text-xs">SPI Bus</span>
							</label>
							<select
								id="ethSpiBus"
								value={ethSpiBus}
								onchange={(e) => onEthSpiBusChange(parseInt((e.target as HTMLSelectElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading || ethConfig?.numSpiBuses === 1}
								class="select select-sm select-bordered w-full"
							>
								<option value={1}>SPI2</option>
								{#if ethConfig?.numSpiBuses === 2}
									<option value={2}>SPI3</option>
								{/if}
							</select>
						</div>
						<div class="form-control">
							<label class="label" for="ethSpiFreq">
								<span class="label-text text-xs">Freq (MHz)</span>
							</label>
							<input
								id="ethSpiFreq"
								type="number"
								value={ethSpiConfig[0]}
								onchange={(e) => updateSpiConfig(0, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethCsPin">
								<span class="label-text text-xs">CS Pin</span>
							</label>
							<input
								id="ethCsPin"
								type="number"
								value={ethSpiConfig[1]}
								onchange={(e) => updateSpiConfig(1, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethIrqPin">
								<span class="label-text text-xs">IRQ Pin</span>
							</label>
							<input
								id="ethIrqPin"
								type="number"
								value={ethSpiConfig[2]}
								onchange={(e) => updateSpiConfig(2, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethRstPin">
								<span class="label-text text-xs">RST Pin</span>
							</label>
							<input
								id="ethRstPin"
								type="number"
								value={ethSpiConfig[3]}
								onchange={(e) => updateSpiConfig(3, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethSckPin">
								<span class="label-text text-xs">SCK Pin</span>
							</label>
							<input
								id="ethSckPin"
								type="number"
								value={ethSpiConfig[4]}
								onchange={(e) => updateSpiConfig(4, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethMisoPin">
								<span class="label-text text-xs">MISO Pin</span>
							</label>
							<input
								id="ethMisoPin"
								type="number"
								value={ethSpiConfig[5]}
								onchange={(e) => updateSpiConfig(5, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label" for="ethMosiPin">
								<span class="label-text text-xs">MOSI Pin</span>
							</label>
							<input
								id="ethMosiPin"
								type="number"
								value={ethSpiConfig[6]}
								onchange={(e) => updateSpiConfig(6, parseInt((e.target as HTMLInputElement).value, 10))}
								disabled={ethActivePreset !== 255 || loading}
								class="input input-sm input-bordered w-full"
							/>
						</div>
					</div>
				</div>
			{/if}
		{/if}
	</div>
</div>
