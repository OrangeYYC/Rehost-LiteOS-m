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
flash_name=out/smallmmc.img
vnc="-vnc :20  -serial mon:stdio"
src_dir=out/arm_virt/qemu_small_system_demo/
bootargs=$(cat <<-END
bootargs=root=emmc fstype=vfat rootaddr=10M rootsize=20M useraddr=30M usersize=50M
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
Make a qemu image for OHOS, and run the image in qemu according
to the options.

    Options:

    -f, --force                    rebuild image
    -n, --net-enable               enable net
    -l, --local-desktop            no VNC
    -b, --bootargs boot_arguments  additional boot arguments(-bk1=v1,k2=v2...)
    -g,  --gdb                     enable gdb for kernel
    -h, --help                     print help info

    By default, image will not be rebuilt if exists, and net will not
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

function check_mmc_tools(){
    type parted > /dev/null
}

function make_mmc(){
    echo -ne "\nStart making ${flash_name}..."
    if [ -f ${flash_name} ]; then
        existed=true
    else
        existed=false
    fi

    # Create raw "disk" with type 0C (FAT32 LBA) according to los_rootfs.h
    if [ $existed == false ]; then
        dd if=/dev/zero of=${flash_name} bs=100M count=1
    fi
    sudo losetup /dev/loop590 ${flash_name}
    if [ $existed == false  ]; then
        sudo parted -s /dev/loop590 -- mklabel msdos mkpart primary fat32 10MiB 30MiB \
            mkpart primary fat32 30MiB 80MiB mkpart primary fat32 80MiB -1s
    fi

    sudo losetup -o 10MiB /dev/loop591 /dev/loop590
    sudo losetup -o 30MiB /dev/loop592 /dev/loop590 # do not touch partition 3 contents
    if [ $existed == false  ]; then
        sudo losetup -o 80MiB /dev/loop593 /dev/loop590
        sudo mkfs.vfat /dev/loop591 > /dev/null
        sudo mkfs.vfat /dev/loop592 > /dev/null
        sudo mkfs.vfat /dev/loop593 > /dev/null
        sudo losetup -d /dev/loop593
    fi

    # Copy files.
    sudo mount /dev/loop591 /mnt
    sudo cp -r -f ${src_dir}/rootfs/* /mnt
    sudo umount /mnt
    sudo mount /dev/loop592 /mnt
    # redundant binaries would overflow disk
    rm -rf ${src_dir}/userfs/test/unittest/*/unstripped
    sudo cp -r -f ${src_dir}/userfs/* /mnt
    sudo umount /mnt

    # Clean.
    sudo losetup -d /dev/loop592
    sudo losetup -d /dev/loop591
    sudo losetup -d /dev/loop590

    # Write bootargs.
    echo -ne "${bootargs}"'\0' > ${src_dir}/bootargs
    dd if=${src_dir}/bootargs of=${flash_name} conv=notrunc seek=512k oflag=seek_bytes

    echo -e "done.\n"
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
        sudo `which qemu-system-arm` -M virt,gic-version=2,secure=on -cpu cortex-a7 -smp cpus=1 -m 1G \
        -bios ${src_dir}/OHOS_Image.bin -global virtio-mmio.force-legacy=false -netdev bridge,id=net0 \
        -device virtio-net-device,netdev=net0,mac=12:22:33:44:55:66 \
        -device virtio-gpu-device,xres=960,yres=480 -device virtio-tablet-device ${vnc} $qemu_option \
        -drive if=none,file=${flash_name},format=raw,id=mmc -device virtio-blk-device,drive=mmc \
        -device virtio-rng-device
    else
        `which qemu-system-arm` -M virt,gic-version=2,secure=on -cpu cortex-a7 -smp cpus=1 -m 1G \
        -bios ${src_dir}/OHOS_Image.bin -global virtio-mmio.force-legacy=false \
        -device virtio-gpu-device,xres=960,yres=480 -device virtio-tablet-device ${vnc} $qemu_option \
        -drive if=none,file=${flash_name},format=raw,id=mmc -device virtio-blk-device,drive=mmc \
        -device virtio-rng-device
    fi
}

unsupported_parameters_check

if [ ! -f ${flash_name} ] || [ ${rebuild_image} = yes ]; then
    check_mmc_tools
    make_mmc
elif [ ${add_boot_args} = yes ]; then
    echo "Update bootargs..."
    echo -e "${bootargs}"'\0' > ${src_dir}/bootargs
    dd if=${src_dir}/bootargs of=${flash_name} conv=notrunc seek=512k oflag=seek_bytes
fi
start_qemu ${net_enable}
