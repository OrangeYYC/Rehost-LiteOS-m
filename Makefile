INSTRUMENT_CC = gcc -g
# CC = gcc -fsanitize=address -g -masm=intel
CC =  gcc -fsanitize=address -g -masm=intel

CFLAGS += \
-DLOSCFG_COMPILER_GCC=1 \
-DLOSCFG_COMPILER_ARM_NONE_EABI=1 \
-DLOSCFG_COMPILER_OPTIMIZE_NONE=1 \
-DLOSCFG_PLATFORM="Cloud_STM32F429IGTx_FIRE" \
-DLOSCFG_PLATFORM_STM32F429IGTX=1 \
-DLOSCFG_USING_BOARD_LD=1 \
-DLOSCFG_PLATFORM_BSP_NVIC=1 \
-DLOSCFG_ARCH_ARM_AARCH32=1 \
-DLOSCFG_ARCH_ARM_CORTEX_M=1 \
-DLOSCFG_ARCH_ARM_V7M=1 \
-DLOSCFG_ARCH_ARM_VER="armv7-m" \
-DLOSCFG_ARCH_FPU_VFP_V4=1 \
-DLOSCFG_ARCH_FPU_VFP_D16=1 \
-DLOSCFG_ARCH_FPU="fpv4-sp-d16" \
-DLOSCFG_ARCH_CORTEX_M4=1 \
-DLOSCFG_ARCH_CPU="cortex-m4" \
-DLOSCFG_SCHED_SQ=1 \
-DLOSCFG_BASE_CORE_TIMESLICE=1 \
-DLOSCFG_OBSOLETE_API=1 \
-DLOSCFG_BASE_CORE_TSK_MONITOR=1 \
-DLOSCFG_TASK_MIN_STACK_SIZE=1024 \
-DLOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE=1536 \
-DLOSCFG_BASE_CORE_TSK_DEFAULT_PRIO=10 \
-DLOSCFG_BASE_CORE_USE_MULTI_LIST=1 \
-DLOSCFG_KERNEL_MEM_BESTFIT_LITTLE=1 \
-DLOSCFG_HWI_PRIO_LIMIT=32 \
-DLOSCFG_BASE_IPC_QUEUE=1 \
-DLOSCFG_BASE_IPC_EVENT=1 \
-DLOSCFG_BASE_IPC_EVENT_LIMIT=10 \
-DLOSCFG_BASE_IPC_MUX=1 \
-DLOSCFG_MUTEX_WAITMODE_PRIO=1 \
-DLOSCFG_BASE_IPC_SEM=1 \
-DLOSCFG_DEMOS_AGENT_TINY_LWM2M=1 \
-DLWM2M_CLIENT_MODE=1 \
-DLWM2M_LITTLE_ENDIAN=1 \
-DLWM2M_SUPPORT_JSON=1 \
-DLOSCFG_COMPONENTS_CONNECTIVITY=1 \
-DLOSCFG_COMPONENTS_SECURITY=1 \
-DLOSCFG_COMPILE_DEBUG=1 \
-DLOSCFG_PLATFORM_OSAPPINIT=1 \
-DLOSCFG_CC_NO_STACKPROTECTOR=1 \
-DMBEDTLS_CONFIG_FILE=\"los_mbedtls_config.h\" \
-Dhidden="__attribute ((visibility(\"default\")))" \
-D__LITEOS__ -DSECUREC_IN_KERNEL=0 -D_ALL_SOURCE -DLOSCFG_ARCH_FPU_DISABLE \
-DLOSCFG_KERNEL_CPP_EXCEPTIONS_SUPPORT -DUSE_HAL_DRIVER -DTESTSTM32F429IGTX -DSTM32F429xx \
-DPACK_CHECKSUM=PACK_NO_CHECKSUM \
-m32 \
-Wall -Werror -std=c99 \
-Wno-comment \
-Wno-unused-function \
-Wno-unused-variable \
-fno-builtin \
-Wno-parentheses-equality \
-Wno-typedef-redefinition \

LDFLAGS += \
-DLOSCFG_COMPILER_GCC=1 \
-DLOSCFG_COMPILER_ARM_NONE_EABI=1 \
-DLOSCFG_COMPILER_OPTIMIZE_NONE=1 \
-DLOSCFG_PLATFORM="Cloud_STM32F429IGTx_FIRE" \
-DLOSCFG_PLATFORM_STM32F429IGTX=1 \
-DLOSCFG_USING_BOARD_LD=1 \
-DLOSCFG_PLATFORM_BSP_NVIC=1 \
-DLOSCFG_ARCH_ARM_AARCH32=1 \
-DLOSCFG_ARCH_ARM_CORTEX_M=1 \
-DLOSCFG_ARCH_ARM_V7M=1 \
-DLOSCFG_ARCH_ARM_VER="armv7-m" \
-DLOSCFG_ARCH_FPU_VFP_V4=1 \
-DLOSCFG_ARCH_FPU_VFP_D16=1 \
-DLOSCFG_ARCH_FPU="fpv4-sp-d16" \
-DLOSCFG_ARCH_CORTEX_M4=1 \
-DLOSCFG_ARCH_CPU="cortex-m4" \
-DLOSCFG_ARCH_FPU_DISABLE=1 \
-DLOSCFG_SCHED_SQ=1 \
-DLOSCFG_BASE_CORE_TIMESLICE=1 \
-DLOSCFG_BASE_CORE_TIMESLICE_TIMEOUT=2 \
-DLOSCFG_BACKTRACE_TYPE=0 \
-DLOSCFG_OBSOLETE_API=1 \
-DLOSCFG_BASE_CORE_TSK_MONITOR=1 \
-DLOSCFG_BASE_CORE_TSK_LIMIT=16 \
-DLOSCFG_TASK_MIN_STACK_SIZE=1024 \
-DLOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE=1536 \
-DLOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE=1536 \
-DLOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE=2048 \
-DLOSCFG_BASE_CORE_TSK_DEFAULT_PRIO=10 \
-DLOSCFG_BASE_CORE_USE_MULTI_LIST=1 \
-DLOSCFG_KERNEL_MEM_BESTFIT_LITTLE=1 \
-DLOSCFG_PLATFORM_HWI_LIMIT=256 \
-DLOSCFG_HWI_PRIO_LIMIT=32 \
-DLOSCFG_BASE_IPC_QUEUE=1 \
-DLOSCFG_BASE_IPC_QUEUE_LIMIT=10 \
-DLOSCFG_BASE_IPC_EVENT=1 \
-DLOSCFG_BASE_IPC_EVENT_LIMIT=10 \
-DLOSCFG_BASE_IPC_MUX=1 \
-DLOSCFG_MUTEX_WAITMODE_PRIO=1 \
-DLOSCFG_BASE_IPC_MUX_LIMIT=20 \
-DLOSCFG_BASE_IPC_SEM=1 \
-DLOSCFG_BASE_IPC_SEM_LIMIT=20 \
-DLOSCFG_DEMOS_AGENT_TINY_LWM2M=1 \
-DLWM2M_CLIENT_MODE=1 \
-DLWM2M_LITTLE_ENDIAN=1 \
-DLWM2M_SUPPORT_JSON=1 \
-DLOSCFG_COMPONENTS_CONNECTIVITY=1 \
-DLOSCFG_COMPONENTS_SECURITY=1 \
-DLOSCFG_COMPILE_DEBUG=1 \
-DLOSCFG_PLATFORM_OSAPPINIT=1 \
-DLOSCFG_CC_NO_STACKPROTECTOR=1 \
-DMBEDTLS_CONFIG_FILE=\"los_mbedtls_config.h\" \
-Dhidden="__attribute ((visibility(\"default\")))" \
-D__LITEOS__ -DSECUREC_IN_KERNEL=0 -D_ALL_SOURCE -DLOSCFG_ARCH_FPU_DISABLE \
-DLOSCFG_KERNEL_CPP_EXCEPTIONS_SUPPORT -DUSE_HAL_DRIVER -DTESTSTM32F429IGTX -DSTM32F429xx \
-DPACK_CHECKSUM=PACK_NO_CHECKSUM \
-m32 \
-Wall -Werror -std=c99 \
-Wno-comment \
-Wno-unused-function \
-Wno-unused-variable \
-fno-builtin \
-Wno-parentheses-equality \
-Wno-typedef-redefinition \

INCLUDE_DIRS += \
-I./arch/arm/cortex-m4/gcc \
-I./arch/include \
-I./components/exchook \
-I./components/net/lwip-2.1/porting/include \
-I./components/net/lwip-2.1/porting/include/arch \
-I./components/net/lwip-2.1/porting/include/lwip \
-I./device/qemu/arm_mps2_an386/liteos_m/board \
-I./device/qemu/arm_mps2_an386/liteos_m/board/driver/net \
-I./device/qemu/arm_mps2_an386/liteos_m/board/include \
-I./kal/libc/newlib/porting/include \
-I./kernel/include \
-I./third_party/third_party_bounds_checking_function/include \
-I./third_party/third_party_cmsis/CMSIS/Core/Include \
-I./third_party/third_party_lwip/src/include \
-I./sub \
-I./utils \
-I./mytest


TARGET = liteos
# device: qemu arm-mps2-an386
default: all

all: ${TARGET}

COMMON = \
./arch/arm/cortex-m4/gcc/los_context.c \
./arch/arm/cortex-m4/gcc/los_interrupt.c \
./arch/arm/cortex-m4/gcc/los_mpu.c \
./arch/arm/cortex-m4/gcc/los_timer.c \
$(wildcard ./components/exchook/*.c) \
./device/qemu/arm_mps2_an386/liteos_m/board/main.c \
$(wildcard ./kal/libc/newlib/porting/src/*.c) \
$(wildcard ./kernel/src/mm/*.c) \
$(wildcard ./kernel/src/*.c) \
$(wildcard ./third_party/third_party_bounds_checking_function/src/*.c) \
$(wildcard ./utils/*.c) \
./mytest/mytest_1.c

PORT = \
$(wildcard ./sub/*.c)

COMMON_OBJ = ${COMMON:.c=.o}
${COMMON_OBJ}: %.o: %.c
	${CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@

#OBJ = ${SRC:.c=.o}
#${OBJ}: %.o: %.c
#	${CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@

#INSTRUMENT_OBJ = ${INSTRUMENT:.c=.o}
#${INSTRUMENT_OBJ}: %.o: %.c
#	${INSTRUMENT_CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@

PORT_OBJ = ${PORT:.c=.o}
${PORT_OBJ}: %.o: %.c
	${CC} ${INCLUDE_DIRS} -m32 -c $< -o $@

$(TARGET): ${COMMON_OBJ} ${PORT_OBJ}
	${CC} ${LDFLAGS} -o $@ ${COMMON_OBJ} ${PORT_OBJ} -lm -lpthread

.PHONY : clean
clean:
	-rm ${OBJ} ${COMMON_OBJ} ${PORT_OBJ} ${INSTRUMENT_OBJ}
	-rm *~
	-rm ${TARGET}
	-rm *.o
