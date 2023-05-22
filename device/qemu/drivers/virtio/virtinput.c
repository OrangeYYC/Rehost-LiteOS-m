/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "osal.h"
#include "osal_io.h"
#include "hdf_input_device_manager.h"
#include "hdf_hid_adapter.h"
#include "utils/hdf_workqueue.h"
#include "los_vm_iomap.h"
#include "virtmmio.h"

#define VIRTQ_EVENT_QSZ     8
#define VIRTQ_STATUS_QSZ    1
#define VIRTMMIO_INPUT_NAME "virtinput"

/*
 * QEMU virtio-tablet coordinates sit in a fixed square:
#define INPUT_EVENT_ABS_MIN    0x0000
#define INPUT_EVENT_ABS_MAX    0x7FFF
 */
#define QEMU_TABLET_LEN        0x8000

enum {
    VIRTIO_INPUT_CFG_UNSET      = 0x00,
    VIRTIO_INPUT_CFG_ID_NAME    = 0x01,
    VIRTIO_INPUT_CFG_ID_SERIAL  = 0x02,
    VIRTIO_INPUT_CFG_ID_DEVIDS  = 0x03,
    VIRTIO_INPUT_CFG_PROP_BITS  = 0x10,
    VIRTIO_INPUT_CFG_EV_BITS    = 0x11,
    VIRTIO_INPUT_CFG_ABS_INFO   = 0x12,
};

struct VirtinAbsinfo {
    uint32_t min;
    uint32_t max;
    uint32_t fuzz;
    uint32_t flat;
    uint32_t res;
};

struct VirtinDevids {
    uint16_t bus;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
};

struct VirtinConfig {
    uint8_t select;
    uint8_t subsel;
    uint8_t size;
#define VIRTIN_PADDINGS  5
    uint8_t reserved[VIRTIN_PADDINGS];
    union {
#define VIRTIN_PROP_LEN  128
        char string[VIRTIN_PROP_LEN];
        uint8_t bitmap[VIRTIN_PROP_LEN];
        struct VirtinAbsinfo abs;
        struct VirtinDevids ids;
    } u;
};

struct VirtinEvent {
    uint16_t type;
    uint16_t code;
    uint32_t value;
};

struct Virtin {
    struct VirtmmioDev dev;

    struct VirtinEvent ev[VIRTQ_EVENT_QSZ]; /* event receive buffer */
    HdfWorkQueue wq;                        /* event work-queue */
};
static const InputDevice *g_virtInputDev; /* work thread need this data, using global for simplicity */

static bool Feature0(uint32_t features, uint32_t *supported, void *dev)
{
    (void)features;
    (void)supported;
    (void)dev;
    return true;
}

static bool Feature1(uint32_t features, uint32_t *supported, void *dev)
{
    (void)dev;
    if (features & VIRTIO_F_VERSION_1) {
        *supported |= VIRTIO_F_VERSION_1;
    } else {
        HDF_LOGE("[%s]virtio-mmio input has no VERSION_1 feature", __func__);
        return false;
    }

    return true;
}

static void PopulateEventQ(const struct Virtin *in)
{
    const struct Virtq *q = &in->dev.vq[0];
    int i;

    for (i = 0; i < VIRTQ_EVENT_QSZ; i++) {
        q->desc[i].pAddr = VMM_TO_DMA_ADDR((VADDR_T)&in->ev[i]);
        q->desc[i].len = sizeof(struct VirtinEvent);
        q->desc[i].flag = VIRTQ_DESC_F_WRITE;

        q->avail->ring[i] = i;
    }

    in->dev.vq[0].avail->index += in->dev.vq[0].qsz;
    OSAL_WRITEL(0, in->dev.base + VIRTMMIO_REG_QUEUENOTIFY);
}

static void VirtinWorkCallback(void *arg)
{
    struct VirtinEvent *ev = arg;
    if (ev->type == EV_ABS) {
        if (ev->code == ABS_X) {    /* scale to actual screen */
            ev->value = ev->value * VirtgpuGetXres() / QEMU_TABLET_LEN;
            ev->code = ABS_MT_POSITION_X;   /* OHOS WMS only support this code for EV_ABS */
        } else if (ev->code == ABS_Y) {
            ev->value = ev->value * VirtgpuGetYres() / QEMU_TABLET_LEN;
            ev->code = ABS_MT_POSITION_Y;
        }
    }
    HidReportEvent(g_virtInputDev, ev->type, ev->code, ev->value);
}

static void VirtinHandleEv(struct Virtin *in)
{
    struct Virtq *q = &in->dev.vq[0];
    uint16_t idx;
    uint16_t add = 0;
    HdfWork w;

    q->avail->flag = VIRTQ_AVAIL_F_NO_INTERRUPT;
    while (q->last != q->used->index) {
        DSB;
        idx = q->used->ring[q->last % q->qsz].id;

        if (HdfWorkInit(&w, VirtinWorkCallback, &in->ev[idx]) == HDF_SUCCESS) {
            (void)HdfAddWork(&in->wq, &w);  /* HDF will alloc for 'realwork' */
        }

        q->avail->ring[(q->avail->index + add++) % q->qsz] = idx;
        q->last++;
    }
    DSB;
    q->avail->index += add;
    q->avail->flag = 0;

    if (q->used->flag != VIRTQ_USED_F_NO_NOTIFY) {
        OSAL_WRITEL(0, in->dev.base + VIRTMMIO_REG_QUEUENOTIFY);
    }
}

static uint32_t VirtinIRQhandle(uint32_t swIrq, void *dev)
{
    (void)swIrq;
    struct Virtin *in = dev;

    if (!(OSAL_READL(in->dev.base + VIRTMMIO_REG_INTERRUPTSTATUS) & VIRTMMIO_IRQ_NOTIFY_USED)) {
        return 1;
    }

    VirtinHandleEv(in);

    OSAL_WRITEL(VIRTMMIO_IRQ_NOTIFY_USED, in->dev.base + VIRTMMIO_REG_INTERRUPTACK);
    return 0;
}

static void VirtinFillHidAbsInfo(struct VirtinConfig *conf, HidInfo *devInfo)
{
    int32_t i;

    for (i = 0; i < HDF_ABS_CNT; i++) {
        DSB;
        conf->select = VIRTIO_INPUT_CFG_ABS_INFO;
        conf->subsel = i;
        DSB;
        if (conf->size == 0) {
            continue;
        }
        devInfo->axisInfo[i].axis = i;
        devInfo->axisInfo[i].min = (int32_t)conf->u.abs.min;
        devInfo->axisInfo[i].max = (int32_t)conf->u.abs.max;
        devInfo->axisInfo[i].fuzz = (int32_t)conf->u.abs.fuzz;
        devInfo->axisInfo[i].flat = (int32_t)conf->u.abs.flat;
        devInfo->axisInfo[i].range = (int32_t)conf->u.abs.res;
    }
}

static void VirtinFillHidCodeBitmap(struct VirtinConfig *conf, HidInfo *devInfo)
{
    uint8_t *qDest = NULL;
    uint32_t i, evType, len;

    devInfo->eventType[0] = 0;
    for (evType = 0; evType < HDF_EV_CNT; evType++) {
        DSB;
        conf->select = VIRTIO_INPUT_CFG_EV_BITS;
        conf->subsel = evType;
        DSB;
        if (conf->size == 0) {
            continue;
        }
        switch (evType) {
            case EV_KEY:
                len = DIV_ROUND_UP(HDF_KEY_CNT, BYTE_HAS_BITS);
                qDest = (uint8_t *)devInfo->keyCode;
                break;
            case EV_REL:
                len = DIV_ROUND_UP(HDF_REL_CNT, BYTE_HAS_BITS);
                qDest = (uint8_t *)devInfo->relCode;
                break;
            case EV_ABS:
                len = DIV_ROUND_UP(HDF_ABS_CNT, BYTE_HAS_BITS);
                qDest = (uint8_t *)devInfo->absCode;
                break;
            default:
                HDF_LOGW("[%s]unsupported event type: %d", __func__, evType);
                continue;
        }
        devInfo->eventType[0] |= 1 << evType;
        for (i = 0; i < len && i < VIRTIN_PROP_LEN; i++) {
            qDest[i] = conf->u.bitmap[i];
        }
    }
}

static void VirtinFillHidDevIds(struct VirtinConfig *conf, HidInfo *devInfo)
{
    conf->select = VIRTIO_INPUT_CFG_ID_DEVIDS;
    conf->subsel = 0;
    DSB;
    if (conf->size) {
        devInfo->bustype = conf->u.ids.bus;
        devInfo->vendor = conf->u.ids.vendor;
        devInfo->product = conf->u.ids.product;
        devInfo->version = conf->u.ids.version;
    }
}

static void VirtinFillHidInfo(const struct Virtin *in, HidInfo *devInfo)
{
    struct VirtinConfig *conf = (struct VirtinConfig *)(in->dev.base + VIRTMMIO_REG_CONFIG);
    uint32_t before, after;

    devInfo->devType = INDEV_TYPE_MOUSE;    /* only mouse and keyboard available */
    devInfo->devName = VIRTMMIO_INPUT_NAME;

    do {
        before = OSAL_READL(in->dev.base + VIRTMMIO_REG_CONFIGGENERATION);

        VirtinFillHidDevIds(conf, devInfo);
        VirtinFillHidCodeBitmap(conf, devInfo);
        VirtinFillHidAbsInfo(conf, devInfo);

        after = OSAL_READL(in->dev.base + VIRTMMIO_REG_CONFIGGENERATION);
    } while (before != after);
}

static int32_t HdfVirtinInitHid(struct Virtin *in)
{
    int32_t ret = HDF_SUCCESS;

    HidInfo *devInfo = OsalMemCalloc(sizeof(HidInfo));
    if (devInfo == NULL) {
        HDF_LOGE("[%s]alloc HidInfo memory failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    VirtinFillHidInfo(in, devInfo);
    SendInfoToHdf(devInfo);

    g_virtInputDev = HidRegisterHdfInputDev(devInfo);
    if (g_virtInputDev == NULL) {
        HDF_LOGE("[%s]register input device failed", __func__);
        ret = HDF_FAILURE;
    }

    OsalMemFree(devInfo);
    return ret;
}

static void VirtinDeInit(struct Virtin *in)
{
    if (in->dev.irq & ~_IRQ_MASK) {
        OsalUnregisterIrq(in->dev.irq & _IRQ_MASK, in);
    }
    LOS_DmaMemFree(in);
}

static struct Virtin *VirtinInitDev(void)
{
    struct Virtin *in = NULL;
    VADDR_T base;
    uint16_t qsz[VIRTQ_NUM];
    int32_t ret, len;

    len = sizeof(struct Virtin) + VirtqSize(VIRTQ_EVENT_QSZ) + VirtqSize(VIRTQ_STATUS_QSZ);
    in = LOS_DmaMemAlloc(NULL, len, sizeof(void *), DMA_CACHE);
    if (in == NULL) {
        HDF_LOGE("[%s]alloc virtio-input memory failed", __func__);
        return NULL;
    }

    if (!VirtmmioDiscover(VIRTMMIO_DEVICE_ID_INPUT, &in->dev)) {
        goto ERR_OUT;
    }

    VirtmmioInitBegin(&in->dev);

    if (!VirtmmioNegotiate(&in->dev, Feature0, Feature1, in)) {
        goto ERR_OUT1;
    }

    base = ALIGN((VADDR_T)in + sizeof(struct Virtin), VIRTQ_ALIGN_DESC);
    qsz[0] = VIRTQ_EVENT_QSZ;
    qsz[1] = VIRTQ_STATUS_QSZ;
    if (VirtmmioConfigQueue(&in->dev, base, qsz, VIRTQ_NUM) == 0) {
        goto ERR_OUT1;
    }

    ret = OsalRegisterIrq(in->dev.irq, OSAL_IRQF_TRIGGER_NONE, (OsalIRQHandle)VirtinIRQhandle,
                                                                VIRTMMIO_INPUT_NAME, in);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("[%s]register IRQ failed: %d", __func__, ret);
        goto ERR_OUT1;
    }
    in->dev.irq |= ~_IRQ_MASK;

    return in;

ERR_OUT1:
    VirtmmioInitFailed(&in->dev);
ERR_OUT:
    VirtinDeInit(in);
    return NULL;
}

static int32_t HdfVirtinInit(struct HdfDeviceObject *device)
{
    struct Virtin *in = NULL;
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("[%s]device is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if ((in = VirtinInitDev()) == NULL) {
        return HDF_FAILURE;
    }
    device->priv = in;

    if ((ret = HdfVirtinInitHid(in)) != HDF_SUCCESS) {
        return ret;
    }

    if ((ret = HdfWorkQueueInit(&in->wq, VIRTMMIO_INPUT_NAME)) != HDF_SUCCESS) {
        return ret;
    }

    PopulateEventQ(in);
    VritmmioInitEnd(&in->dev);  /* now virt queue can be used */
    return HDF_SUCCESS;
}

static void HdfVirtinRelease(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        return;
    }

    struct Virtin *in = deviceObject->priv;
    if (in == NULL) {
        return;
    }

    if (in->wq.realWorkQueue) {
        HdfWorkQueueDestroy(&in->wq);
    }
    if (g_virtInputDev) {
        HidUnregisterHdfInputDev(g_virtInputDev);
    }
    VirtinDeInit(in);
}

struct HdfDriverEntry g_virtInputEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_VIRTIO_MOUSE",
    .Init = HdfVirtinInit,
    .Release = HdfVirtinRelease,
};

HDF_INIT(g_virtInputEntry);
