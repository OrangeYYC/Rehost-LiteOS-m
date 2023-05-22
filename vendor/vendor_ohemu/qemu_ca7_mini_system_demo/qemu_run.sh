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
flash_name=ca7_mini.img
vnc="-vnc :20  -serial mon:stdio"
src_dir=out/arm_virt/qemu_ca7_mini_system_demo
bootargs=$(cat <<-END
bootargs=root=cfi-flash fstype=jffs2 rootaddr=10M rootsize=27M useraddr=37M usersize=27M
END
)

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

help_info=$(cat <<-END
Usage: qemu-run [OPTION]...
Make a qemu image($flash_name) for OHOS, and run the image in qemu according
to the options.

    Options:

    -f, --force                    rebuild ${flash_name}
    -n, --net-enable               enable net
    -l, --local-desktop            no VNC
    -b, --bootargs boot_arguments  additional boot arguments(-bk1=v1,k2=v2...)
    -g,  --gdb                     enable gdb for kernel
    -h, --help                     print help info

    By default, ${flash_name} will not be rebuilt if exists, and net will not
    be enabled, gpu enabled and waiting for VNC connection at port 5920.
END
)

if [ "$qemu_help" = "yes" ]; then
    echo "${help_info}"
    exit 0
fi

if [ "$vnc_enable" = "no" ]; then
    vnc=""
fi

if [ "$add_boot_args" = "yes" ]; then
    bootargs+=" "${boot_args//","/" "}
fi

if [ "$gdb_enable" = "yes" ]; then
    qemu_option+="-s -S"
fi

function unsupported_parameters_check(){
    if [ "$qemu_test" = "test" ]; then
        echo "Error: The -t|--test option is not supported !"
        echo "${help_info}"
        exit 1
    fi
}

function make_flash(){
    echo -e "\nStart making ${flash_name}..."
    dd if=/dev/zero of=${flash_name} bs=64M count=1
    dd if=${src_dir}/OHOS_Image.bin of=${flash_name} conv=notrunc seek=0 oflag=seek_bytes
    echo -e "Succeed making ${flash_name}.\n"
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
    if [[ "${vnc}" == "-vnc "* ]]; then
        echo -e "Waiting VNC connection on: 5920 ...(Ctrl-C exit)"
    fi
    if [ ${net_enable} = yes ]
    then
        net_config 2>/dev/null
        sudo `which qemu-system-arm` -M virt,gic-version=2,secure=on -cpu cortex-a7 -smp cpus=1 -m 1G -drive \
        if=pflash,file=./${flash_name},format=raw -global virtio-mmio.force-legacy=false -netdev bridge,id=net0 \
        -device virtio-net-device,netdev=net0,mac=12:22:33:44:55:66 \
        -device virtio-gpu-device,xres=960,yres=480 -device virtio-tablet-device ${vnc} $qemu_option \
        -device virtio-rng-device
    else
        `which qemu-system-arm` -M virt,gic-version=2,secure=on -cpu cortex-a7 -smp cpus=1 -m 1G -drive \
        if=pflash,file=./${flash_name},format=raw -global virtio-mmio.force-legacy=false \
        -device virtio-gpu-device,xres=960,yres=480 -device virtio-tablet-device ${vnc} $qemu_option \
        -device virtio-rng-device
    fi
}

unsupported_parameters_check

if [ ! -f "${flash_name}" ] || [ ${rebuild_image} = yes ]; then
    make_flash ${flash_name} "${bootargs}" ${src_dir}
elif [ ${add_boot_args} = yes ]; then
    echo "Update bootargs..."
    echo -e "${bootargs}"'\0' > ${src_dir}/bootargs
    dd if=${src_dir}/bootargs of=${flash_name} conv=notrunc seek=9984k oflag=seek_bytes
fi
start_qemu ${net_enable}
