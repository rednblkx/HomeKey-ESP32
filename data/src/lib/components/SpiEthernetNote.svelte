<script lang="ts">
  let { multipleSpi = false, selectedBus = 1 } = $props<{
    multipleSpi?: boolean;
    selectedBus?: number;
  }>();
</script>

{#if multipleSpi}
  <div class="form-control">
    <!-- svelte-ignore a11y_label_has_associated_control -->
    <label class="label">
      <span class="label-text block whitespace-normal break-words max-w-sm">
        Note: Your chip model supports two general purpose SPI buses and
        Ethernet is assigned to the {selectedBus === 1 ? "first" : "second"} bus.
        Ensure that the PN532 pins {selectedBus === 1
          ? "match"
          : "do not conflict with"} the Ethernet pins, as they will {selectedBus ===
        1
          ? "share the same bus."
          : "be initialized on separate buses."}
      </span>
    </label>
  </div>
{/if}
