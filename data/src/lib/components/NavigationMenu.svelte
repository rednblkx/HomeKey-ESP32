<script lang="ts">
	import { onMount } from 'svelte';
	import { getCurrentTheme, toggleTheme, initTheme, logoSrc } from '$lib/stores/theme.svelte.js';
    import { websocketState } from '$lib/stores/websocket.svelte';

	interface Props {
		onClose: () => void;
		isMobile: boolean;
		triggerElement: HTMLDivElement | null;
		id?: string;
	}

	let { onClose, isMobile, triggerElement = $bindable(), id }: Props = $props();

	let ul: HTMLUListElement;

	onMount(() => {
		initTheme();
	});
</script>

<div class="bg-base-200 text-base-content lg:w-70 h-full flex flex-col" role="navigation" aria-label="Main navigation" {id}>
	<!-- Header Section -->
	<div class="text-lg font-bold hidden lg:block">
		<div class="flex items-center flex-col p-4 justify-center">
			<img src={logoSrc()} alt="HomeKey-ESP32 logo" class="w-8 h-8" />
			<div class="flex items-center flex-col">
				<span>HomeKey-ESP32</span>
				<div class="flex items-center gap-1">
					<div class="inline-grid *:[grid-area:1/1]">
						<div class="status animate-ping" class:status-success={websocketState.connected} class:status-error={!websocketState.connected}></div>
						<div class="status" class:status-success={websocketState.connected} class:status-error={!websocketState.connected}></div>
					</div>
					<span class="text-sm" class:text-success={websocketState.connected} class:text-error={!websocketState.connected}>{websocketState.connected ? 'Online' : 'Offline'}</span>
				</div>
			</div>
		</div>
	</div>

	<!-- Menu Section -->
	<!-- svelte-ignore a11y_click_events_have_key_events -->
	<ul
		class="menu flex-1 p-4 w-full bg-base-100 lg:bg-base-200"
		role="menu"
		bind:this={ul}
		onclick={onClose}
	>
		<li class="my-1" role="menuitem">
			<a href="/" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
				</svg>
				Info
			</a>
		</li>
		<li class="my-1" role="menuitem">
			<a href="/mqtt" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-6 mr-2" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" d="M9.348 14.652a3.75 3.75 0 0 1 0-5.304m5.304 0a3.75 3.75 0 0 1 0 5.304m-7.425 2.121a6.75 6.75 0 0 1 0-9.546m9.546 0a6.75 6.75 0 0 1 0 9.546M5.106 18.894c-3.808-3.807-3.808-9.98 0-13.788m13.788 0c3.808 3.807 3.808 9.98 0 13.788M12 12h.008v.008H12V12Zm.375 0a.375.375 0 1 1-.75 0 .375.375 0 0 1 .75 0Z" />
				</svg>
				MQTT
			</a>
		</li>
		<li class="my-1" role="menuitem">
			<a href="/actions" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 18h.01M8 21h8a2 2 0 002-2V5a2 2 0 00-2-2H8a2 2 0 00-2 2v14a2 2 0 002 2z" />
				</svg>
				Actions
			</a>
		</li>
		<li class="my-1" role="menuitem">
			<a href="/misc" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10.325 4.317c.426-1.756 2.924-1.756 3.35 0a1.724 1.724 0 002.573 1.066c1.543-.94 3.31.826 2.37 2.37a1.724 1.724 0 001.065 2.572c1.756.426 1.756 2.924 0 3.35a1.724 1.724 0 00-1.066 2.573c.94 1.543-.826 3.31-2.37 2.37a1.724 1.724 0 00-2.572 1.065c-.426 1.756-2.924 1.756-3.35 0a1.724 1.724 0 00-2.573-1.066c-1.543.94-3.31-.826-2.37-2.37a1.724 1.724 0 00-1.065-2.572c-1.756-.426-1.756-2.924 0-3.35a1.724 1.724 0 001.066-2.573c-.94-1.543.826-3.31 2.37-2.37.996.608 2.296.07 2.572-1.065z" />
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 12a3 3 0 11-6 0 3 3 0 016 0z" />
				</svg>
				System
			</a>
		</li>
		<li class="my-1" role="menuitem">
			<a href="/ota" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12" />
				</svg>
				OTA Update
			</a>
		</li>
		<li class="my-1" role="menuitem">
			<a href="/logs" class="text-lg flex items-center">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
				</svg>
				Logs
			</a>
		</li>
		<li class="lg:mt-auto hidden lg:inline-grid" role="menuitem">
			<label class="swap swap-rotate" aria-label="Toggle theme">

				<input type="checkbox" onclick={toggleTheme} checked={getCurrentTheme() !== 'dracula'} />

				<svg class="swap-on w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />
				</svg>
				<svg class="swap-off w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2" aria-hidden="true">
					<path stroke-linecap="round" stroke-linejoin="round" d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z" />
				</svg>
			</label>
		</li>
	</ul>
</div>
