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

#include "base_async_work.h"

#include "mdns_addlocalservice_context.h"
#include "mdns_async_work.h"
#include "mdns_exec.h"
#include "mdns_removelocalservice_context.h"
#include "mdns_resolvelocalservice_context.h"
#include "mdns_startsearching_context.h"

namespace OHOS {
namespace NetManagerStandard {
void MDnsAsyncWork::ExecAddLocalService(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<MDnsAddLocalServiceContext, MDnsExec::ExecAddLocalService>(env, data);
}

void MDnsAsyncWork::ExecRemoveLocalService(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<MDnsRemoveLocalServiceContext, MDnsExec::ExecRemoveLocalService>(env, data);
}

void MDnsAsyncWork::ExecResolveLocalService(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<MDnsResolveLocalServiceContext, MDnsExec::ExecResolveLocalService>(env, data);
}

void MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::ExecStartSearchingMDNS(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<MDnsStartSearchingContext, MDnsExec::ExecStartSearchingMDNS>(env, data);
}

void MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::ExecStopSearchingMDNS(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<MDnsStopSearchingContext, MDnsExec::ExecStopSearchingMDNS>(env, data);
}

void MDnsAsyncWork::AddLocalServiceCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<MDnsAddLocalServiceContext, MDnsExec::AddLocalServiceCallback>(env, status, data);
}

void MDnsAsyncWork::RemoveLocalServiceCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<MDnsRemoveLocalServiceContext, MDnsExec::RemoveLocalServiceCallback>(env, status,
                                                                                                          data);
}

void MDnsAsyncWork::ResolveLocalServiceCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<MDnsResolveLocalServiceContext, MDnsExec::ResolveLocalServiceCallback>(env, status,
                                                                                                            data);
}

void MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::StartSearchingMDNSCallback(napi_env env, napi_status status,
                                                                               void *data)
{
    BaseAsyncWork::AsyncWorkCallback<MDnsStartSearchingContext, MDnsExec::StartSearchingMDNSCallback>(env, status,
                                                                                                      data);
}

void MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::StopSearchingMDNSCallback(napi_env env, napi_status status,
                                                                              void *data)
{
    BaseAsyncWork::AsyncWorkCallback<MDnsStopSearchingContext, MDnsExec::StopSearchingMDNSCallback>(env, status, data);
}
} // namespace NetManagerStandard
} // namespace OHOS
