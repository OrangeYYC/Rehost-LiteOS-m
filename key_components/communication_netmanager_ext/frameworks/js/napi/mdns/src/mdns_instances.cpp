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

#include <mutex>

#include "mdns_instances.h"
#include "net_manager_constants.h"
#include "netmanager_ext_log.h"

namespace OHOS::NetManagerStandard {
std::map<MDnsDiscoveryObserver *, MDnsDiscoveryInstance *> MDnsDiscoveryInstance::discoverInstanceMap_;
std::mutex g_mDNSDiscoverMutex;

MDnsDiscoveryInstance::MDnsDiscoveryInstance(EventManager *eventManager)
    : observer_(new (std::nothrow) MDnsDiscoveryObserver), manager_(eventManager)
{
}

MDnsDiscoveryInstance *MDnsDiscoveryInstance::MakeMDnsDiscovery(EventManager *eventManager)
{
    std::lock_guard<std::mutex> lock(g_mDNSDiscoverMutex);
    auto mdnsDiscovery = new MDnsDiscoveryInstance(eventManager);
    if (mdnsDiscovery->observer_ == nullptr) {
        NETMANAGER_EXT_LOGE("mdnsDiscovery->observer_ is nullptr");
        return nullptr;
    }
    discoverInstanceMap_[mdnsDiscovery->observer_.GetRefPtr()] = mdnsDiscovery;
    return mdnsDiscovery;
}

void MDnsDiscoveryInstance::DeleteMDnsDiscovery(MDnsDiscoveryInstance *mdnsDiscovery)
{
    std::lock_guard<std::mutex> lock(g_mDNSDiscoverMutex);
    if (mdnsDiscovery->observer_ == nullptr) {
        NETMANAGER_EXT_LOGE("mdnsDiscovery->observer_ is nullptr");
        return;
    }
    discoverInstanceMap_.erase(mdnsDiscovery->observer_.GetRefPtr());
    delete mdnsDiscovery;
}

sptr<MDnsDiscoveryObserver> MDnsDiscoveryInstance::GetObserver() const
{
    return observer_;
}

EventManager *MDnsDiscoveryInstance::GetEventManager() const
{
    return manager_;
}
} // namespace OHOS::NetManagerStandard
