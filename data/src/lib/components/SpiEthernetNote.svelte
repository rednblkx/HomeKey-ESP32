<script lang="ts">
  let { spiNumBuses = 1, selectedBus = 1 } = $props<{
    spiNumBuses: number;
    selectedBus: number;
  }>();
</script>

<div class="form-control mb-2">
  <!-- svelte-ignore a11y_label_has_associated_control -->
  <label class="label">
    <span class="label-text block whitespace-normal wrap-break-words">
      Note: Your chip model {spiNumBuses === 1
        ? "only supports one"
        : "supports two"} general purpose SPI {spiNumBuses === 1
        ? "bus"
        : "buses"}
      {spiNumBuses > 1 ? "and Ethernet is assigned to the" : "."}
      {spiNumBuses > 1
        ? selectedBus === 1
          ? "SPI2 bus."
          : "SPI3 bus."
        : ""}
      <br />
      Ensure that the PN532 pins {selectedBus === 1
        ? "(except CS/SS) match"
        : "do not conflict with"}
      the Ethernet pins as they will {selectedBus === 1
        ? "share the same bus."
        : "be initialized on separate buses."}
    </span>
  </label>
</div>
