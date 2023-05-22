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

#include <cstdint>
#include <string_view>

#include "module_template.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi_utils.h"
#include "netmanager_ext_log.h"

#include "constant.h"
#include "mdns_addlocalservice_context.h"
#include "mdns_async_work.h"
#include "mdns_instances.h"
#include "mdns_module.h"
#include "mdns_removelocalservice_context.h"
#include "mdns_resolvelocalservice_context.h"
#include "mdns_startsearching_context.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr const char *MDNS_MODULE_NAME = "net.mdns";

static std::string GetContextIdString(napi_env env, napi_value obj)
{
    if (NapiUtils::HasNamedProperty(env, obj, CONTEXT_ATTR_APPINFO)) {
        napi_value info = NapiUtils::GetNamedProperty(env, obj, CONTEXT_ATTR_APPINFO);
        if (NapiUtils::HasNamedProperty(env, info, APPINFO_ATTR_NAME)) {
            return NapiUtils::GetStringPropertyUtf8(env, info, APPINFO_ATTR_NAME);
        }
    }
    return std::string();
}

static void *ParseMDnsDiscoveryParams(napi_env env, size_t argc, napi_value *argv, EventManager *manager)
{
    std::unique_ptr<MDnsDiscoveryInstance, decltype(&MDnsDiscoveryInstance::DeleteMDnsDiscovery)> mdnsDiscover(
        MDnsDiscoveryInstance::MakeMDnsDiscovery(manager), MDnsDiscoveryInstance::DeleteMDnsDiscovery);
    if (NapiUtils::GetValueType(env, argv[ARG_NUM_0]) == napi_object &&
        NapiUtils::GetValueType(env, argv[ARG_NUM_1]) == napi_string) {
        mdnsDiscover->context_ = GetContextIdString(env, argv[ARG_NUM_0]);
        mdnsDiscover->serviceType_ = NapiUtils::GetStringFromValueUtf8(env, argv[ARG_NUM_1]);
        return mdnsDiscover.release();
    }
    NETMANAGER_EXT_LOGE("constructor params invalid, should be context + serviceType");
    return nullptr;
}

napi_value MDnsModule::CreateDiscoveryService(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::NewInstance(env, info, FUNCTION_DISCOVERY_SERVICE, ParseMDnsDiscoveryParams,
                                       [](napi_env, void *data, void *) {
                                           NETMANAGER_BASE_LOGI("finalize DiscoveryService");
                                           auto manager = static_cast<EventManager *>(data);
                                           auto mdnsDiscover = static_cast<MDnsDiscoveryInstance *>(manager->GetData());
                                           delete manager;
                                           MDnsDiscoveryInstance::DeleteMDnsDiscovery(mdnsDiscover);
                                       });
}

napi_value MDnsModule::AddLocalService(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<MDnsAddLocalServiceContext>(env, info, FUNCTION_ADDLOCALSERVICE, nullptr,
                                                                 MDnsAsyncWork::ExecAddLocalService,
                                                                 MDnsAsyncWork::AddLocalServiceCallback);
}

napi_value MDnsModule::RemoveLocalService(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<MDnsRemoveLocalServiceContext>(env, info, FUNCTION_REMOVELOCALSERVICE, nullptr,
                                                                    MDnsAsyncWork::ExecRemoveLocalService,
                                                                    MDnsAsyncWork::RemoveLocalServiceCallback);
}

napi_value MDnsModule::ResolveLocalService(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<MDnsResolveLocalServiceContext>(env, info, FUNCTION_RESOLVELOCALSERVICE, nullptr,
                                                                     MDnsAsyncWork::ExecResolveLocalService,
                                                                     MDnsAsyncWork::ResolveLocalServiceCallback);
}

napi_value MDnsModule::DiscoveryServiceInterface::On(napi_env env, napi_callback_info info)
{
    std::initializer_list<std::string> events = {EVENT_SERVICESTART, EVENT_SERVICESTOP, EVENT_SERVICEFOUND,
                                                 EVENT_SERVICELOST};
    return ModuleTemplate::On(env, info, events, false);
}

napi_value MDnsModule::DiscoveryServiceInterface::StartSearchingMDNS(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<MDnsStartSearchingContext>(
        env, info, FUNCTION_STARTSEARCHINGMDNS, nullptr,
        MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::ExecStartSearchingMDNS,
        MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::StartSearchingMDNSCallback);
}

napi_value MDnsModule::DiscoveryServiceInterface::StopSearchingMDNS(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<MDnsStopSearchingContext>(
        env, info, FUNCTION_STOPSEARCHINGMDNS, nullptr,
        MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::ExecStopSearchingMDNS,
        MDnsAsyncWork::MDnsDiscoverInterfaceAsyncWork::StopSearchingMDNSCallback);
}

napi_value MDnsModule::InitMDnsModule(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> functions = {
        DECLARE_NAPI_FUNCTION(FUNCTION_CREATE_DISCOVERY, CreateDiscoveryService),
        DECLARE_NAPI_FUNCTION(FUNCTION_ADDLOCALSERVICE, AddLocalService),
        DECLARE_NAPI_FUNCTION(FUNCTION_REMOVELOCALSERVICE, RemoveLocalService),
        DECLARE_NAPI_FUNCTION(FUNCTION_RESOLVELOCALSERVICE, ResolveLocalService)};
    NapiUtils::DefineProperties(env, exports, functions);

    std::initializer_list<napi_property_descriptor> MDnsDiscoveryFunctions = {
        DECLARE_NAPI_FUNCTION(DiscoveryServiceInterface::FUNCTION_ON, DiscoveryServiceInterface::On),
        DECLARE_NAPI_FUNCTION(DiscoveryServiceInterface::FUNCTION_STARTSEARCHINGMDNS,
                              DiscoveryServiceInterface::StartSearchingMDNS),
        DECLARE_NAPI_FUNCTION(DiscoveryServiceInterface::FUNCTION_STOPSEARCHINGMDNS,
                              DiscoveryServiceInterface::StopSearchingMDNS),
    };
    ModuleTemplate::DefineClass(env, exports, MDnsDiscoveryFunctions, FUNCTION_DISCOVERY_SERVICE);

    std::initializer_list<napi_property_descriptor> mdnsError = {
            DECLARE_NAPI_STATIC_PROPERTY(INTERNAL_ERROR_NAME,
                                         NapiUtils::CreateUint32(env, static_cast<uint32_t>(MDnsErr::INTERNAL_ERROR))),
            DECLARE_NAPI_STATIC_PROPERTY(ALREADY_ACTIVE_NAME,
                                         NapiUtils::CreateUint32(env, static_cast<uint32_t>(MDnsErr::ALREADY_ACTIVE))),
            DECLARE_NAPI_STATIC_PROPERTY(MAX_LIMIT_NAME,
                                         NapiUtils::CreateUint32(env, static_cast<uint32_t>(MDnsErr::MAX_LIMIT))),
    };
    napi_value mdnsErrorValue = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, mdnsErrorValue, mdnsError);
    NapiUtils::SetNamedProperty(env, exports, MDNS_ERR, mdnsErrorValue);
    return exports;
}

static napi_module g_mdnsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = MDnsModule::InitMDnsModule,
    .nm_modname = MDNS_MODULE_NAME,
    .nm_priv = nullptr,
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterMDnsModule(void)
{
    napi_module_register(&g_mdnsModule);
}
} // namespace NetManagerStandard
} // namespace OHOS
