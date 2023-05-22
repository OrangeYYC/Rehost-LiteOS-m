/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "route_utils.h"

#include <arpa/inet.h>

#include "net_mgr_log_wrapper.h"
#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *IPV6_DEFAULT_PRIFX = "fe80::";
static constexpr const char *IPV6_DEFAULT_GATEWAY = "::";
static constexpr int32_t IPV6_PRIFX_LEN = 64;
static constexpr int32_t IPV4_PRIFX_LEN = 32;
static constexpr int32_t IPV6_UINT_LEN = 16;
static constexpr int32_t IPV4_UINT_LEN = 4;
static constexpr int32_t IP_PER_UINT_SIZE = 8;
static constexpr int32_t IP_PER_UINT_MASK = 0xFF;

int32_t RouteUtils::AddRoutesToLocal(const std::string &iface, const std::list<Route> &routes)
{
    std::list<Route>::const_iterator iter;
    for (iter = routes.begin(); iter != routes.end(); ++iter) {
        if (!(iter->rtnType_ == RTN_UNICAST && iter->destination_.prefixlen_ == 0)) {
            NETMGR_LOG_D("AddRoutesToLocalNetwork: dest addr[%{public}s], gw addr[%{public}s]",
                         iter->destination_.address_.c_str(), iter->gateway_.address_.c_str());
            AddRoute(LOCAL_NET_ID, *iter);
        }
    }

    Route ipv6Rt;
    ipv6Rt.iface_ = iface;
    ipv6Rt.destination_.type_ = INetAddr::IPV6;
    ipv6Rt.destination_.address_ = IPV6_DEFAULT_PRIFX;
    ipv6Rt.destination_.prefixlen_ = IPV6_PRIFX_LEN;
    ipv6Rt.gateway_.address_ = IPV6_DEFAULT_GATEWAY;
    ipv6Rt.gateway_.prefixlen_ = 0;
    ipv6Rt.hasGateway_ = false;
    ipv6Rt.rtnType_ = RTN_UNICAST;
    ipv6Rt.isDefaultRoute_ = false;
    return AddRoute(LOCAL_NET_ID, ipv6Rt);
}

int32_t RouteUtils::RemoveRoutesFromLocal(const std::list<Route> &routes)
{
    std::list<Route>::const_iterator iter;
    for (iter = routes.begin(); iter != routes.end(); ++iter) {
        RemoveRoute(LOCAL_NET_ID, *iter);
    }

    return 0;
}

int32_t RouteUtils::AddRoute(int32_t netId, const Route &route)
{
    return ModifyRoute(ROUTE_ADD, netId, route);
}

int32_t RouteUtils::RemoveRoute(int32_t netId, const Route &route)
{
    return ModifyRoute(ROUTE_REMOVE, netId, route);
}

int32_t RouteUtils::UpdateRoutes(int32_t netId, const NetLinkInfo &newnl, const NetLinkInfo &oldnl)
{
    std::list<Route> added;
    std::list<Route> updated;
    std::list<Route> removed;
    std::list<Route>::const_iterator itero;
    std::list<Route>::const_iterator itern;
    std::list<Route>::const_iterator iterf;

    for (itero = oldnl.routeList_.begin(); itero != oldnl.routeList_.end(); ++itero) {
        iterf = std::find(newnl.routeList_.begin(), newnl.routeList_.end(), *itero);
        if (iterf == newnl.routeList_.end()) {
            removed.push_back(*itero);
        } else {
            if (itero->rtnType_ != iterf->rtnType_ || itero->mtu_ != iterf->mtu_) {
                updated.push_back(*iterf);
            }
        }
    }

    for (itern = newnl.routeList_.begin(); itern != newnl.routeList_.end(); ++itern) {
        if (std::find(oldnl.routeList_.begin(), oldnl.routeList_.end(), *itern) == oldnl.routeList_.end()) {
            added.push_back(*itern);
        }
    }

    for (itern = added.begin(); itern != added.end(); ++itern) {
        if (itern->hasGateway_) {
            continue;
        }
        AddRoute(netId, *itern);
    }

    for (itern = added.begin(); itern != added.end(); ++itern) {
        if (!itern->hasGateway_) {
            continue;
        }
        AddRoute(netId, *itern);
    }

    for (itern = removed.begin(); itern != removed.end(); ++itern) {
        RemoveRoute(netId, *itern);
    }

    return (!added.empty() || !updated.empty() || !removed.empty()) ? 1 : 0;
}

int32_t RouteUtils::ModifyRoute(routeOperateType op, int32_t netId, const Route &route)
{
    int32_t ret = -1;
    std::string nextHop;
    std::string dest;

    NETMGR_LOG_D("ModifyRoute: netId[%{public}d], dest addr[%{public}s], gw addr[%{public}s]", netId,
                 route.destination_.address_.c_str(), route.gateway_.address_.c_str());

    switch (route.rtnType_) {
        case RTN_UNICAST:
            if (route.hasGateway_) {
                nextHop = route.gateway_.address_;
            }
            break;
        case RTN_UNREACHABLE:
            nextHop = NEXTHOP_UNREACHABLE;
            break;
        case RTN_THROW:
            nextHop = NEXTHOP_THROW;
            break;
        default:
            break;
    }

    ToPrefixString(route.destination_.address_, route.destination_.prefixlen_, dest);
    if (dest.empty()) {
        NETMGR_LOG_E("route dest is empty.");
        return ret;
    }

    switch (op) {
        case ROUTE_ADD:
            ret = NetsysController::GetInstance().NetworkAddRoute(netId, route.iface_, dest, nextHop);
            break;
        case ROUTE_REMOVE:
            ret = NetsysController::GetInstance().NetworkRemoveRoute(netId, route.iface_, dest, nextHop);
            break;
        default:
            break;
    }

    return ret;
}

void RouteUtils::ToPrefixString(const std::string &src, int32_t prefixLen, std::string &dest)
{
    dest = MaskAddress(src, prefixLen);
    NETMGR_LOG_D("ToPrefixString: src addr[%{public}s], src prefixlen[%{public}d], mask dest addr[%{public}s]",
                 src.c_str(), prefixLen, dest.c_str());
    if (!dest.empty()) {
        dest += "/";
        dest += std::to_string(prefixLen);
    }
}

std::string RouteUtils::MaskAddress(const std::string &addr, int32_t prefixLen)
{
    int32_t netScope = 0;
    int32_t hostScope = 0;
    uint8_t mask = 0;
    uint8_t array[IPV6_UINT_LEN] = {0};
    char str[INET6_ADDRSTRLEN] = {0};
    int32_t af = prefixLen > IPV4_PRIFX_LEN ? AF_INET6 : AF_INET;
    int32_t arrayLen = (af == AF_INET ? IPV4_UINT_LEN : IPV6_UINT_LEN);

    netScope = prefixLen / IP_PER_UINT_SIZE;
    hostScope = prefixLen % IP_PER_UINT_SIZE;
    mask = (uint8_t)(IP_PER_UINT_MASK << (IP_PER_UINT_SIZE - hostScope));

    if (inet_pton(af, addr.c_str(), array) != 1) {
        return std::string("");
    }

    if (netScope < arrayLen) {
        array[netScope] = (uint8_t)(array[netScope] & mask);
    }

    netScope++;
    for (; netScope < arrayLen; netScope++) {
        array[netScope] = 0;
    }

    if (inet_ntop(af, array, str, INET6_ADDRSTRLEN) == nullptr) {
        return std::string("");
    }

    return std::string(str);
}
} // namespace NetManagerStandard
} // namespace OHOS
