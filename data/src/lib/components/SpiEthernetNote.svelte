<script lang="ts">
  let { multipleSpi = false, ethernetEnabled = false } = $props<{
    multipleSpi?: boolean;
    ethernetEnabled?: boolean;
  }>();
</script>

{#if ethernetEnabled}
  {#if multipleSpi}
    <div class="form-control">
      <!-- svelte-ignore a11y_label_has_associated_control -->
      <label class="label">
        <span class="label-text block whitespace-normal break-words max-w-sm">
          Note: Your chip model supports two general purpose SPI buses and
          Ethernet will be assigned to the second bus. Ensure that the PN532
          pins do not conflict with the Ethernet pins, as they will be
          initialized on separate buses.
        </span>
      </label>
    </div>
  {:else}
    <div class="form-control">
      <!-- svelte-ignore a11y_label_has_associated_control -->
      <label class="label">
        <span class="label-text block whitespace-normal break-words max-w-sm">
          Note: Your chip model only supports one general purpose SPI bus.
          Ensure that the PN532 pins match the Ethernet pins, as they will share
          the same bus.
        </span>
      </label>
    </div>
  {/if}
{/if}
