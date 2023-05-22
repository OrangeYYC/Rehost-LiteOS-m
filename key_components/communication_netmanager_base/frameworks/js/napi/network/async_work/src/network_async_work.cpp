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

#include "network_async_work.h"

#include "base_async_work.h"
#include "network_exec.h"

namespace OHOS::NetManagerStandard {
void NetworkAsyncWork::ExecGetType(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetTypeContext, NetworkExec::ExecGetType>(env, data);
}

void NetworkAsyncWork::GetTypeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallbackForSystem<GetTypeContext, NetworkExec::GetTypeCallback>(env, status, data);
}

void NetworkAsyncWork::ExecSubscribe(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SubscribeContext, NetworkExec::ExecSubscribe>(env, data);
}

void NetworkAsyncWork::SubscribeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallbackForSystem<SubscribeContext, NetworkExec::SubscribeCallback>(env, status, data);
}

void NetworkAsyncWork::ExecUnsubscribe(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UnsubscribeContext, NetworkExec::ExecUnsubscribe>(env, data);
}

void NetworkAsyncWork::UnsubscribeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallbackForSystem<UnsubscribeContext, NetworkExec::UnsubscribeCallback>(env, status, data);
}
} // namespace OHOS::NetManagerStandard