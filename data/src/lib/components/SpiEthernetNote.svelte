<script lang="ts">
  export let multipleSpi: boolean = false;
  export let ethernetEnabled: boolean = false;
</script>

{#if ethernetEnabled}
  {#if multipleSpi}
    <div class="form-control">
      <!-- svelte-ignore a11y_label_has_associated_control -->
      <label class="label">
        <span class="label-text block whitespace-normal break-words">Note: Your chip model has multiple user SPI buses and Ethernet is enabled. Ensure that the PN532 pins do not conflict with the Ethernet pins, as they will be initialized on separate buses.</span>
      </label>
    </div>
  {:else}
    <div class="form-control">
      <!-- svelte-ignore a11y_label_has_associated_control -->
      <label class="label">
        <span class="label-text block whitespace-normal break-words">Note: Your chip model has one user SPI bus and Ethernet is enabled. Ensure that the PN532 pins match the Ethernet pins, as they will share the same bus.</span>
      </label>
    </div>
  {/if}
{/if}
