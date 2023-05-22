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

#include "mdns_manager.h"

#include <unistd.h>

#include "mdns_event_proxy.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

MDnsManager::MDnsManager()
{
    InitHandler();
}

int32_t MDnsManager::RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        NETMGR_EXT_LOG_E("callback is nullptr");
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    if (std::find_if(registerMap_.begin(), registerMap_.end(), [cb](const auto &elem) {
            return elem.first->AsObject().GetRefPtr() == cb->AsObject().GetRefPtr();
        }) != registerMap_.end()) {
        return NET_MDNS_ERR_CALLBACK_DUPLICATED;
    }
    MDnsProtocolImpl::Result result{.serviceName = serviceInfo.name,
                                    .serviceType = serviceInfo.type,
                                    .port = serviceInfo.port,
                                    .txt = serviceInfo.txtRecord};
    int32_t err = impl.Register(result);
    impl.RunTaskLater([this, cb, serviceInfo, err]() {
        cb->HandleRegisterResult(serviceInfo, err);
        return true;
    });
    if (err == NETMANAGER_EXT_SUCCESS) {
        registerMap_.emplace_back(cb, serviceInfo.name + MDNS_DOMAIN_SPLITER_STR + serviceInfo.type);
    }
    return err;
}

int32_t MDnsManager::UnRegisterService(const sptr<IRegistrationCallback> &cb)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        NETMGR_EXT_LOG_E("callback is nullptr");
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    auto local = std::find_if(registerMap_.begin(), registerMap_.end(), [cb](const auto &elem) {
        return elem.first->AsObject().GetRefPtr() == cb->AsObject().GetRefPtr();
    });
    if (local == registerMap_.end()) {
        return NET_MDNS_ERR_CALLBACK_NOT_FOUND;
    }
    int32_t err = impl.UnRegister(local->second);
    if (err == NETMANAGER_EXT_SUCCESS) {
        registerMap_.erase(local);
    }
    return err;
}

int32_t MDnsManager::StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        NETMGR_EXT_LOG_E("callback is nullptr");
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    if (std::find_if(discoveryMap_.begin(), discoveryMap_.end(), [cb](const auto &elem) {
            return elem.first->AsObject().GetRefPtr() == cb->AsObject().GetRefPtr();
        }) != discoveryMap_.end()) {
        return NET_MDNS_ERR_CALLBACK_DUPLICATED;
    }
    int32_t err = impl.Discovery(serviceType);
    if (err == NETMANAGER_EXT_SUCCESS) {
        discoveryMap_.emplace_back(cb, serviceType);
    }
    return err;
}

int32_t MDnsManager::StopDiscoverService(const sptr<IDiscoveryCallback> &cb)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        NETMGR_EXT_LOG_E("callback is nullptr");
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    auto local = std::find_if(discoveryMap_.begin(), discoveryMap_.end(), [cb](const auto &elem) {
        return elem.first->AsObject().GetRefPtr() == cb->AsObject().GetRefPtr();
    });
    if (local == discoveryMap_.end()) {
        return NET_MDNS_ERR_CALLBACK_NOT_FOUND;
    }
    int32_t err = impl.StopDiscovery(local->second);
    if (err == NETMANAGER_EXT_SUCCESS) {
        discoveryMap_.erase(local);
    }
    return err;
}

int32_t MDnsManager::ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        NETMGR_EXT_LOG_E("callback is nullptr");
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    if (std::find_if(resolveMap_.begin(), resolveMap_.end(), [cb](const auto &elem) {
            return elem.first->AsObject().GetRefPtr() == cb->AsObject().GetRefPtr();
        }) != resolveMap_.end()) {
        return NET_MDNS_ERR_CALLBACK_DUPLICATED;
    }
    std::string instance = serviceInfo.name + MDNS_DOMAIN_SPLITER_STR + serviceInfo.type;
    int32_t err = impl.ResolveInstance(instance);
    if (err == NETMANAGER_EXT_SUCCESS) {
        resolveMap_.emplace_back(cb, instance);
    }
    return err;
}

void MDnsManager::InitHandler()
{
    static auto handle = [this](const MDnsProtocolImpl::Result &result, int32_t error) {
        ReceiveResult(result, error);
    };
    impl.SetHandler(handle);
}

void MDnsManager::ReceiveResult(const MDnsProtocolImpl::Result &result, int32_t error)
{
    NETMGR_EXT_LOG_D("ReceiveResult: %{public}d, error: %{public}d", result.type, error);
    switch (result.type) {
        case MDnsProtocolImpl::SERVICE_STARTED:
            [[fallthrough]];
        case MDnsProtocolImpl::SERVICE_STOPED:
            return ReceiveRegister(result, error);
        case MDnsProtocolImpl::SERVICE_FOUND:
            [[fallthrough]];
        case MDnsProtocolImpl::SERVICE_LOST:
            return ReceiveDiscover(result, error);
        case MDnsProtocolImpl::INSTANCE_RESOLVED:
            return ReceiveInstanceResolve(result, error);
        case MDnsProtocolImpl::DOMAIN_RESOLVED:
            [[fallthrough]];
        case MDnsProtocolImpl::UNKNOWN:
            [[fallthrough]];
        default:
            return;
    }
}

void MDnsManager::ReceiveRegister(const MDnsProtocolImpl::Result &result, int32_t error) {}

void MDnsManager::ReceiveDiscover(const MDnsProtocolImpl::Result &result, int32_t error)
{
    NETMGR_EXT_LOG_D("discoveryMap_ size: [%{public}zu]", discoveryMap_.size());
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto iter = discoveryMap_.begin(); iter != discoveryMap_.end(); ++iter) {
        if (iter->second != result.serviceType) {
            continue;
        }
        auto cb = iter->first;
        MDnsServiceInfo info;
        info.name = result.serviceName;
        info.type = result.serviceType;
        if (result.type == MDnsProtocolImpl::SERVICE_FOUND) {
            cb->HandleServiceFound(info, error);
        }
        if (result.type == MDnsProtocolImpl::SERVICE_LOST) {
            cb->HandleServiceLost(info, error);
        }
    }
}

void MDnsManager::ReceiveInstanceResolve(const MDnsProtocolImpl::Result &result, int32_t error)
{
    NETMGR_EXT_LOG_D("resolveMap_ size: [%{public}zu]", resolveMap_.size());
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto iter = resolveMap_.begin(); iter != resolveMap_.end();) {
        if (iter->second != result.serviceName + MDNS_DOMAIN_SPLITER_STR + result.serviceType) {
            ++iter;
            continue;
        }

        if (result.addr.empty()|| result.domain.empty()) {
            ++iter;
            continue;
        }

        auto cb = iter->first;
        impl.StopResolveInstance(result.serviceName + MDNS_DOMAIN_SPLITER_STR + result.serviceType);
        cb->HandleResolveResult(ConvertResultToInfo(result), error);
        iter = resolveMap_.erase(iter);
    }
}

MDnsServiceInfo MDnsManager::ConvertResultToInfo(const MDnsProtocolImpl::Result &result)
{
    MDnsServiceInfo info;
    info.name = result.serviceName;
    info.type = result.serviceType;
    if (!result.addr.empty()) {
        info.family = result.ipv6 ? MDnsServiceInfo::IPV6 : MDnsServiceInfo::IPV4;
    }
    info.addr = result.addr;
    info.port = result.port;
    info.txtRecord = result.txt;
    return info;
}

void MDnsManager::GetDumpMessage(std::string &message)
{
    message.append("mDNS Info:\n");
    const auto &config = impl.GetConfig();
    message.append("\tIPv6 Support: " + std::to_string(config.ipv6Support) + "\n");
    message.append("\tAll Iface: " + std::to_string(config.configAllIface) + "\n");
    message.append("\tTop Domain: " + config.topDomain + "\n");
    message.append("\tHostname: " + config.hostname + "\n");
    message.append("\tService Count: " + std::to_string(registerMap_.size()) + "\n");
}
} // namespace NetManagerStandard
} // namespace OHOS