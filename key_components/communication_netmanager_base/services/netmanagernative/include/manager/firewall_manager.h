/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_BASE_FIREWALL_MANAGER_H
#define NETMANAGER_BASE_FIREWALL_MANAGER_H

#include <iostream>
#include <map>
#include <mutex>
#include <vector>

#include "iptables_type.h"

namespace OHOS {
namespace nmd {
struct FirewallChainStatus {
    bool enable;
    NetManagerStandard::FirewallType type;
    std::vector<uint32_t> uids;
};

class FirewallManager {
public:
    FirewallManager();
    ~FirewallManager();
    /**
     * Set uids allowed list chain
     *
     * @param chain Chain type
     * @param uids Allowed list uids
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t SetUidsAllowedListChain(NetManagerStandard::ChainType chain, const std::vector<uint32_t> &uids);

    /**
     * Set uids denied list chain
     *
     * @param chain Chain type
     * @param uids Denied list uids
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t SetUidsDeniedListChain(NetManagerStandard::ChainType chain, const std::vector<uint32_t> &uids);

    /**
     * Enable chain
     *
     * @param chain Chain type
     * @param enable Enable or disable
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t EnableChain(NetManagerStandard::ChainType chain, bool enable);

    /**
     * Set uid rule
     *
     * @param chain Chain type
     * @param uid Uid
     * @param firewallRule Allow or deny
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t SetUidRule(NetManagerStandard::ChainType chain, uint32_t uid,
                       NetManagerStandard::FirewallRule firewallRule);

private:
    std::string FetchChainName(NetManagerStandard::ChainType chain);
    NetManagerStandard::FirewallType FetchChainType(NetManagerStandard::ChainType chain);
    int32_t InitChain();
    int32_t DeInitChain();
    int32_t InitDefaultRules();
    int32_t ClearAllRules();
    int32_t IptablesNewChain(NetManagerStandard::ChainType chain);
    int32_t IptablesDeleteChain(NetManagerStandard::ChainType chain);
    int32_t IptablesSetRule(const std::string &chainName, const std::string &option, const std::string &target,
                            uint32_t uid);
    std::string ReadMaxUidConfig();
    int32_t IsFirewallChian(NetManagerStandard::ChainType chain);
    inline void CheckChainInitialization();

private:
    bool chainInitFlag_;
    std::string strMaxUid_;
    std::mutex firewallMutex_;
    NetManagerStandard::FirewallType firewallType_;
    std::map<NetManagerStandard::ChainType, FirewallChainStatus> firewallChainStatus_;
};
} // namespace nmd
} // namespace OHOS
#endif // /* NETMANAGER_BASE */
