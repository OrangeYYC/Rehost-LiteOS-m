/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NSTACK_COMMON_H
#define NSTACK_COMMON_H
#include "nstackx.h"
#include "nstackx_list.h"
#include "nstackx_epoll.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef DFINDER_SAVE_DEVICE_LIST
typedef struct {
    NSTACKX_DeviceInfo *deviceList;
    uint32_t *deviceCountPtr;
    sem_t wait;
} GetDeviceListMessage;
#endif

struct DeviceInfo;

void NotifyDeviceListChanged(const NSTACKX_DeviceInfo *deviceList, uint32_t deviceCount);
void NotifyDeviceFound(const NSTACKX_DeviceInfo *deviceList, uint32_t deviceCount);
#ifndef DFINDER_USE_MINI_NSTACKX
void NotifyMsgReceived(const char *moduleName, const char *deviceId, const uint8_t *data, uint32_t len);
#endif /* END OF DFINDER_USE_MINI_NSTACKX */
void NotifyDFinderMsgRecver(DFinderMsgType msgType);
EpollDesc GetMainLoopEpollFd(void);
List *GetMainLoopEvendChain(void);
uint32_t GetDefaultDiscoverInterval(uint32_t discoverCount);
int32_t CheckBusinessTypeReplyUnicast(uint8_t businessType);
int32_t GetServiceDiscoverInfo(const uint8_t *buf, size_t size, struct DeviceInfo *deviceInfo, char **remoteUrlPtr);
List *GetEventNodeChain(void);
EpollDesc GetEpollFD(void);

#ifdef __cplusplus
};
#endif

#endif /* #ifndef NSTACK_COMMON_H */
