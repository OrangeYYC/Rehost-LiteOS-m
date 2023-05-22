### Qemu Arm Cortex-m4 mps2-an386 HOWTO

#### 1. Brief introduction
`arm_mps2_an386/` subdirectory contains part of the OpenHarmony LiteOS demonstration support for Qemu Arm Cortex-m4 mps2-an386 Platform,
here called *arm_mps2_an386*.
cortex-m4 Virtual platform is a `qemu-system-arm` machine target that provides emulation
for a generic, arm-based board.

Introduced functionality adds support for Cortex-m4 (1 CPU with security extensions), 16MB memory virtual platform.

Note: System memory size is hard-coded to 16MB.

#### 2. Setting up environment

[Setting up a development environment](https://gitee.com/openharmony/docs/blob/HEAD/en/device-dev/quick-start/quickstart-lite-env-setup.md)

Compiler install

1.Command to install

Note: Command to install toolchain without arm-none-eabi-gdb, gdb cannot be debugged.

```
sudo apt install gcc-arm-none-eabi
```

2.The installation package to install

Note: If you have already passed the command to install gcc-arm-none-eabi, can through the command: `sudo apt remove
gcc-arm-none-eabi` after unloading, install again.

Download toolchain: [package](https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2)。

```
chmod 777 gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2
tar -xvf gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 install_path
```

Add the installation path to the environment variable:

```
vim ~/.bashrc
```

Add the following command line to the last line of ~/.bashrc:

```
export PATH=$PATH:install_path/gcc-arm-none-eabi-6-2017-q2-update/bin
```

#### 3. Code acquisition

[Code acquisition](https://gitee.com/openharmony/docs/blob/HEAD/en/device-dev/get-code/sourcecode-acquire.md)

Note: One can use `repo` to fetch code in a straightforward manner.

#### 4. Building from sources

In the root directory of the obtained source code, run the following command:

```
hb set
```

Select `qemu_mini_system_demo` under **ohemu**.

Run the following build command:
```
hb build -f
```

This will build `OHOS_Image` for Qemu Cortex-m4 mps2-an386 machine.


After build is finished, the resulting image can be found in:
```
out/arm_mps2_an386/qemu_mini_system_demo/
```
#### 5. Running image in Qemu

a) If not installed, please install `qemu-system-arm`
For details, please refer to the HOWTO: [Qemu installation](https://gitee.com/openharmony/device_qemu/blob/HEAD/README.md)

b) Run

Run the `./qemu-run --help` command. The following information is displayed:

```
Usage: qemu-run [OPTION]...
Run a OHOS image in qemu according to the options.

    Options:

    -e, --exec file_name     kernel exec file name
    -n, --net-enable         enable net
    -g, --gdb                enable gdb for kernel
    -t, --test               test mode, exclusive with -g
    -h, --help               print help info

    By default, the kernel exec file is: out/arm_mps2_an386/qemu_mini_system_demo/OHOS_Image.
```

#### 6. gdb debug

```
cd device/qemu/arm_mps2_an386
vim liteos_m/config.gni
```

In the modified `board_opt_flags` compiler options:

```
board_opt_flags = []
```
to:

```
board_opt_flags = [ "-g" ]
```

Save and exit, recompile under OHOS root directory:

```
hb build -f
```

In a window to enter the command:

```
./qemu-run -g
```

In another window to enter the command:

```
arm-none-eabi-gdb out/arm_mps2_an386/qemu_mini_system_demo/OHOS_Image
(gdb) target remote localhost:1234
(gdb) b main
```

More GDB related debugging can refer to [GDB instruction manual](https://sourceware.org/gdb/current/onlinedocs/gdb).
