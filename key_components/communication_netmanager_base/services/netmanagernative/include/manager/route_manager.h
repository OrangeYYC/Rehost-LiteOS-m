/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_ROUTE_MANAGER_H
#define INCLUDE_ROUTE_MANAGER_H

#include <linux/netlink.h>
#include <map>
#include <netinet/in.h>

#include "netlink_msg.h"
#include "network_permission.h"

namespace OHOS {
namespace nmd {
typedef struct RuleInfo {
    uint32_t ruleTable;
    uint32_t rulePriority;
    uint32_t ruleFwmark;
    uint32_t ruleMask;
    std::string ruleIif;
    std::string ruleOif;
} RuleInfo;

typedef struct RouteInfo {
    uint32_t routeTable;
    std::string routeInterfaceName;
    std::string routeDestinationName;
    std::string routeNextHop;
} RouteInfo;

typedef struct InetAddr {
    int32_t family;
    int32_t bitlen;
    int32_t prefixlen;
    uint8_t data[sizeof(struct in6_addr)];
} InetAddr;

class RouteManager {
public:
    RouteManager();
    ~RouteManager() = default;

    /**
     * Route table type
     *
     */
    enum TableType {
        INTERFACE,
        VPN_NETWORK,
        LOCAL_NETWORK,
    };

    /**
     * The interface is add route table
     *
     * @param tableType Route table type.Must be one of INTERFACE/VPN_NETWORK/LOCAL_NETWORK.
     * @param interfaceName Output network device name of the route item
     * @param destinationName Destination address of route item
     * @param nextHop Gateway address of the route item
     * @return Returns 0, add route table successfully, otherwise it will fail
     */
    static int32_t AddRoute(TableType tableType, const std::string &interfaceName, const std::string &destinationName,
                            const std::string &nextHop);

    /**
     * The interface is remove route table
     *
     * @param tableType Route table type.Must be one of INTERFACE/VPN_NETWORK/LOCAL_NETWORK.
     * @param interfaceName Output network device name of the route item
     * @param destinationName Destination address of route item
     * @param nextHop Gateway address of the route item
     * @return Returns 0, remove route table successfully, otherwise it will fail
     */
    static int32_t RemoveRoute(TableType tableType, const std::string &interfaceName,
                               const std::string &destinationName, const std::string &nextHop);

    /**
     * The interface is update route table
     *
     * @param tableType Route table type.Must be one of INTERFACE/VPN_NETWORK/LOCAL_NETWORK.
     * @param interfaceName Output network device name of the route item
     * @param destinationName Destination address of route item
     * @param nextHop Gateway address of the route item
     * @return Returns 0, update route table successfully, otherwise it will fail
     */
    static int32_t UpdateRoute(TableType tableType, const std::string &interfaceName,
                               const std::string &destinationName, const std::string &nextHop);

    /**
     * Add interface to default network
     *
     * @param interfaceName Output network device name of the route item
     * @param permission Network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @return Returns 0, add interface to default network successfully, otherwise it will fail
     */
    static int32_t AddInterfaceToDefaultNetwork(const std::string &interfaceName, NetworkPermission permission);

    /**
     * Remove interface from default network
     *
     * @param interfaceName Output network device name of the route item
     * @param permission Network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @return Returns 0, remove interface from default network  successfully, otherwise it will fail
     */
    static int32_t RemoveInterfaceFromDefaultNetwork(const std::string &interfaceName, NetworkPermission permission);

    /**
     * Add interface to physical network
     *
     * @param netId Network number
     * @param interfaceName Output network device name of the route item
     * @param permission Network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @return Returns 0, add interface to physical network successfully, otherwise it will fail
     */
    static int32_t AddInterfaceToPhysicalNetwork(uint16_t netId, const std::string &interfaceName,
                                                 NetworkPermission permission);

    /**
     * Remove interface from physical network
     *
     * @param netId Network number
     * @param interfaceName Output network device name of the route item
     * @param permission Network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @return Returns 0, remove interface from physical network successfully, otherwise it will fail
     */
    static int32_t RemoveInterfaceFromPhysicalNetwork(uint16_t netId, const std::string &interfaceName,
                                                      NetworkPermission permission);

    /**
     * Modify physical network permission
     *
     * @param netId Network number
     * @param interfaceName Output network device name of the route item
     * @param oldPermission Old network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @param newPermission New network permission. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM.
     * @return Returns 0, modify physical network permission successfully, otherwise it will fail
     */
    static int32_t ModifyPhysicalNetworkPermission(uint16_t netId, const std::string &interfaceName,
                                                   NetworkPermission oldPermission, NetworkPermission newPermission);

    /**
     * Add interface to local network
     *
     * @param netId Network number
     * @param interfaceName Output network device name of the route item
     * @return Returns 0, add interface to local network successfully, otherwise it will fail
     */
    static int32_t AddInterfaceToLocalNetwork(uint16_t netId, const std::string &interfaceName);

    /**
     * Remove interface from local network
     *
     * @param netId Network number
     * @param interfaceName Output network device name of the route item
     * @return Returns 0, remove interface from local network successfully, otherwise it will fail
     */
    static int32_t RemoveInterfaceFromLocalNetwork(uint16_t netId, const std::string &interfaceName);

    /**
     * Enable sharing network
     *
     * @param inputInterface Input network device name of the route item
     * @param outputInterface Output network device name of the route item
     * @return Returns 0, enable sharing network successfully, otherwise it will fail
     */
    static int32_t EnableSharing(const std::string &inputInterface, const std::string &outputInterface);

    /**
     * Disable sharing network
     *
     * @param inputInterface Input network device name of the route item
     * @param outputInterface Output network device name of the route item
     * @return Returns 0, disable sharing network successfully, otherwise it will fail
     */
    static int32_t DisableSharing(const std::string &inputInterface, const std::string &outputInterface);

    /**
     * Parse destination address
     *
     * @param addr Address to be parse
     * @param res Parse result
     * @return Returns 0, parse destination address successfully, otherwise it will fail
     */
    static int32_t ReadAddr(const std::string &addr, InetAddr *res);

    /**
     * Parse gateway address
     *
     * @param addr Address to be parse
     * @param res Parse result
     * @return Returns 0, parse gateway address successfully, otherwise it will fail
     */
    static int32_t ReadAddrGw(const std::string &addr, InetAddr *res);

private:
    static std::mutex interfaceToTableLock_;
    static std::map<std::string, uint32_t> interfaceToTable_;
    static int32_t Init();
    static int32_t ClearRules();
    static int32_t ClearRoutes(const std::string &interfaceName);
    static int32_t AddLocalNetworkRules();
    static int32_t UpdatePhysicalNetwork(uint16_t netId, const std::string &interfaceName,
                                         NetworkPermission permission, bool add);
    static int32_t UpdateLocalNetwork(uint16_t netId, const std::string &interfaceName, bool add);
    static int32_t UpdateIncomingPacketMark(uint16_t netId, const std::string &interfaceName,
                                            NetworkPermission permission, bool add);
    static int32_t UpdateExplicitNetworkRule(uint16_t netId, uint32_t table, NetworkPermission permission, bool add);
    static int32_t UpdateOutputInterfaceRules(const std::string &interfaceName, uint32_t table,
                                              NetworkPermission permission, bool add);
    static int32_t UpdateSharingNetwork(uint16_t action, const std::string &inputInterface,
                                        const std::string &outputInterface);
    static int32_t ClearSharingRules(const std::string &inputInterface);
    static int32_t UpdateRuleInfo(uint32_t action, uint8_t ruleType, RuleInfo ruleInfo);
    static int32_t SendRuleToKernel(uint32_t action, uint16_t ruleFlag, uint8_t ruleType, RuleInfo ruleInfo);
    static int32_t UpdateRouteRule(uint16_t action, uint16_t flags, RouteInfo routeInfo);
    static int32_t SendRouteToKernel(uint16_t action, uint16_t routeFlag, rtmsg msg, RouteInfo routeInfo,
                                     uint32_t index);
    static uint32_t FindTableByInterfacename(const std::string &interfaceName);
    static uint32_t GetRouteTableFromType(TableType tableType, const std::string &interfaceName);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_ROUTE_MANAGER_H
