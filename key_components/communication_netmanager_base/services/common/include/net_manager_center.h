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

#ifndef NET_MANAGER_CENTER_H
#define NET_MANAGER_CENTER_H

#include <set>

#include "dns_base_service.h"
#include "net_all_capabilities.h"
#include "net_conn_base_service.h"
#include "net_ethernet_base_service.h"
#include "net_policy_base_service.h"
#include "net_stats_base_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetManagerCenter {
public:
    static NetManagerCenter &GetInstance();
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames);
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName);
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId);
    int32_t UnregisterNetSupplier(uint32_t supplierId);
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo);
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo);
    void RegisterConnService(const sptr<NetConnBaseService> &service);

    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end, NetStatsInfo &info);
    int32_t ResetStatsFactory();
    void RegisterStatsService(const sptr<NetStatsBaseService> &service);

    int32_t ResetPolicyFactory();
    int32_t ResetPolicies();
    void RegisterPolicyService(const sptr<NetPolicyBaseService> &service);

    int32_t ResetEthernetFactory();
    void RegisterEthernetService(const sptr<NetEthernetBaseService> &service);

    int32_t GetAddressesByName(const std::string &hostName, int32_t netId, std::vector<INetAddr> &addrInfo);
    void RegisterDnsService(const sptr<DnsBaseService> &service);

    int32_t RestrictBackgroundChanged(bool isRestrictBackground);
    bool IsUidNetAccess(uint32_t uid, bool metered);
    bool IsUidNetAllowed(uint32_t uid, bool metered);

private:
    sptr<NetConnBaseService> connService_;
    sptr<NetStatsBaseService> statsService_;
    sptr<NetPolicyBaseService> policyService_;
    sptr<NetEthernetBaseService> ethernetService_;
    sptr<DnsBaseService> dnsService_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MANAGER_CENTER_H
