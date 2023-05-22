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

#include <cstdint>
#include <string_view>

#include <napi/native_api.h>
#include <napi/native_common.h>

#include "constant.h"
#include "module_template.h"
#include "napi_utils.h"
#include "net_manager_ext_constants.h"
#include "netmanager_ext_log.h"
#include "netshare_async_work.h"
#include "netshare_exec.h"
#include "netshare_issharing_context.h"
#include "netshare_observer_wrapper.h"
#include "netshare_startsharing_context.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
// module name
constexpr const char *SHARE_MODULE_NAME = "net.sharing";

// name of functions
constexpr const char *FUNCTION_IS_SHARING_SUPPORTED = "isSharingSupported";
constexpr const char *FUNCTION_IS_SHARING = "isSharing";
constexpr const char *FUNCTION_START_SHARING = "startSharing";
constexpr const char *FUNCTION_STOP_SHARING = "stopSharing";
constexpr const char *FUNCTION_GET_SHARING_IFACES = "getSharingIfaces";
constexpr const char *FUNCTION_GET_SHARING_STATE = "getSharingState";
constexpr const char *FUNCTION_GET_SHARABLE_REGEXES = "getSharableRegexes";
constexpr const char *FUNCTION_GET_STATS_RX_BYTES = "getStatsRxBytes";
constexpr const char *FUNCTION_GET_STATS_TX_BYTES = "getStatsTxBytes";
constexpr const char *FUNCTION_GET_STATS_TOTAL_BYTES = "getStatsTotalBytes";
constexpr const char *FUNCTION_ON = "on";
constexpr const char *FUNCTION_OFF = "off";

// enum SharingIfaceState
constexpr const char *INTERFACE_SHARING_IFACE_STATE = "SharingIfaceState";
constexpr const char *SHARING_NIC_SERVING = "SHARING_NIC_SERVING";
constexpr const char *SHARING_NIC_CAN_SERVER = "SHARING_NIC_CAN_SERVER";
constexpr const char *SHARING_NIC_ERROR = "SHARING_NIC_ERROR";

// enum SharingIfaceType
constexpr const char *INTERFACE_SHARING_IFACE_TYPE = "SharingIfaceType";
constexpr const char *SHARING_WIFI = "SHARING_WIFI";
constexpr const char *SHARING_USB = "SHARING_USB";
constexpr const char *SHARING_BLUETOOTH = "SHARING_BLUETOOTH";
} // namespace

napi_value IsSharingSupported(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<IsSharingSupportedContext>(env, info, FUNCTION_IS_SHARING_SUPPORTED, nullptr,
                                                                NetShareAsyncWork::ExecIsSharingSupported,
                                                                NetShareAsyncWork::IsSharingSupportedCallback);
}

napi_value IsSharing(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<NetShareIsSharingContext>(env, info, FUNCTION_IS_SHARING, nullptr,
                                                               NetShareAsyncWork::ExecIsSharing,
                                                               NetShareAsyncWork::IsSharingCallback);
}

napi_value StartSharing(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<NetShareStartSharingContext>(env, info, FUNCTION_START_SHARING, nullptr,
                                                                  NetShareAsyncWork::ExecStartSharing,
                                                                  NetShareAsyncWork::StartSharingCallback);
}

napi_value StopSharing(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<StopSharingContext>(env, info, FUNCTION_STOP_SHARING, nullptr,
                                                         NetShareAsyncWork::ExecStopSharing,
                                                         NetShareAsyncWork::StopSharingCallback);
}

napi_value GetSharingIfaces(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetSharingIfacesContext>(env, info, FUNCTION_GET_SHARING_IFACES, nullptr,
                                                              NetShareAsyncWork::ExecGetSharingIfaces,
                                                              NetShareAsyncWork::GetSharingIfacesCallback);
}

napi_value GetSharingState(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetSharingStateContext>(env, info, FUNCTION_GET_SHARING_STATE, nullptr,
                                                             NetShareAsyncWork::ExecGetSharingState,
                                                             NetShareAsyncWork::GetSharingStateCallback);
}

napi_value GetSharableRegexes(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetSharableRegexesContext>(env, info, FUNCTION_GET_SHARABLE_REGEXES, nullptr,
                                                                NetShareAsyncWork::ExecGetSharableRegexes,
                                                                NetShareAsyncWork::GetSharableRegexesCallback);
}

napi_value GetStatsRxBytes(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetStatsRxBytesContext>(env, info, FUNCTION_GET_STATS_RX_BYTES, nullptr,
                                                             NetShareAsyncWork::ExecGetStatsRxBytes,
                                                             NetShareAsyncWork::GetStatsRxBytesCallback);
}

napi_value GetStatsTxBytes(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetStatsTxBytesContext>(env, info, FUNCTION_GET_STATS_TX_BYTES, nullptr,
                                                             NetShareAsyncWork::ExecGetStatsTxBytes,
                                                             NetShareAsyncWork::GetStatsTxBytesCallback);
}

napi_value GetStatsTotalBytes(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetStatsTotalBytesContext>(env, info, FUNCTION_GET_STATS_TOTAL_BYTES, nullptr,
                                                                NetShareAsyncWork::ExecGetStatsTotalBytes,
                                                                NetShareAsyncWork::GetStatsTotalBytesCallback);
}

napi_value On(napi_env env, napi_callback_info info)
{
    std::initializer_list<std::string_view> events = {EVENT_SHARE_STATE_CHANGE, EVENT_IFACE_SHARE_STATE_CHANGE,
                                                      EVENT_SHARE_UPSTREAM_CHANGE};
    return DelayedSingleton<NetShareObserverWrapper>::GetInstance()->On(env, info, events, false);
}

napi_value Off(napi_env env, napi_callback_info info)
{
    std::initializer_list<std::string_view> events = {EVENT_SHARE_STATE_CHANGE, EVENT_IFACE_SHARE_STATE_CHANGE,
                                                      EVENT_SHARE_UPSTREAM_CHANGE};
    return DelayedSingleton<NetShareObserverWrapper>::GetInstance()->Off(env, info, events, false);
}

void InitProperties(napi_env env, napi_value &exports)
{
    std::initializer_list<napi_property_descriptor> sharingIfaceState = {
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_NIC_SERVING,
            NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceState::SHARING_NIC_SERVING))),
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_NIC_CAN_SERVER,
            NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceState::SHARING_NIC_CAN_SERVER))),
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_NIC_ERROR,
            NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceState::SHARING_NIC_ERROR))),
    };
    napi_value ifaceState = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, ifaceState, sharingIfaceState);
    NapiUtils::SetNamedProperty(env, exports, INTERFACE_SHARING_IFACE_STATE, ifaceState);

    std::initializer_list<napi_property_descriptor> sharingIfaceType = {
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_WIFI, NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceType::SHARING_WIFI))),
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_USB, NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceType::SHARING_USB))),
        DECLARE_NAPI_STATIC_PROPERTY(
            SHARING_BLUETOOTH,
            NapiUtils::CreateUint32(env, static_cast<uint32_t>(SharingIfaceType::SHARING_BLUETOOTH))),
    };
    napi_value ifaceType = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, ifaceType, sharingIfaceType);
    NapiUtils::SetNamedProperty(env, exports, INTERFACE_SHARING_IFACE_TYPE, ifaceType);
}

napi_value InitNetShareModule(napi_env env, napi_value exports)
{
    InitProperties(env, exports);

    NapiUtils::DefineProperties(env, exports,
                                {
                                    DECLARE_NAPI_FUNCTION(FUNCTION_IS_SHARING_SUPPORTED, IsSharingSupported),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_IS_SHARING, IsSharing),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_START_SHARING, StartSharing),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_STOP_SHARING, StopSharing),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_SHARING_IFACES, GetSharingIfaces),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_SHARING_STATE, GetSharingState),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_SHARABLE_REGEXES, GetSharableRegexes),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_STATS_RX_BYTES, GetStatsRxBytes),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_STATS_TX_BYTES, GetStatsTxBytes),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_GET_STATS_TOTAL_BYTES, GetStatsTotalBytes),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_ON, On),
                                    DECLARE_NAPI_FUNCTION(FUNCTION_OFF, Off),
                                });

    return exports;
}

static napi_module g_netshareModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitNetShareModule,
    .nm_modname = SHARE_MODULE_NAME,
    .nm_priv = nullptr,
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterNetShareModule(void)
{
    napi_module_register(&g_netshareModule);
}
} // namespace NetManagerStandard
} // namespace OHOS
