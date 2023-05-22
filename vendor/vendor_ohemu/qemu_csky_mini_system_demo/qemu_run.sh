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

qemu_option=""

if [ "$elf_file" = "Invalid" ]; then
    elf_file=out/SmartL_E802/qemu_csky_mini_system_demo/OHOS_Image
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

function start_qemu(){
    qemu-system-cskyv2 -machine smartl -nographic -kernel $elf_file $qemu_option
}

unsupported_parameters_check

start_qemu
