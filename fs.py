Import("env")
from os.path import isfile, join

board = env.BoardConfig()

def _parse_size(value):
    if isinstance(value, int):
        return value
    elif value.isdigit():
        return int(value)
    elif value.startswith("0x"):
        return int(value, 16)
    elif value[-1].upper() in ("K", "M"):
        base = 1024 if value[-1].upper() == "K" else 1024 * 1024
        return int(value[:-1]) * base
    return value

def _parse_partitions(env):
    partitions_csv = env.subst("$PARTITIONS_TABLE_CSV")
    if not isfile(partitions_csv):
        sys.stderr.write("Could not find the file %s with partitions "
                         "table.\n" % partitions_csv)
        env.Exit(1)
        return

    result = []
    # The first offset is 0x9000 because partition table is flashed to 0x8000 and
    # occupies an entire flash sector, which size is 0x1000
    next_offset = 0x9000
    with open(partitions_csv) as fp:
        for line in fp.readlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            tokens = [t.strip() for t in line.split(",")]
            if len(tokens) < 5:
                continue

            bound = 0x10000 if tokens[1] in ("0", "app") else 4
            calculated_offset = (next_offset + bound - 1) & ~(bound - 1)
            partition = {
                "name": tokens[0],
                "type": tokens[1],
                "subtype": tokens[2],
                "offset": tokens[3] or calculated_offset,
                "size": tokens[4],
                "flags": tokens[5] if len(tokens) > 5 else None
            }
            result.append(partition)
            next_offset = _parse_size(partition["offset"]) + _parse_size(
                partition["size"]
            )

    return result
def before_upload(source, target, env):
  for p in _parse_partitions(env):
    if p["type"] == "data" and p["subtype"] in ("spiffs", "fat"):
      print(p)
      env["FS_START"] = _parse_size(p["offset"])
      env["FS_SIZE"] = _parse_size(p["size"])
      env["FS_PAGE"] = int("0x100", 16)
      env["FS_BLOCK"] = int("0x1000", 16)
  
  env.Execute(f'$MKFSTOOL -c $PROJECT_DATA_DIR -s $FS_SIZE -p $FS_PAGE -b $FS_BLOCK $BUILD_DIR/$ESP32_FS_IMAGE_NAME\.bin')

  env.Append(UPLOADERFLAGS=['$FS_START', '$BUILD_DIR/$ESP32_FS_IMAGE_NAME\.bin'])

env.AddPreAction("upload", before_upload)
