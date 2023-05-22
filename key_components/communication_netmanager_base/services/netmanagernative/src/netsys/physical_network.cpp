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

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "route_manager.h"

#include "physical_network.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;

PhysicalNetwork::PhysicalNetwork(uint16_t netId, NetworkPermission permission)
    : NetsysNetwork(netId), permission_(permission)
{
}

void PhysicalNetwork::AddDefault()
{
    std::set<std::string>::iterator it;
    for (it = interfaces_.begin(); it != interfaces_.end(); ++it) {
        RouteManager::AddInterfaceToDefaultNetwork(*it, permission_);
    }
    isDefault_ = true;
}

void PhysicalNetwork::RemoveDefault()
{
    std::set<std::string>::iterator it;
    for (it = interfaces_.begin(); it != interfaces_.end(); ++it) {
        RouteManager::RemoveInterfaceFromDefaultNetwork(*it, permission_);
    }
    isDefault_ = false;
}

int32_t PhysicalNetwork::AddInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry PhysicalNetwork::AddInterface %{public}s", interfaceName.c_str());
    if (ExistInterface(interfaceName)) {
        return NETMANAGER_SUCCESS;
    }

    if (RouteManager::AddInterfaceToPhysicalNetwork(netId_, interfaceName, permission_) != 0) {
        NETNATIVE_LOGE("Failed to add interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }

    if (isDefault_) {
        RouteManager::AddInterfaceToDefaultNetwork(interfaceName, permission_);
    }

    interfaces_.insert(interfaceName);

    return NETMANAGER_SUCCESS;
}

int32_t PhysicalNetwork::RemoveInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry PhysicalNetwork::RemoveInterface %{public}s", interfaceName.c_str());
    if (!ExistInterface(interfaceName)) {
        return NETMANAGER_SUCCESS;
    }

    if (isDefault_) {
        RouteManager::RemoveInterfaceFromDefaultNetwork(interfaceName, permission_);
    }

    if (RouteManager::RemoveInterfaceFromPhysicalNetwork(netId_, interfaceName, permission_) != 0) {
        NETNATIVE_LOGE("Failed to remove interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }

    interfaces_.erase(interfaceName);
    return NETMANAGER_SUCCESS;
}
} // namespace nmd
} // namespace OHOS
