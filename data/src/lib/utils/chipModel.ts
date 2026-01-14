export function getChipModelString(model: number): string {
  switch (model) {
    case 1:
      return 'ESP32';
    case 2:
      return 'ESP32-S2';
    case 9:
      return 'ESP32-S3';
    case 5:
      return 'ESP32-C3';
    case 12:
      return 'ESP32-C2';
    case 13:
      return 'ESP32-C6';
    case 16:
      return 'ESP32-H2';
    case 18:
      return 'ESP32-P4';
    case 20:
      return 'ESP32-C61';
    case 23:
      return 'ESP32-C5';
    case 25:
      return 'ESP32-H21';
    case 28:
      return 'ESP32-H4';
    case 999:
      return 'POSIX/Linux';
    default:
      return 'Unknown';
  }
}

export function doesChipModelHaveMultipleSPIBuses(model: number): boolean {
  switch (model) {
    // The following list is from chips in the ESP-IDF
    // whose `SOC_SPI_PERIPH_NUM` > 2
    // (which is the condition used in the IDF
    // for determining if the SPI3 bus exists).
    case 1: // ESP32
    case 2: // ESP32-S2
    case 9: // ESP32-S3
    case 18: // ESP32-P4
    case 28: // ESP32-H4
      return true;
    default:
      return false;
  }
}
