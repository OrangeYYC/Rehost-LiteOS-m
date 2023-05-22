/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "conn_manager.h"

#include "local_network.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "physical_network.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
namespace {
constexpr int32_t INTERFACE_UNSET = -1;
constexpr int32_t LOCAL_NET_ID = 99;
} // namespace

ConnManager::ConnManager()
{
    networks_[LOCAL_NET_ID] = std::make_shared<LocalNetwork>(LOCAL_NET_ID);
    defaultNetId_ = 0;
    needReinitRouteFlag_ = false;
}

ConnManager::~ConnManager()
{
    networks_.clear();
}

int32_t ConnManager::CreatePhysicalNetwork(uint16_t netId, NetworkPermission permission)
{
    if (needReinitRouteFlag_) {
        std::set<int32_t> netIds;
        for (const auto &iter : networks_) {
            if (iter.first == LOCAL_NET_ID || iter.second == nullptr) {
                continue;
            }
            netIds.insert(iter.second->GetNetId());
        }
        for (auto netId : netIds) {
            RemoveInterfaceFromNetwork(netId, physicalInterfaceName_[netId]);
            DestroyNetwork(netId);
        }
        needReinitRouteFlag_ = false;
    }
    std::shared_ptr<NetsysNetwork> network = std::make_shared<PhysicalNetwork>(netId, permission);
    networks_[netId] = network;
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::DestroyNetwork(int32_t netId)
{
    if (netId == LOCAL_NET_ID) {
        NETNATIVE_LOGE("Cannot destroy local network");
        return NETMANAGER_ERROR;
    }
    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        if (defaultNetId_ == netId) {
            if (nw->IsPhysical()) {
                static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
            }
            defaultNetId_ = 0;
        }
        nw->ClearInterfaces();
    }
    networks_.erase(netId);
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::SetDefaultNetwork(int32_t netId)
{
    if (defaultNetId_ == netId) {
        return NETMANAGER_SUCCESS;
    }

    // check if this network exists
    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        if (!nw->IsPhysical()) {
            NETNATIVE_LOGE("SetDefaultNetwork fail, network :%{public}d is not physical ", netId);
            return NETMANAGER_ERROR;
        }
        static_cast<PhysicalNetwork *>(nw.get())->AddDefault();
    }

    if (defaultNetId_ != 0) {
        const auto &defaultNet = FindNetworkById(defaultNetId_);
        if (std::get<0>(defaultNet)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(defaultNet);
            if (!nw->IsPhysical()) {
                NETNATIVE_LOGE("SetDefaultNetwork fail, defaultNetId_ :%{public}d is not physical", defaultNetId_);
                return NETMANAGER_ERROR;
            }
            static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
        }
    }
    defaultNetId_ = netId;
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::ClearDefaultNetwork()
{
    if (defaultNetId_ != 0) {
        const auto &net = FindNetworkById(defaultNetId_);
        if (std::get<0>(net)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
            if (!nw->IsPhysical()) {
                NETNATIVE_LOGE("ClearDefaultNetwork fail, defaultNetId_ :%{public}d is not physical", defaultNetId_);
                return NETMANAGER_ERROR;
            }
            static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
        }
    }
    defaultNetId_ = 0;
    return NETMANAGER_SUCCESS;
}

std::tuple<bool, std::shared_ptr<NetsysNetwork>> ConnManager::FindNetworkById(int32_t netId)
{
    NETNATIVE_LOG_D("Entry ConnManager::FindNetworkById netId:%{public}d", netId);
    std::map<int32_t, std::shared_ptr<NetsysNetwork>>::iterator it;
    for (it = networks_.begin(); it != networks_.end(); ++it) {
        if (netId == it->first) {
            return std::make_tuple(true, it->second);
        }
    }
    return std::make_tuple<bool, std::shared_ptr<NetsysNetwork>>(false, nullptr);
}

int32_t ConnManager::GetDefaultNetwork() const
{
    return defaultNetId_;
}

int32_t ConnManager::GetNetworkForInterface(std::string &interfaceName)
{
    NETNATIVE_LOG_D("Entry ConnManager::GetNetworkForInterface interfaceName:%{public}s", interfaceName.c_str());
    std::map<int32_t, std::shared_ptr<NetsysNetwork>>::iterator it;
    for (it = networks_.begin(); it != networks_.end(); ++it) {
        if (it->second->ExistInterface(interfaceName)) {
            return it->first;
        }
    }
    return INTERFACE_UNSET;
}

int32_t ConnManager::AddInterfaceToNetwork(int32_t netId, std::string &interfaceName)
{
    NETNATIVE_LOG_D("Entry ConnManager::AddInterfaceToNetwork netId:%{public}d, interfaceName:%{public}s", netId,
                    interfaceName.c_str());
    int32_t alreadySetNetId = GetNetworkForInterface(interfaceName);
    if ((alreadySetNetId != netId) && (alreadySetNetId != INTERFACE_UNSET)) {
        NETNATIVE_LOGE("AddInterfaceToNetwork failed alreadySetNetId:%{public}d", alreadySetNetId);
        return NETMANAGER_ERROR;
    }

    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        physicalInterfaceName_[netId] = interfaceName;
        return nw->AddInterface(interfaceName);
    }
    return NETMANAGER_ERROR;
}

int32_t ConnManager::RemoveInterfaceFromNetwork(int32_t netId, std::string &interfaceName)
{
    int32_t alreadySetNetId = GetNetworkForInterface(interfaceName);
    if ((alreadySetNetId != netId) || (alreadySetNetId == INTERFACE_UNSET)) {
        return NETMANAGER_SUCCESS;
    } else if (alreadySetNetId == netId) {
        const auto &net = FindNetworkById(netId);
        if (std::get<0>(net)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
            int32_t ret = nw->RemoveInterface(interfaceName);
            physicalInterfaceName_.erase(netId);
            return ret;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::ReinitRoute()
{
    NETNATIVE_LOG_D("ConnManager::ReInitRoute");
    needReinitRouteFlag_ = true;
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::AddRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop)
{
    return RouteManager::AddRoute(GetTableType(netId), interfaceName, destination, nextHop);
}

int32_t ConnManager::RemoveRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop)
{
    return RouteManager::RemoveRoute(GetTableType(netId), interfaceName, destination, nextHop);
}

int32_t ConnManager::UpdateRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop)
{
    return RouteManager::UpdateRoute(GetTableType(netId), interfaceName, destination, nextHop);
}

RouteManager::TableType ConnManager::GetTableType(int32_t netId)
{
    RouteManager::TableType tableType;
    if (netId == LOCAL_NET_ID) {
        tableType = RouteManager::LOCAL_NETWORK;
    } else {
        tableType = RouteManager::INTERFACE;
    }
    return tableType;
}

int32_t ConnManager::GetFwmarkForNetwork(int32_t netId)
{
    return NETMANAGER_ERROR;
}

int32_t ConnManager::SetPermissionForNetwork(int32_t netId, NetworkPermission permission)
{
    return NETMANAGER_ERROR;
}

void ConnManager::GetDumpInfos(std::string &infos)
{
    static const std::string TAB = "  ";
    infos.append("Netsys connect manager :\n");
    infos.append(TAB + "default NetId: " + std::to_string(defaultNetId_) + "\n");
    std::for_each(networks_.begin(), networks_.end(), [&infos](const auto &network) {
        infos.append(TAB + "NetId:" + std::to_string(network.first));
        std::string interfaces = TAB + "interfaces: {";
        for (const auto &interface : network.second->GetAllInterface()) {
            interfaces.append(interface + ", ");
        }
        infos.append(interfaces + "}\n");
    });
}
} // namespace nmd
} // namespace OHOS
