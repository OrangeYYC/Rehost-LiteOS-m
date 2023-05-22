/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETMANAGEREXT_NETSHARE_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGEREXT_NETSHARE_ASYNC_WORK_H

#include <napi/native_api.h>
#include <napi/native_node_api.h>

#include "nocopyable.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(NetShareAsyncWork);

    NetShareAsyncWork() = delete;
    ~NetShareAsyncWork() = delete;

    static void ExecIsSharingSupported(napi_env env, void *data);
    static void ExecIsSharing(napi_env env, void *data);
    static void ExecStartSharing(napi_env env, void *data);
    static void ExecStopSharing(napi_env env, void *data);
    static void ExecGetSharingIfaces(napi_env env, void *data);
    static void ExecGetSharingState(napi_env env, void *data);
    static void ExecGetSharableRegexes(napi_env env, void *data);
    static void ExecGetStatsRxBytes(napi_env env, void *data);
    static void ExecGetStatsTxBytes(napi_env env, void *data);
    static void ExecGetStatsTotalBytes(napi_env env, void *data);

    static void IsSharingSupportedCallback(napi_env env, napi_status status, void *data);
    static void IsSharingCallback(napi_env env, napi_status status, void *data);
    static void StartSharingCallback(napi_env env, napi_status status, void *data);
    static void StopSharingCallback(napi_env env, napi_status status, void *data);
    static void GetSharingIfacesCallback(napi_env env, napi_status status, void *data);
    static void GetSharingStateCallback(napi_env env, napi_status status, void *data);
    static void GetSharableRegexesCallback(napi_env env, napi_status status, void *data);
    static void GetStatsRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetStatsTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetStatsTotalBytesCallback(napi_env env, napi_status status, void *data);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGEREXT_NETSHARE_ASYNC_WORK_H
