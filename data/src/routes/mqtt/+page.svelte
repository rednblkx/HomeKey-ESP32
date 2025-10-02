<script lang="ts">
	import AppMqtt from '$lib/components/AppMqtt.svelte';

	let { data } = $props();
</script>

<AppMqtt mqtt={data.mqtt} error={data.error} />