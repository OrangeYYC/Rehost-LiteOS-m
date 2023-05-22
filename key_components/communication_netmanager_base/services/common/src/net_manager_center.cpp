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

#include "net_manager_center.h"

#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetManagerCenter &NetManagerCenter::GetInstance()
{
    static NetManagerCenter gInstance;
    return gInstance;
}

int32_t NetManagerCenter::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetIfaceNames(bearerType, ifaceNames);
}

int32_t NetManagerCenter::GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetIfaceNameByType(bearerType, ident, ifaceName);
}

int32_t NetManagerCenter::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                              const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
}

int32_t NetManagerCenter::UnregisterNetSupplier(uint32_t supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UnregisterNetSupplier(supplierId);
}

int32_t NetManagerCenter::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetLinkInfo(supplierId, netLinkInfo);
}

int32_t NetManagerCenter::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
}

void NetManagerCenter::RegisterConnService(const sptr<NetConnBaseService> &service)
{
    connService_ = service;
}

int32_t NetManagerCenter::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                              NetStatsInfo &info)
{
    if (statsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return statsService_->GetIfaceStatsDetail(iface, start, end, info);
}

int32_t NetManagerCenter::ResetStatsFactory()
{
    if (statsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return statsService_->ResetStatsFactory();
}

void NetManagerCenter::RegisterStatsService(const sptr<NetStatsBaseService> &service)
{
    statsService_ = service;
}

int32_t NetManagerCenter::ResetPolicyFactory()
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return ResetPolicies();
}

int32_t NetManagerCenter::ResetPolicies()
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return policyService_->ResetPolicies();
}

void NetManagerCenter::RegisterPolicyService(const sptr<NetPolicyBaseService> &service)
{
    policyService_ = service;
}

int32_t NetManagerCenter::ResetEthernetFactory()
{
    if (ethernetService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return ethernetService_->ResetEthernetFactory();
}

void NetManagerCenter::RegisterEthernetService(const sptr<NetEthernetBaseService> &service)
{
    ethernetService_ = service;
}

int32_t NetManagerCenter::GetAddressesByName(const std::string &hostName, int32_t netId,
                                             std::vector<INetAddr> &addrInfo)
{
    if (dnsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return dnsService_->GetAddressesByName(hostName, netId, addrInfo);
}

void NetManagerCenter::RegisterDnsService(const sptr<DnsBaseService> &service)
{
    dnsService_ = service;
}

int32_t NetManagerCenter::RestrictBackgroundChanged(bool isRestrictBackground)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RestrictBackgroundChanged(isRestrictBackground);
}

bool NetManagerCenter::IsUidNetAccess(uint32_t uid, bool metered)
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return IsUidNetAllowed(uid, metered);
}

bool NetManagerCenter::IsUidNetAllowed(uint32_t uid, bool metered)
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return policyService_->IsUidNetAllowed(uid, metered);
}
} // namespace NetManagerStandard
} // namespace OHOS
