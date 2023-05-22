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

#include "netshare_exec.h"

#include "constant.h"
#include "napi_utils.h"
#include "net_manager_constants.h"
#include "net_manager_ext_constants.h"
#include "netmanager_ext_log.h"
#include "networkshare_client.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetShareExec::ExecIsSharingSupported(IsSharingSupportedContext *context)
{
    int32_t supported = NETWORKSHARE_IS_UNSUPPORTED;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->IsSharingSupported(supported);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecIsSharingSupported error, errorCode: %{public}d", result);
        return false;
    }
    context->SetSharingSupported(supported);
    return true;
}

napi_value NetShareExec::IsSharingSupportedCallback(IsSharingSupportedContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), static_cast<bool>(context->GetSharingSupported()));
}

bool NetShareExec::ExecIsSharing(NetShareIsSharingContext *context)
{
    int32_t sharingStatus = NETWORKSHARE_IS_UNSHARING;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->IsSharing(sharingStatus);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecIsSharing error, errorCode: %{public}d", result);
        return false;
    }
    context->SetSharing(sharingStatus);
    return true;
}

napi_value NetShareExec::IsSharingCallback(NetShareIsSharingContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), static_cast<bool>(context->GetSharing()));
}

bool NetShareExec::ExecStartSharing(NetShareStartSharingContext *context)
{
    SharingIfaceType ifaceType = static_cast<SharingIfaceType>(context->GetParam());
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->StartSharing(ifaceType);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecStartSharing error, errorCode: %{public}d", result);
        return false;
    }
    return true;
}

napi_value NetShareExec::StartSharingCallback(NetShareStartSharingContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetShareExec::ExecStopSharing(StopSharingContext *context)
{
    SharingIfaceType ifaceType = static_cast<SharingIfaceType>(context->GetParam());
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->StopSharing(ifaceType);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecStopSharing error, errorCode: %{public}d", result);
        return false;
    }
    return true;
}

napi_value NetShareExec::StopSharingCallback(StopSharingContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetShareExec::ExecGetSharingIfaces(GetSharingIfacesContext *context)
{
    SharingIfaceState ifaceState = static_cast<SharingIfaceState>(context->GetParam());
    std::vector<std::string> ifaces;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetSharingIfaces(ifaceState, ifaces);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetSharingIfaces error, errorCode: %{public}d", result);
        return false;
    }
    context->SetIface(ifaces);
    return true;
}

napi_value NetShareExec::GetSharingIfacesCallback(GetSharingIfacesContext *context)
{
    napi_value ifacesArray = NapiUtils::CreateArray(context->GetEnv(), context->GetIfaces().size());
    uint32_t index = 0;
    for (auto iface : context->GetIfaces()) {
        napi_value item = NapiUtils::CreateStringUtf8(context->GetEnv(), iface);
        NapiUtils::SetArrayElement(context->GetEnv(), ifacesArray, index++, item);
    }
    return ifacesArray;
}

bool NetShareExec::ExecGetSharingState(GetSharingStateContext *context)
{
    SharingIfaceType ifaceType = static_cast<SharingIfaceType>(context->GetParam());
    SharingIfaceState ifaceState;

    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetSharingState(ifaceType, ifaceState);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetSharingState error, errorCode: %{public}d", result);
        return false;
    }
    context->SetParam(static_cast<int32_t>(ifaceState));
    return true;
}

napi_value NetShareExec::GetSharingStateCallback(GetSharingStateContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->GetParam());
}

bool NetShareExec::ExecGetSharableRegexes(GetSharableRegexesContext *context)
{
    SharingIfaceType ifaceType = static_cast<SharingIfaceType>(context->GetParam());
    std::vector<std::string> ifaceRegexs;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetSharableRegexs(ifaceType, ifaceRegexs);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetSharableRegexes error, errorCode: %{public}d", result);
        return false;
    }
    context->SetIface(ifaceRegexs);
    return true;
}

napi_value NetShareExec::GetSharableRegexesCallback(GetSharableRegexesContext *context)
{
    napi_value ifacesArray = NapiUtils::CreateArray(context->GetEnv(), context->GetIfaces().size());
    uint32_t index = 0;
    for (auto iface : context->GetIfaces()) {
        napi_value item = NapiUtils::CreateStringUtf8(context->GetEnv(), iface);
        NapiUtils::SetArrayElement(context->GetEnv(), ifacesArray, index++, item);
    }
    return ifacesArray;
}

bool NetShareExec::ExecGetStatsRxBytes(GetStatsRxBytesContext *context)
{
    int32_t bytes = 0;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetStatsRxBytes(bytes);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetStatsRxBytes error, errorCode: %{public}d", result);
        return false;
    }
    context->SetBytes32(bytes);
    return true;
}

napi_value NetShareExec::GetStatsRxBytesCallback(GetStatsRxBytesContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->GetBytes32());
}

bool NetShareExec::ExecGetStatsTxBytes(GetStatsTxBytesContext *context)
{
    int32_t bytes = 0;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetStatsTxBytes(bytes);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetStatsTxBytes error, errorCode: %{public}d", result);
        return false;
    }
    context->SetBytes32(bytes);
    return true;
}

napi_value NetShareExec::GetStatsTxBytesCallback(GetStatsTxBytesContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->GetBytes32());
}

bool NetShareExec::ExecGetStatsTotalBytes(GetStatsTotalBytesContext *context)
{
    int32_t bytes = 0;
    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->GetStatsTotalBytes(bytes);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecGetStatsTotalBytes error, errorCode: %{public}d", result);
        return false;
    }
    context->SetBytes32(bytes);
    return true;
}

napi_value NetShareExec::GetStatsTotalBytesCallback(GetStatsTotalBytesContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->GetBytes32());
}
} // namespace NetManagerStandard
} // namespace OHOS
