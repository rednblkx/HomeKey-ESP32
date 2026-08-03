# Core dumps

When the firmware panics, the device stores an ELF core dump in flash and
survives the reboot with it intact. The HTTP API decodes it on-device, so the
common question — *which task crashed, and where* — is answered without
installing anything.

This exists because a board with no reachable serial port otherwise gives you
nothing: the reset reason says `PANIC` and stops there. The rolling log buffer
shows what the device was doing; the core dump shows where it died.

## Enabling it

Core dump support is compiled in, but the partition table has to be written
once, because a 128 KB dump partition does not fit the stock layout. The app
slots shrink from 1920 KB to 1856 KB to make room:

```text
nvs       0x009000..0x019000    64 K   unchanged
otadata   0x019000..0x01B000     8 K   unchanged
app0      0x020000..0x1F0000  1856 K   unchanged offset, 64 K smaller
app1      0x1F0000..0x3C0000  1856 K   moved
coredump  0x3C0000..0x3E0000   128 K   new
spiffs    0x3E0000..0x400000   128 K   unchanged
```

This particular split was chosen so that `nvs`, `otadata` and `spiffs` all keep
their existing offsets -- only `app1` moves. **HomeKit pairing, HomeKey reader
credentials and the web assets are therefore untouched**, and there is nothing
to back up and restore.

Application headroom drops from about 10% to 7% as a result, which is the real
cost of this feature.

Flash the changed regions only, leaving NVS alone:

```bash
esptool.py --chip esp32s3 -p /dev/cu.usbmodem* erase_region 0x3C0000 0x20000
esptool.py --chip esp32s3 -p /dev/cu.usbmodem* write_flash \
    0x0      build/bootloader/bootloader.bin \
    0x8000   build/partition_table/partition-table.bin \
    0x19000  build/ota_data_initial.bin \
    0x20000  build/HomeKey-ESP32.bin
```

The erase removes data left by the old `app1` layout. It is required because
unread core dumps are deliberately not overwritten.

`0x9000` (NVS) is deliberately absent. Do **not** flash a merged image here:
merged images pad the gaps with `0xFF` and would erase NVS, taking the pairing
with it.

Belt and braces, before you start:

```bash
esptool.py --chip esp32s3 -p /dev/cu.usbmodem* read_flash 0x9000 0x10000 nvs-backup.bin
# restore, only if something goes wrong:
# esptool.py --chip esp32s3 -p /dev/cu.usbmodem* write_flash 0x9000 nvs-backup.bin
```

After that, firmware updates continue over OTA as before.

## Reading a dump

Over HTTP — the summary is already decoded, so this is usually all you need:

```bash
curl -s -u webUsername:webPassword http://<device>/coredump/info | jq
```

Omit `-u webUsername:webPassword` when web authentication is disabled.

```json
{
  "supported": true,
  "available": true,
  "size": 8192,
  "valid": true,
  "panicReason": "StoreProhibited",
  "task": "nfc_poll",
  "pc": "0x420a1c34",
  "appElfSha256": "3f2a...",
  "backtrace": ["0x420a1c34", "0x420a0f18", "0x4038b2a0"],
  "backtraceCorrupted": false,
  "runningVersion": "dev-31-gabc1234"
}
```

`backtrace` is present on Xtensa targets (ESP32, ESP32-S3). RISC-V targets
(ESP32-C3, C6) record a raw stack dump instead of an unwound backtrace, so those
need the offline path below.

## Turning addresses into file:line

The addresses mean nothing on their own. Resolve them against **the exact ELF
that was running when it crashed** — check `appElfSha256` against that build.

```bash
xtensa-esp32s3-elf-addr2line -pfiaC -e build/HomeKey-ESP32.elf \
    0x420a1c34 0x420a0f18 0x4038b2a0
```

Use `riscv32-esp-elf-addr2line` on C3/C6. Both are on `PATH` after
`. $IDF_PATH/export.sh`.

## Full offline decode

For a complete view — all task backtraces, registers, memory regions — download
the raw image and decode it:

```bash
curl -u webUsername:webPassword -o coredump.elf http://<device>/coredump
esp-coredump info_corefile -c coredump.elf -t elf build/HomeKey-ESP32.elf
```

`esp-coredump` ships with ESP-IDF. `dbg_corefile` instead of `info_corefile`
drops you into GDB against the crash state.

## Clearing it

The first dump is kept and later panics are **not** recorded until it is
cleared — the first crash is usually the informative one, and without this a
reboot loop would immediately bury it.

```bash
curl -u webUsername:webPassword -X POST http://<device>/coredump/erase
```

## Caveats

- The partition is 128 KB with `CONFIG_ESP_COREDUMP_MAX_TASKS_NUM` at 16. ESP-IDF
  calculates the complete image size before writing it; if that image exceeds
  the partition, it writes no new dump. Keep the task count and stack sizes in
  mind when adding long-lived tasks.
- A dump whose checksum fails remains available from `/coredump`, but
  `/coredump/info` does not attempt to decode corrupt ELF data.
- `appElfSha256` is worth checking every time. Symbolicating against a rebuilt
  ELF produces plausible, wrong answers.
- There is no web UI panel for this yet, only the HTTP API above. A panel
  belongs next to the rolling log buffer in the Logs view; both touch the same
  component, so it is left to whichever of the two changes lands second rather
  than creating an avoidable merge conflict.
