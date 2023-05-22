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

#include "ethernet_exec.h"

#include <cstdint>
#include <new>
#include <numeric>
#include <string>

#include "ethernet_client.h"
#include "napi_utils.h"
#include "net_manager_constants.h"
#include "netmanager_ext_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace EthernetExec {
namespace {
constexpr const char *MODE = "mode";
constexpr const char *IP_ADDR = "ipAddr";
constexpr const char *ROUTE = "route";
constexpr const char *NET_MASK = "netMask";
constexpr const char *GATEWAY = "gateway";
constexpr const char *DNS_SERVERS = "dnsServers";
constexpr const char *DOMAIN = "domain";
} // namespace
bool ExecGetIfaceConfig(GetIfaceConfigContext *context)
{
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->GetIfaceConfig(context->iface_, context->config_);
    if (context->config_ == nullptr || result != NETMANAGER_EXT_SUCCESS) {
        NETMANAGER_EXT_LOGE("ExecGetIfaceConfig error, errorCode: %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    return true;
}

napi_value GetIfaceConfigCallback(GetIfaceConfigContext *context)
{
    napi_value interfaceConfiguration = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetInt32Property(context->GetEnv(), interfaceConfiguration, MODE, context->config_->mode_);
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, IP_ADDR,
                                     context->config_->ipStatic_.ipAddr_.address_);
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, ROUTE,
                                     context->config_->ipStatic_.route_.address_);
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, GATEWAY,
                                     context->config_->ipStatic_.gateway_.address_);
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, NET_MASK,
                                     context->config_->ipStatic_.netMask_.address_);
    std::string dnsServers = std::accumulate(context->config_->ipStatic_.dnsServers_.begin(),
                                             context->config_->ipStatic_.dnsServers_.end(), std::string(),
                                             [](const std::string &str_append, INetAddr const &iter) {
                                                 return str_append + iter.address_ + ",";
                                             });

    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, DNS_SERVERS, dnsServers);
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), interfaceConfiguration, DOMAIN,
                                     context->config_->ipStatic_.domain_);
    return interfaceConfiguration;
}

bool ExecSetIfaceConfig(SetIfaceConfigContext *context)
{
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->SetIfaceConfig(context->iface_, context->config_);
    if (result != NETMANAGER_EXT_SUCCESS) {
        NETMANAGER_EXT_LOGE("ExecSetIfaceConfig error, errorCode: %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    return true;
}

napi_value SetIfaceConfigCallback(SetIfaceConfigContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ExecIsIfaceActive(IsIfaceActiveContext *context)
{
    int32_t result =
        DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive(context->iface_, context->ifActivate_);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecIsIfaceActive error, errorCode: %{public}d", result);
        return false;
    }
    return true;
}

napi_value IsIfaceActiveCallback(IsIfaceActiveContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->ifActivate_);
}

bool ExecGetAllActiveIfaces(GetAllActiveIfacesContext *context)
{
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->GetAllActiveIfaces(context->ethernetNameList_);
    if (result != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(result);
        NETMANAGER_EXT_LOGE("ExecIsIfaceActive error, errorCode: %{public}d", result);
        return false;
    }
    return true;
}

napi_value GetAllActiveIfacesCallback(GetAllActiveIfacesContext *context)
{
    napi_value ifaces = NapiUtils::CreateArray(context->GetEnv(), context->ethernetNameList_.size());
    uint32_t index = 0;
    for (const auto &iface : context->ethernetNameList_) {
        napi_value ifaceStr = NapiUtils::CreateStringUtf8(context->GetEnv(), iface);
        NapiUtils::SetArrayElement(context->GetEnv(), ifaces, index++, ifaceStr);
    }
    return ifaces;
}
} // namespace EthernetExec
} // namespace NetManagerStandard
} // namespace OHOS
