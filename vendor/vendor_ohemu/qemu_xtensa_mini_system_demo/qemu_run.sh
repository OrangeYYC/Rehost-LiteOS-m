#!/bin/bash

#Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

set -e

elf_file=$1
rebuild_image=$2
vnc_enable=$3
add_boot_args=$4
boot_args=$5
net_enable=$6
gdb_enable=$7
qemu_test=$8
test_file=$9
qemu_help=${10}


if [ "$elf_file" = "Invalid" ]; then
    elf_file=out/esp32/qemu_xtensa_mini_system_demo/OHOS_Image
fi

help_info=$(cat <<-END
Usage: qemu-run [OPTION]...
Run a OHOS image in qemu according to the options.

    Options:

    -e,  --exec file_name     kernel exec file name
    -g,  --gdb                enable gdb for kernel
    -h,  --help               print help info

    By default, the kernel exec file is: ${elf_file}.
END
)

if [ "$qemu_help" = "yes" ]; then
    echo "${help_info}"
    exit 0
fi

if [ "$gdb_enable" = "yes" ]; then
    qemu_option+="-s -S"
fi

function unsupported_parameters_check(){
    if [ "$rebuild_image" = "yes" ]; then
        echo "Error: The -f|--force option is not supported !"
        echo "${help_info}"
        exit 1
    fi

    if [ "$vnc_enable" = "no" ]; then
        echo "Error: The -l|--local-desktop option is not supported !"
        echo "${help_info}"
        exit 1
    fi

    if [ "$add_boot_args" = "yes" ]; then
        echo "Error: The -b|--bootargs option is not supported !"
        echo "${help_info}"
        exit 1
    fi

    if [ "$net_enable" = "yes" ]; then
        echo "Error: The -n|--net-enable option is not supported !"
        echo "${help_info}"
        exit 1
    fi

    if [ "$qemu_test" = "test" ]; then
        echo "Error: The -t|--qemu-test option is not supported !"
        echo "${help_info}"
        exit 1
    fi
}

function merge_bin() {
    out_dir=out/esp32/qemu_xtensa_mini_system_demo
    esptool.py --chip esp32 elf2image --flash_mode dio --flash_freq 80m                 \
    --flash_size 4MB -o $out_dir/liteos.bin $elf_file

    esptool.py --chip esp32 merge_bin --fill-flash-size 4MB                             \
    -o $out_dir/liteos_merge.bin                                                        \
    0x8000 vendor/ohemu/qemu_xtensa_mini_system_demo/image/partition-table.bin          \
    0x1000 vendor/ohemu/qemu_xtensa_mini_system_demo/image/bootloader.bin               \
    0x10000 $out_dir/liteos.bin

    image_file=$out_dir/liteos_merge.bin
}

function start_qemu(){
    esptool_version=`esptool.py version | sed -n '2p'`
    if [ "$esptool_version" \< "3.1" ]; then
        echo "Error: The esptool.py version is too low"
        exit 1
    fi
    merge_bin
    export QEMU_XTENSA_CORE_REGS_ONLY=1
    $QEMU/qemu-system-xtensa -nographic $qemu_option -machine esp32                                  \
    -drive file=$image_file,if=mtd,format=raw
}


unsupported_parameters_check

start_qemu
