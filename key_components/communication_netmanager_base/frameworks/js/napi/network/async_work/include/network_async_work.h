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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nocopyable.h"

namespace OHOS::NetManagerStandard {
class NetworkAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(NetworkAsyncWork);

    NetworkAsyncWork() = delete;

    ~NetworkAsyncWork() = delete;

    static void ExecGetType(napi_env env, void *data);

    static void GetTypeCallback(napi_env env, napi_status status, void *data);

    static void ExecSubscribe(napi_env env, void *data);

    static void SubscribeCallback(napi_env env, napi_status status, void *data);

    static void ExecUnsubscribe(napi_env env, void *data);

    static void UnsubscribeCallback(napi_env env, napi_status status, void *data);
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H */
