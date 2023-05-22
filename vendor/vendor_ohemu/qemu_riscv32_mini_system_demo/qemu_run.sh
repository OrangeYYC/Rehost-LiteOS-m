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

vnc="-vnc :20  -serial mon:stdio"
qemu_option=""
fsimg="vendor/ohemu/qemu_riscv32_mini_system_demo/fs-storage.img"

#### Submit code to CI test command, do not modify #####
if [ "$qemu_test" = "test" ]; then
    qemu_option+="-serial file:$test_file"
fi

if [ "$elf_file" = "Invalid" ]; then
    elf_file=out/riscv32_virt/qemu_riscv_mini_system_demo/OHOS_Image
fi

help_info=$(cat <<-END
Usage: qemu-run [OPTION]...
Run a OHOS image in qemu according to the options.

    Options:

    -e,  --exec file_name     kernel exec file name
    -n,  --net-enable         enable net
    -l,  --local-desktop      no VNC
    -g,  --gdb                enable gdb for kernel
    -t,  --test               test mode, exclusive with -g
    -h,  --help               print help info

    By default, the kernel exec file is: ${elf_file}.
END
)

if [ "$qemu_help" = "yes" ]; then
    echo "${help_info}"
    exit 0
fi

if [ "$vnc_enable" = "no" ]; then
    vnc=""
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

    if [ "$add_boot_args" = "yes" ]; then
        echo "Error: The -b|--bootargs option is not supported !"
        echo "${help_info}"
        exit 1
    fi
}

function net_config(){
    echo "Network config..."
    set +e
    sudo modprobe tun tap
    sudo ip link add br0 type bridge
    sudo ip address add 10.0.2.2/24 dev br0
    sudo ip link set dev br0 up
    set -e
}

function start_qemu(){
    net_enable=${1}
    if [ ${net_enable} = yes ]; then
        net_config 2>/dev/null
        sudo `which qemu-system-riscv32` -M virt -m 128M -bios none -kernel $elf_file \
        -global virtio-mmio.force-legacy=false \
        $qemu_option \
        -netdev bridge,id=net0 \
        -device virtio-net-device,netdev=net0,mac=12:22:33:44:55:66 \
        -device virtio-gpu-device,xres=800,yres=480 -device virtio-tablet-device ${vnc} \
        -drive if=pflash,file=$fsimg,format=raw,index=1 \
        -append "root=/dev/vda or console=ttyS0"
    else
        qemu-system-riscv32 -M virt -m 128M -bios none -kernel $elf_file \
        -global virtio-mmio.force-legacy=false \
        $qemu_option \
        -device virtio-gpu-device,xres=800,yres=480 -device virtio-tablet-device ${vnc} \
        -drive if=pflash,file=$fsimg,format=raw,index=1 \
        -append "root=/dev/vda or console=ttyS0"
    fi
}

unsupported_parameters_check

start_qemu ${net_enable}
