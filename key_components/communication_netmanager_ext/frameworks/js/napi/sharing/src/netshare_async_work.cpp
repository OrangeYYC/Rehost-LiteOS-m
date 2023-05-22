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

#include "netshare_async_work.h"

#include "base_async_work.h"
#include "netshare_exec.h"

namespace OHOS {
namespace NetManagerStandard {
void NetShareAsyncWork::ExecIsSharingSupported(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IsSharingSupportedContext, NetShareExec::ExecIsSharingSupported>(env, data);
}

void NetShareAsyncWork::ExecIsSharing(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<NetShareIsSharingContext, NetShareExec::ExecIsSharing>(env, data);
}

void NetShareAsyncWork::ExecStartSharing(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<NetShareStartSharingContext, NetShareExec::ExecStartSharing>(env, data);
}

void NetShareAsyncWork::ExecStopSharing(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<StopSharingContext, NetShareExec::ExecStopSharing>(env, data);
}

void NetShareAsyncWork::ExecGetSharingIfaces(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSharingIfacesContext, NetShareExec::ExecGetSharingIfaces>(env, data);
}

void NetShareAsyncWork::ExecGetSharingState(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSharingStateContext, NetShareExec::ExecGetSharingState>(env, data);
}

void NetShareAsyncWork::ExecGetSharableRegexes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSharableRegexesContext, NetShareExec::ExecGetSharableRegexes>(env, data);
}

void NetShareAsyncWork::ExecGetStatsRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetStatsRxBytesContext, NetShareExec::ExecGetStatsRxBytes>(env, data);
}

void NetShareAsyncWork::ExecGetStatsTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetStatsTxBytesContext, NetShareExec::ExecGetStatsTxBytes>(env, data);
}

void NetShareAsyncWork::ExecGetStatsTotalBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetStatsTotalBytesContext, NetShareExec::ExecGetStatsTotalBytes>(env, data);
}

void NetShareAsyncWork::IsSharingSupportedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IsSharingSupportedContext, NetShareExec::IsSharingSupportedCallback>(env, status,
                                                                                                          data);
}

void NetShareAsyncWork::IsSharingCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<NetShareIsSharingContext, NetShareExec::IsSharingCallback>(env, status, data);
}

void NetShareAsyncWork::StartSharingCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<NetShareStartSharingContext, NetShareExec::StartSharingCallback>(env, status,
                                                                                                      data);
}

void NetShareAsyncWork::StopSharingCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<StopSharingContext, NetShareExec::StopSharingCallback>(env, status, data);
}

void NetShareAsyncWork::GetSharingIfacesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSharingIfacesContext, NetShareExec::GetSharingIfacesCallback>(env, status,
                                                                                                      data);
}

void NetShareAsyncWork::GetSharingStateCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSharingStateContext, NetShareExec::GetSharingStateCallback>(env, status, data);
}

void NetShareAsyncWork::GetSharableRegexesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSharableRegexesContext, NetShareExec::GetSharableRegexesCallback>(env, status,
                                                                                                          data);
}

void NetShareAsyncWork::GetStatsRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetStatsRxBytesContext, NetShareExec::GetStatsRxBytesCallback>(env, status, data);
}

void NetShareAsyncWork::GetStatsTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetStatsTxBytesContext, NetShareExec::GetStatsTxBytesCallback>(env, status, data);
}

void NetShareAsyncWork::GetStatsTotalBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetStatsTotalBytesContext, NetShareExec::GetStatsTotalBytesCallback>(env, status,
                                                                                                          data);
}
} // namespace NetManagerStandard
} // namespace OHOS
