Import("env")
from os.path import join

board = env.BoardConfig()
mcu = board.get("build.mcu", "esp32")

target_firm = env.DataToBin(
    join("$BUILD_DIR", "${ESP32_FS_IMAGE_NAME}"), "$PROJECT_DATA_DIR"
)
env.NoCache(target_firm)
AlwaysBuild(target_firm)

def before_upload(source, target, env):
    env.Append(UPLOADERFLAGS=['$FS_START', join('$BUILD_DIR','${ESP32_FS_IMAGE_NAME}.bin')])


def after_build(source, target, env):
  env.Execute(f'"$PYTHONEXE" "$OBJCOPY" --chip {mcu} merge_bin --fill-flash-size 4MB -o {join(env.subst("$BUILD_DIR"), "firmware_merged.bin")} $FLASH_EXTRA_IMAGES $ESP32_APP_OFFSET {join(env.subst('$BUILD_DIR'), '${PROGNAME}.bin')} $FS_START {join(env.subst('$BUILD_DIR'),'${ESP32_FS_IMAGE_NAME}.bin')}')

env.Depends(join('$BUILD_DIR', '${PROGNAME}.bin'), target_firm)
env.Depends("upload", target_firm)
env.AddPreAction("upload", before_upload)
env.AddPostAction(join('$BUILD_DIR', '${PROGNAME}.bin'), after_build)