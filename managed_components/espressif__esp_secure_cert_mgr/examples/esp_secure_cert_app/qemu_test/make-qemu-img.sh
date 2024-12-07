#!/usr/bin/env bash
set -e
arg_esp_secucertimg=$1
arg_partition_table=$2
arg_projpath="$3"
arg_projname="$4"
arg_flashimg=$5

if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
    echo "Usage: make-qemu-img.sh esp_secure_cert_partition partition_table path_to_build_dir project_name image_name"
    exit 1
fi

dd if=/dev/zero bs=1024 count=4096 of=${arg_flashimg}
dd if=${arg_projpath}/build/bootloader/bootloader.bin bs=1 seek=$((0x1000)) of=${arg_flashimg} conv=notrunc
dd if=${arg_esp_secucertimg} bs=1 seek=$((0xD000)) of=${arg_flashimg} conv=notrunc
dd if=${arg_partition_table} bs=1 seek=$((0xC000)) of=${arg_flashimg} conv=notrunc
dd if=${arg_projpath}/build/${arg_projname}.bin bs=1 seek=$((0x20000)) of=${arg_flashimg} conv=notrunc
