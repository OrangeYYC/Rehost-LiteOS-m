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

#ifndef INCLUDE_CONN_MANAGER_H
#define INCLUDE_CONN_MANAGER_H

#include <map>
#include <set>
#include <vector>

#include "netsys_network.h"
#include "network_permission.h"
#include "route_manager.h"

namespace OHOS {
namespace nmd {
class ConnManager {
public:
    enum RouteAction {
        ROUTE_ADD,
        ROUTE_REMOVE,
        ROUTE_UPDATE,
    };

    ConnManager();
    ~ConnManager();

    /**
     * Creates a physical network
     *
     * @param netId The network Id to create
     * @param permission The permission necessary to use the network. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM
     *
     * @return Returns 0, successfully create the physical network, otherwise it will fail
     */
    int32_t CreatePhysicalNetwork(uint16_t netId, NetworkPermission permission);

    /**
     * Destroy a network. Any interfaces added to the network are removed, and the network ceases
     *        to be the default network
     *
     * @param netId The network to destroy
     *
     * @return Returns 0, successfully destroy the network, otherwise it will fail
     */
    int32_t DestroyNetwork(int32_t netId);

    /**
     * Set network as default network
     *
     * @param netId The network to set as the default
     *
     * @return Returns 0, successfully Set default network, otherwise it will fail
     */
    int32_t SetDefaultNetwork(int32_t netId);

    /**
     * Clear default network
     *
     * @return Returns 0, successfully clear default network, otherwise it will fail
     */
    int32_t ClearDefaultNetwork();

    /**
     * Get default network
     *
     * @return NetId of default network
     */
    int32_t GetDefaultNetwork() const;

    /**
     * Add an interface to a network. The interface must not be assigned to any network, including
     *        the specified network
     *
     * @param netId The network to add the interface
     * @param interafceName The name of the interface to add
     *
     * @return Returns 0, successfully add an interface to a network, otherwise it will fail
     */
    int32_t AddInterfaceToNetwork(int32_t netId, std::string &interafceName);

    /**
     * Remove an interface to a network. The interface must be assigned to the specified network
     *
     * @param netId The network to add the interface
     * @param interafceName The name of the interface to remove
     *
     * @return Returns 0, successfully remove an interface to a network, otherwise it will fail
     */
    int32_t RemoveInterfaceFromNetwork(int32_t netId, std::string &interafceName);

    /**
     * Reinit route when netmanager restart
     *
     * @param
     *
     * @return Returns 0, reinit route successfully, otherwise it will fail
     */
    int32_t ReinitRoute();

    /**
     * Add a route for specific network
     *
     * @param netId The network to add the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     *
     * @return Returns 0, successfully add a route for specific network, otherwise it will fail
     */
    int32_t AddRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop);

    /**
     * Remove a route for specific network
     *
     * @param netId The network to remove the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     *
     * @return Returns 0, successfully remove a route for specific network, otherwise it will fail
     */
    int32_t RemoveRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop);

    /**
     * Update a route for specific network
     *
     * @param netId The network to update the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     *
     * @return Returns 0, successfully update a route for specific network, otherwise it will fail
     */
    int32_t UpdateRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop);

    /**
     * Get the mark for the given network id
     *
     * @param netId The network to get the mark
     *
     * @return A Mark of the given network id.
     */
    int32_t GetFwmarkForNetwork(int32_t netId);

    /**
     * Set the permission required to access a specific network
     *
     * @param netId The network to set
     * @param permission Network permission to use
     *
     * @return Returns 0, successfully set the permission for specific network, otherwise it will fail
     */
    int32_t SetPermissionForNetwork(int32_t netId, NetworkPermission permission);

    /**
     * Get the Dump Infos object
     *
     * @param infos The output message
     */
    void GetDumpInfos(std::string &infos);

private:
    int32_t defaultNetId_;
    bool needReinitRouteFlag_;
    std::map<int32_t, std::string> physicalInterfaceName_;
    std::map<int32_t, std::shared_ptr<NetsysNetwork>> networks_;
    std::tuple<bool, std::shared_ptr<NetsysNetwork>> FindNetworkById(int32_t netId);
    int32_t GetNetworkForInterface(std::string &interfaceName);
    RouteManager::TableType GetTableType(int32_t netId);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_CONN_MANAGER_H
