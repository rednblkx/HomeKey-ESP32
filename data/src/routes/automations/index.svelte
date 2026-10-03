<script lang="ts">
	import AppAutomations from '$lib/components/AppAutomations.svelte';
	import { route } from 'sv-router/generated';
</script>

<AppAutomations
	config={route.meta.automationsData?.config ?? undefined}
	schema={route.meta.automationsData?.schema ?? null}
	error={route.meta.automationsData?.error ?? null}
/>
