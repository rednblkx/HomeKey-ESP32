<script>
	import HKInfo from '$lib/components/HKInfo.svelte';
	import { loadingStates } from '$lib/stores/loading.svelte.js';
	import { route } from 'sv-router/generated';
</script>

{#if loadingStates.pageLoading}
	<div class="flex justify-center items-center min-h-screen" aria-live="polite" aria-label="Loading page">
		<div class="text-center">
			<span class="loading loading-spinner loading-lg" aria-label="Loading page content"></span>
			<p class="mt-4 text-lg">Loading HomeKey information...</p>
		</div>
	</div>
{:else}
	<HKInfo hkInfo={route.meta.homeData?.hkInfo} error={route.meta.homeData?.error} />
{/if}
