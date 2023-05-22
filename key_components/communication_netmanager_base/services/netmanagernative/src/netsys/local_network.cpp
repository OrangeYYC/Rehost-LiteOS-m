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

#include "local_network.h"
namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;

LocalNetwork::LocalNetwork(uint16_t netId) : NetsysNetwork(netId) {}

int32_t LocalNetwork::AddInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry LocalNetwork::AddInterface %{public}s", interfaceName.c_str());
    if (ExistInterface(interfaceName)) {
        return NETMANAGER_SUCCESS;
    }

    if (RouteManager::AddInterfaceToLocalNetwork(netId_, interfaceName) != 0) {
        NETNATIVE_LOGE("Failed to add interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }

    interfaces_.insert(interfaceName);
    return NETMANAGER_SUCCESS;
}

int32_t LocalNetwork::RemoveInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry LocalNetwork::RemoveInterface %{public}s", interfaceName.c_str());
    if (!ExistInterface(interfaceName)) {
        return NETMANAGER_SUCCESS;
    }

    if (RouteManager::RemoveInterfaceFromLocalNetwork(netId_, interfaceName) != 0) {
        NETNATIVE_LOGE("Failed to remove interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }

    interfaces_.erase(interfaceName);
    return NETMANAGER_SUCCESS;
}
} // namespace nmd
} // namespace OHOS
