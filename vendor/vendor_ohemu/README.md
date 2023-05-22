# vendor_ohos

## Introduction

The repository is mainly developed by OpenHarmony community, rather than the
typical certain manufacturer, mainly including the development of QEMU products
with characteristics such as graphics, soft bus, etc.

## Software Architecture

Graphic display product samples of virt platform supporting QEMU's ARMv7-a
architecture, samples of virt platform supporting RISC-V architecture,
samples of esp32 board supporting Xtensa architecture, samples of virt
platform supporting C-SKY architecture, and samples of Cortex-M4 architecture.

code path:

```
device/qemu/                          --- device_qemu repository path
├── arm_mps2_an386                    --- Cortex-M4 architecture MPS2-AN386 platform
├── arm_virt                          --- ARMv7-a architecture virt platform
├── drivers                           --- virt drivers
├── riscv32_virt                      --- RISC-V architecture virt platform
├── esp32                             --- Xtensa architecture board
└── SmartL_E802                       --- C-SKY architecture virt platform
vendor/ohos/                          --- vendor_ohos repository path
├── qemu_small_system_demo            --- small system default demo
├── qemu_mini_system_demo             --- mini system default demo
├── qemu_riscv_mini_system_demo       --- mini system demo with riscv architecture
├── qemu_xtensa_mini_system_demo      --- mini system demo with xtensa architecture
└── qemu_csky_mini_system_demo        --- mini system demo with c-sky architecture
```

## Installation

[QEMU Install Guide](https://gitee.com/openharmony/device_qemu/blob/HEAD/README.md)

## Usage

Refer to involved repositories documents.

## Contribution

[How to involve](https://gitee.com/openharmony/docs/blob/HEAD/en/contribute/contribution.md)

[Commit message spec](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## Repositories Involved

[device\_qemu](https://gitee.com/openharmony/device_qemu/blob/HEAD/README.md)

