include $(LITEOSTOPDIR)/../../drivers/hdf_core/adapter/khdf/liteos/lite.mk
MODULE_NAME := $(notdir $(shell pwd))
LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/../../third_party/lwip/src/include/ \
                 -I $(LITEOSTOPDIR)/../../drivers/hdf_core/framework/include/ \
                 -I $(LITEOSTOPDIR)/../../drivers/hdf_core/framework/model/input/driver/ \
                 -I $(LITEOSTOPDIR)/../../drivers/hdf_core/framework/model/network/wifi/include/ \
                 -I $(LITEOSTOPDIR)/../../drivers/hdf_core/framework/model/network/wifi/platform/include/ \
                 -I $(LITEOSTOPDIR)/../../drivers/hdf_core/framework/model/network/wifi/core/components/eapol/ \
                 -I $(LITEOSTOPDIR)/../../third_party/FreeBSD/sys/dev/evdev/
LOCAL_SRCS := virtnet.c virtmmio.c virtgpu.c virtinput.c virtblock.c fakesdio.c
LOCAL_CFLAGS += $(LOCAL_INCLUDE)

ifdef LOSCFG_HW_RANDOM_ENABLE
LOCAL_SRCS += virtrng.c
endif

include $(HDF_DRIVER)
