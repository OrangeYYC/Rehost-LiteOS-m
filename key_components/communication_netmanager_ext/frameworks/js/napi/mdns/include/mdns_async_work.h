/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETMANAGEREXT_MDNS_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGEREXT_MDNS_ASYNC_WORK_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nocopyable.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(MDnsAsyncWork);

    MDnsAsyncWork() = delete;
    ~MDnsAsyncWork() = delete;

    static void ExecAddLocalService(napi_env env, void *data);
    static void ExecRemoveLocalService(napi_env env, void *data);
    static void ExecResolveLocalService(napi_env env, void *data);

    static void AddLocalServiceCallback(napi_env env, napi_status status, void *data);
    static void RemoveLocalServiceCallback(napi_env env, napi_status status, void *data);
    static void ResolveLocalServiceCallback(napi_env env, napi_status status, void *data);

    class MDnsDiscoverInterfaceAsyncWork final {
    public:
        static void ExecStartSearchingMDNS(napi_env env, void *data);
        static void ExecStopSearchingMDNS(napi_env env, void *data);

        static void StartSearchingMDNSCallback(napi_env env, napi_status status, void *data);
        static void StopSearchingMDNSCallback(napi_env env, napi_status status, void *data);
    };
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGEREXT_MDNS_ASYNC_WORK_H
