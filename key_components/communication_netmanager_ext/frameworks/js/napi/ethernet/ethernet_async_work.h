/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NET_EXT_NAPI_ETHERNET_ASYNC_WORK_H
#define NET_EXT_NAPI_ETHERNET_ASYNC_WORK_H

#include <napi/native_api.h>

namespace OHOS {
namespace NetManagerStandard {
namespace EthernetAsyncWork {
void ExecGetIfaceConfig(napi_env env, void *data);
void GetIfaceConfigCallback(napi_env env, napi_status status, void *data);

void ExecSetIfaceConfig(napi_env env, void *data);
void SetIfaceConfigCallback(napi_env env, napi_status status, void *data);

void ExecIsIfaceActive(napi_env env, void *data);
void IsIfaceActiveCallback(napi_env env, napi_status status, void *data);

void ExecGetAllActiveIfaces(napi_env env, void *data);
void GetAllActiveIfacesCallback(napi_env env, napi_status status, void *data);
} // namespace EthernetAsyncWork
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_EXT_NAPI_ETHERNET_ASYNC_WORK_H
