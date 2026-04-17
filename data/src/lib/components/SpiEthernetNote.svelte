<script lang="ts">
  let { spiNumBuses = 1, selectedBus = 1 } = $props<{
    spiNumBuses: number;
    selectedBus: number;
  }>();
</script>
<div class="alert alert-warning alert-sm mb-2">
  <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0 stroke-current" fill="none" viewBox="0 0 24 24">
    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
  </svg>
  <!-- svelte-ignore a11y_label_has_associated_control -->
  <span class="text-xs">
    Note: {spiNumBuses == 1 ? "Only one SPI bus available" : "Two SPI buses available"} and Ethernet is assigned to the
    {selectedBus === 1
      ? "first (SPI2) bus."
      : "second (SPI3) bus."}
    <br />
    Ensure that the PN532 pins {selectedBus === 1
      ? "(except CS/SS) match"
      : "do not conflict with"}
    the Ethernet pins as they will {selectedBus === 1
      ? "share the same bus."
      : "be initialized on separate buses."}
  </span>
</div>
