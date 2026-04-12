<script lang="ts">
	import AppMisc from '$lib/components/AppMisc.svelte';
	import { route } from 'sv-router/generated';
</script>

<AppMisc misc={route.meta.miscData?.misc} eth={route.meta.miscData?.eth} error={route.meta.miscData?.error} nfcPresets={route.meta.miscData?.nfcPresets} />
