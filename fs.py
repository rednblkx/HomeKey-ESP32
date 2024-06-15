Import("env")
from os.path import join

target_firm = env.DataToBin(
    join("$BUILD_DIR", "${ESP32_FS_IMAGE_NAME}"), "$PROJECT_DATA_DIR"
)
env.NoCache(target_firm)
AlwaysBuild(target_firm)

def before_upload(source, target, env):
    env.Append(UPLOADERFLAGS=['$FS_START', join('$BUILD_DIR','${ESP32_FS_IMAGE_NAME}.bin')])

env.AddPreAction("upload", before_upload)
env.Depends("upload", target_firm)