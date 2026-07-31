<script lang="ts">
	import type { EthConfig, NfcGpioPinsPreset } from '$lib/types/api';
	import SpiEthernetNote from './SpiEthernetNote.svelte';
	import { route } from 'sv-router/generated';

	interface Props {
		nfcGpioPins: [number, number, number, number];
		nfcPinsPreset: number;
		nfcPresets: NfcGpioPinsPreset | null;
		nfcReaderType: number;
		nfcIrqPin: number;
		nfcVenPin: number;
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
    overrideStrappingRestriction: boolean;
	}

	let {
		nfcGpioPins = $bindable(),
		nfcPinsPreset = $bindable(),
		nfcPresets,
		nfcReaderType = $bindable(),
		nfcIrqPin = $bindable(),
		nfcVenPin = $bindable(),
		ethernetEnabled = $bindable(),
		ethActivePreset = $bindable(),
		ethPhyType = $bindable(),
		ethSpiBus = $bindable(),
		ethRmiiConfig = $bindable(),
		ethSpiConfig = $bindable(),
		ethConfig,
		nfcFastPollingEnabled = $bindable(false),
    overrideStrappingRestriction = $bindable(),
		nfcConnected = false,
		loading = false,
	}: Props = $props();

	const isCaptivePortal = $derived(route.pathname.startsWith('/captive-portal'));

	let currentEthChip = $derived(() => {
		if (ethPhyType !== undefined && ethConfig?.supportedChips) {
			return ethConfig.supportedChips[ethPhyType];
		}
		return null;
	});
</script>

<div class="space-y-4">
  {#if ethernetEnabled && !currentEthChip()?.emac}
    <SpiEthernetNote spiNumBuses={ethConfig?.numSpiBuses ?? 1} selectedBus={ethSpiBus} />
  {/if}
	<div class="py-2 px-3 bg-base-100 rounded-lg">
		<div class="flex items-center justify-between mb-2">
			<p class="text-sm font-medium">GPIO Allocation</p>
		</div>
    <div class="flex justify-between items-center py-2 px-3 bg-error/10 border border-error/20 rounded-lg">
      <div class="flex items-center gap-2">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="max-w-5 text-error">
          <path stroke-linecap="round" stroke-linejoin="round" d="M12 9v3.75m-9.303 3.376c-.866 1.5.217 3.374 1.948 3.374h14.71c1.73 0 2.813-1.874 1.948-3.374L13.949 3.378c-.866-1.5-3.032-1.5-3.898 0L2.697 16.126ZM12 15.75h.007v.008H12v-.008Z" />
        </svg>
        <div>
          <p class="text-sm font-medium">Override strapping pins restriction</p>
          <p class="text-xs text-base-content/60">CAUTION! Using one of the strapping pins can disrupt normal operation if not handled with care.</p>
        </div>
      </div>
      <input
        type="checkbox"
        bind:checked={overrideStrappingRestriction}
        class="toggle toggle-primary toggle-sm"
        disabled={loading}
      />
    </div>
	</div>
	<!-- NFC Reader -->
	<div class="py-2 px-3 bg-base-100 rounded-lg">
		<div class="flex items-center justify-between mb-2">
			<p class="text-sm font-medium">NFC Reader</p>
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
			<label class="label" for="nfcReaderType">
				<span class="label-text text-xs">Reader Type</span>
			</label>
			<select
				id="nfcReaderType"
				bind:value={nfcReaderType}
				class="select select-sm select-bordered w-full"
				disabled={loading}
			>
				<option value={0}>PN532</option>
				<option value={1}>PN7161</option>
			</select>
		</div>
		<div class="form-control mb-2">
			<label class="label" for="nfcPreset">
				<span class="label-text text-xs">Preset</span>
			</label>
			<select
				id="nfcPreset"
				bind:value={nfcPinsPreset}
				class="select select-sm select-bordered w-full"
				disabled={loading}
			>
				{#if nfcPresets?.presets}
					{#each nfcPresets.presets as preset, i}
						{#if preset.type === nfcReaderType}
							<option value={i}>{preset.name}</option>
						{/if}
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
					bind:value={nfcGpioPins[0]}
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
					bind:value={nfcGpioPins[1]}
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
					bind:value={nfcGpioPins[2]}
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
					bind:value={nfcGpioPins[3]}
					class="input input-sm input-bordered w-full"
				/>
			</div>
		</div>
		{#if nfcReaderType === 1}
			<div class="grid grid-cols-2 gap-2 mb-2">
				<div class="form-control">
					<label class="label" for="nfcIrqPin">
						<span class="label-text text-xs">IRQ Pin</span>
					</label>
					<input
						id="nfcIrqPin"
						type="number"
						disabled={nfcPinsPreset !== 255 || loading}
						bind:value={nfcIrqPin}
						class="input input-sm input-bordered w-full"
					/>
				</div>
				<div class="form-control">
					<label class="label" for="nfcVenPin">
						<span class="label-text text-xs">VEN Pin</span>
					</label>
					<input
						id="nfcVenPin"
						type="number"
						disabled={nfcPinsPreset !== 255 || loading}
						bind:value={nfcVenPin}
						class="input input-sm input-bordered w-full"
					/>
				</div>
			</div>
		{/if}
    <div class="flex items-center justify-between py-2 px-3 bg-base-200 rounded-lg">
      <div>
        <p class="text-sm font-medium">Fast NFC Polling</p>
        <p class="text-xs text-base-content/60">Reduces the delay between poll cycles for quicker tag detection.</p>
      </div>
      <input
        type="checkbox"
        bind:checked={nfcFastPollingEnabled}
        class="toggle toggle-primary toggle-sm"
		disabled={loading}
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
				bind:checked={ethernetEnabled}
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
					bind:value={ethActivePreset}
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
					bind:value={ethPhyType}
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
								bind:value={ethRmiiConfig[0]}
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
								bind:value={ethRmiiConfig[1]}
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
								bind:value={ethRmiiConfig[2]}
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
								bind:value={ethRmiiConfig[3]}
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
								bind:value={ethRmiiConfig[4]}
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
								bind:value={ethSpiBus}
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
								bind:value={ethSpiConfig[0]}
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
								bind:value={ethSpiConfig[1]}
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
								bind:value={ethSpiConfig[2]}
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
								bind:value={ethSpiConfig[3]}
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
								bind:value={ethSpiConfig[4]}
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
								bind:value={ethSpiConfig[5]}
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
								bind:value={ethSpiConfig[6]}
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
