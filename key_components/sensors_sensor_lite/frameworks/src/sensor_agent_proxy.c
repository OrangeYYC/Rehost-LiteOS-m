/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "sensor_agent_proxy.h"

#include <log.h>
#include <securec.h>
#include <stdlib.h>
#include "sensor_agent.h"
#include "sensor_service.h"

static SensorInfo *g_sensorLists;
static SensorEvent *g_sensorEvent;
static int32_t g_sensorListsLength;
static SvcIdentity g_svcIdentity = {
    .handle = 0,
    .token = 0,
};
static IpcObjectStub g_objectStub;

typedef struct CallbackNode {
    RecordSensorCallback callback;
    void *next;
} CallbackNode;

CallbackNode g_callbackNodes[(int32_t)SENSOR_TYPE_ID_MAX] = {
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
};

int32_t IsRegisterCallback()
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    int32_t isRegisterCallback = 0;
    for (int32_t sensorTypeId = 0; sensorTypeId < SENSOR_TYPE_ID_MAX; sensorTypeId++) {
        if (g_callbackNodes[sensorTypeId].next != NULL) {
            isRegisterCallback = 1;
            break;
        }
    }
    return isRegisterCallback;
}

static int32_t InsertCallbackNode(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((sensorTypeId >= (int32_t)SENSOR_TYPE_ID_MAX) || (sensorTypeId < 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "%s invalid sensor type id", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    CallbackNode *pre = &(g_callbackNodes[sensorTypeId]);
    CallbackNode *node = (CallbackNode *)(g_callbackNodes[sensorTypeId].next);
    while (node != NULL) {
        if (node->callback == user->callback) {
            HILOG_DEBUG(HILOG_MODULE_SEN, "%s callback has been inserted", __func__);
            return SENSOR_OK;
        }
        pre = node;
        node = (CallbackNode *)(node->next);
    }
    CallbackNode *nd = (CallbackNode *)malloc(sizeof(CallbackNode));
    if (nd == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "%s malloc failed", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    nd->callback = user->callback;
    nd->next = NULL;
    pre->next = nd;
    return SENSOR_OK;
}

static int32_t DeleteCallbackNode(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((sensorTypeId >= (int32_t)SENSOR_TYPE_ID_MAX) || (sensorTypeId < 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "%s invalid sensor id", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    CallbackNode *pre = &(g_callbackNodes[sensorTypeId]);
    CallbackNode *node = (CallbackNode *)(g_callbackNodes[sensorTypeId].next);
    while (node != NULL) {
        if (node->callback == user->callback) {
            pre->next = (CallbackNode *)(node->next);
            free(node);
            return SENSOR_OK;
        }
        node = (CallbackNode *)(node->next);
    }
    return SENSOR_ERROR_INVALID_PARAM;
}

void *GetServiceProxy()
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(SENSOR_SERVICE);
    if (iUnknown == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s get sensor service failed", __func__);
        return NULL;
    }
    IClientProxy *sensorApi = NULL;
    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&sensorApi);
    return sensorApi;
}

typedef struct SensorNotifyBuffer {
    int32_t retCode;
    int32_t count;
    SensorInfo **sensorInfo;
} SensorNotifyBuffer;

int32_t GetSensorInfos(IOwner owner, IpcIo *reply)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    SensorNotifyBuffer *notify = (SensorNotifyBuffer *)owner;
    if (notify == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s notify is null", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    } else {        
        ReadInt32(reply, &(notify->retCode));
        if (notify->retCode < 0) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s failed, retCode: %d", __func__, notify->retCode);
            return SENSOR_ERROR_INVALID_PARAM;
        }
        ReadInt32(reply, &(notify->count));
        uint32_t len;
        ReadUint32(reply, &len);
        uint8_t *data = (uint8_t *)ReadBuffer(reply, (size_t)len);
        if ((notify->count <= 0) || (data == NULL)) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s failed, count is incorrect or dataBuf is NULL or buff is NULL", __func__);
            notify->retCode = SENSOR_ERROR_INVALID_PARAM;
            return SENSOR_ERROR_INVALID_PARAM;
        }
        SensorInfo *sensorInfo = (SensorInfo *)(data);
        *(notify->sensorInfo) = (SensorInfo *)malloc(sizeof(SensorInfo) * notify->count);
        if (*(notify->sensorInfo) == NULL) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s malloc sensorInfo failed", __func__);
            notify->retCode = SENSOR_ERROR_INVALID_PARAM;
            return SENSOR_ERROR_INVALID_PARAM;
        }
        for (int32_t i = 0; i < notify->count; i++) {
            if (memcpy_s((*(notify->sensorInfo) + i), sizeof(SensorInfo), (sensorInfo + i),
                sizeof(SensorInfo))) {
                HILOG_ERROR(HILOG_MODULE_APP, "%s copy sensorInfo failed", __func__);
                free(*(notify->sensorInfo));
                *(notify->sensorInfo) = NULL;
                notify->retCode = SENSOR_ERROR_INVALID_PARAM;
                return SENSOR_ERROR_INVALID_PARAM;
            }
        }
        return notify->retCode;
    }
}

int32_t Notify(IOwner owner, int32_t code, IpcIo *reply)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    int32_t functionId;
    ReadInt32(reply, &functionId);
    if (functionId == SENSOR_SERVICE_ID_GetAllSensors) {
        return GetSensorInfos(owner, reply);
    }
    int32_t *ret = (int32_t *)owner;
    if (ret == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s ret is null", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    } else {
        if ((functionId > SENSOR_SERVICE_ID_GetAllSensors) && (functionId < SENSORMGR_LISTENER_NAME_LEN)) {
            ReadInt32(reply, ret);
            HILOG_DEBUG(HILOG_MODULE_APP, "%s ret: %d", __func__, *ret);
        } else {
            *ret = SENSOR_ERROR_INVALID_PARAM;
            HILOG_ERROR(HILOG_MODULE_APP, "%s functionId: %d is invalid", __func__, functionId);
        }
        return *ret;
    }
}

void DispatchData(SensorEvent *sensorEvent)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if (sensorEvent == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, sensorEvent is NULL", __func__);
        return;
    }
    int32_t sensorId = sensorEvent->sensorTypeId;
    CallbackNode *node = (CallbackNode *)(g_callbackNodes[sensorId].next);
    while (node != NULL) {
        node->callback(sensorEvent);
        node = (CallbackNode *)(node->next);
    }
}

int32_t SensorChannelCallback(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if (data == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, data is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    uint32_t len1;
    ReadUint32(data, &len1);
    uint8_t *eventData = (uint8_t *)ReadBuffer(data, (size_t)len1);
    uint32_t len2;
    ReadUint32(data, &len2);
    uint8_t *sensorData = (uint8_t *)ReadBuffer(data, (size_t)len2);
    if ((eventData == NULL) || (sensorData == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, eventBuff or sensorDataBuff or buff is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    SensorEvent *event = (SensorEvent *)(eventData);
    g_sensorEvent->dataLen = event->dataLen;
    g_sensorEvent->timestamp = event->timestamp;
    g_sensorEvent->mode = event->mode;
    g_sensorEvent->option = event->option;
    g_sensorEvent->sensorTypeId = event->sensorTypeId;
    g_sensorEvent->version = event->version;
    g_sensorEvent->data = sensorData;
    DispatchData(g_sensorEvent);
    return SENSOR_OK;
}

int32_t RegisterSensorChannel(const void *proxy, int32_t sensorId)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if (IsRegisterCallback()) {
        HILOG_DEBUG(HILOG_MODULE_APP, "%s sensorChannel has been registered ", __func__);
        return SENSOR_OK;
    }
    g_objectStub.func = SensorChannelCallback;
    g_objectStub.args = NULL;
    g_objectStub.isRemote = false;

    g_svcIdentity.handle = IPC_INVALID_HANDLE;
    g_svcIdentity.token = SERVICE_TYPE_ANONYMOUS;
    g_svcIdentity.cookie = (uintptr_t)&g_objectStub;
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_MAX_OBJECTS);
    WriteInt32(&request, sensorId);
    bool writeRemote = WriteRemoteObject(&request, &g_svcIdentity);
    if (!writeRemote) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s WriteRemoteObject failed.", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IClientProxy *client = (IClientProxy *)proxy;
    if (client == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s client is null", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t retCode = -1;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_SubscribeSensor, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if (g_sensorEvent == NULL) {
        g_sensorEvent = (SensorEvent *)malloc(sizeof(SensorEvent));
        if (g_sensorEvent == NULL) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s malloc failed", __func__);
            return SENSOR_ERROR_INVALID_PARAM;
        }
    }
    return SENSOR_OK;
}

int32_t UnregisterSensorChannel(const void *proxy, int32_t sensorId)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if (!IsRegisterCallback()) {
        IpcIo request;
        char data[MAX_IO_SIZE];
        IpcIoInit(&request, data, MAX_IO_SIZE, 0);
        WriteInt32(&request, sensorId);
        IClientProxy *client = (IClientProxy *)proxy;
        if (client == NULL) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s client is null", __func__);
            return SENSOR_ERROR_INVALID_PARAM;
        } else {
            int32_t retCode = -1;
            int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_UnsubscribeSensor, &request, &retCode, Notify);
            if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
                HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
                return SENSOR_ERROR_INVALID_PARAM;
            }
            if (g_sensorEvent != NULL) {
                free(g_sensorEvent);
                g_sensorEvent = NULL;
            }
            HILOG_DEBUG(HILOG_MODULE_APP, "%s sensorChannel has been destroyed ", __func__);
        }
    }
    return SENSOR_OK;
}

int32_t InitSensorList(const void *proxy)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if (proxy == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    SensorNotifyBuffer owner = {
        .retCode = -1,
        .count = 0,
        .sensorInfo = &g_sensorLists,
    };
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_MAX_OBJECTS);
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_GetAllSensors, &request, &owner, Notify);
    if ((ret != SENSOR_OK) || (owner.retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, owner.retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    g_sensorListsLength = owner.count;
    return SENSOR_OK;
}

int32_t GetAllSensorsByProxy(const void *proxy, SensorInfo **sensorInfo, int32_t *count)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((sensorInfo == NULL) || (count == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s sensorInfo or count is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if (g_sensorLists == NULL) {
        int32_t ret = InitSensorList(proxy);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_APP, "%s init sensorList failed, ret: %d", __func__, ret);
            return SENSOR_ERROR_INVALID_PARAM;
        }
    }
    *count = g_sensorListsLength;
    *sensorInfo = g_sensorLists;
    return SENSOR_OK;
}

int32_t ActivateSensorByProxy(const void *proxy, int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, 0);
    WriteInt32(&request, sensorId);
    int32_t retCode = -1;
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_ActivateSensor, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return retCode;
}

int32_t DeactivateSensorByProxy(const void *proxy, int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, 0);
    WriteInt32(&request, sensorId);
    int32_t retCode = -1;
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_DeactivateSensor, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return retCode;
}

int32_t SetBatchByProxy(const void *proxy, int32_t sensorId, const SensorUser *user, int64_t samplingInterval,
    int64_t reportInterval)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, 0);
    WriteInt32(&request, sensorId);
    WriteInt64(&request, samplingInterval);
    WriteInt64(&request, reportInterval);
    int32_t retCode = -1;
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_SetBatchs, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return retCode;
}

int32_t CheckSensorTypeId(int32_t sensorTypeId)
{
    if ((g_sensorLists == NULL) || (g_sensorListsLength == 0)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s g_sensorLists or g_sensorListsLength is invalid", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < g_sensorListsLength; i++) {
        if ((g_sensorLists + i)->sensorId == sensorTypeId) {
            return SENSOR_OK;
        }
    }
    return SENSOR_ERROR_INVALID_PARAM;
}

int32_t SubscribeSensorByProxy(const void *proxy, int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL) || (user->callback == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user or callback is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if (CheckSensorTypeId(sensorId) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s sensorid: %d is invalid", __func__, sensorId);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = RegisterSensorChannel(proxy, sensorId);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s register sensor channel failed, ret: %d", __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    ret = InsertCallbackNode(sensorId, user);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s insert callback node failed, ret: %d", __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_OK;
}

int32_t UnsubscribeSensorByProxy(const void *proxy, int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL) || (user->callback == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user or callback is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if (CheckSensorTypeId(sensorId) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s sensorid: %d is invalid", __func__, sensorId);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = DeleteCallbackNode(sensorId, user);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s delete callback node failed, ret: %d", __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }

    ret = UnregisterSensorChannel(proxy, sensorId);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s delete callback node failed, ret: %d", __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_OK;
}

int32_t SetModeByProxy(const void *proxy, int32_t sensorId, const SensorUser *user, int32_t mode)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, 0);
    WriteInt32(&request, sensorId);
    WriteInt32(&request, mode);
    int32_t retCode = -1;
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_SetMode, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return retCode;
}

int32_t SetOptionByProxy(const void *proxy, int32_t sensorId, const SensorUser *user, int32_t option)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "%s begin", __func__);
    if ((proxy == NULL) || (user == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s proxy or user is NULL", __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, 0);
    WriteInt32(&request, sensorId);
    WriteInt32(&request, option);
    int32_t retCode = -1;
    IClientProxy *client = (IClientProxy *)proxy;
    int32_t ret = client->Invoke(client, SENSOR_SERVICE_ID_SetOption, &request, &retCode, Notify);
    if ((ret != SENSOR_OK) || (retCode != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s failed, ret: %d, retCode: %d", __func__, ret, retCode);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return retCode;
}
