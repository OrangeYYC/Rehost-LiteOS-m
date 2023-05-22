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

#ifndef NETMANAGER_BASE_BANDWIDTH_MANAGER_H
#define NETMANAGER_BASE_BANDWIDTH_MANAGER_H

#include <iostream>
#include <map>
#include <mutex>
#include <vector>

#include "i_notify_callback.h"
#include "iptables_type.h"

namespace OHOS {
namespace nmd {
class BandwidthManager {
    enum Operate {
        OP_SET = 1,
        OP_UNSET = 2,
    };

public:
    BandwidthManager();
    ~BandwidthManager();

    /**
     * Enable data saver
     *
     * @param enable Enable or disable
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t EnableDataSaver(bool enable);

    /**
     * Set iface quota
     *
     * @param ifName Iface name
     * @param bytes Quota value
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t SetIfaceQuota(const std::string &ifName, int64_t bytes);

    /**
     * Remove iface quota
     *
     * @param ifName Iface name
     * @param bytes Quota value
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t RemoveIfaceQuota(const std::string &ifName);

    /**
     * Add denied list
     * @param ifName Iface name
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t AddDeniedList(uint32_t uid);

    /**
     * Remove denied list
     *
     * @param uid Uid
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t RemoveDeniedList(uint32_t uid);

    /**
     * Add allowed list
     *
     * @param uid Uid
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t AddAllowedList(uint32_t uid);

    /**
     * Remove allowed list
     *
     * @param uid Uid
     *
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t RemoveAllowedList(uint32_t uid);

private:
    std::string FetchChainName(NetManagerStandard::ChainType chain);
    int32_t InitChain();
    int32_t DeInitChain();
    int32_t InitDefaultBwChainRules();
    int32_t InitDefaultListBoxChainRules();
    int32_t InitDefaultAlertChainRules();
    int32_t InitDefaultRules();
    int32_t IptablesNewChain(NetManagerStandard::ChainType chain);
    int32_t IptablesNewChain(const std::string &chainName);
    int32_t IptablesDeleteChain(NetManagerStandard::ChainType chain);
    int32_t IptablesDeleteChain(const std::string &chainName);
    int32_t SetGlobalAlert(Operate operate, int64_t bytes);
    int32_t SetCostlyAlert(Operate operate, const std::string &iface, int64_t bytes);
    inline void CheckChainInitialization();

private:
    bool chainInitFlag_ = false;
    bool dataSaverEnable_ = false;
    int64_t globalAlertBytes_ = 0;
    std::mutex bandwidthMutex_;
    std::map<std::string, int64_t> ifaceAlertBytes_;
    std::map<std::string, int64_t> ifaceQuotaBytes_;
    std::vector<uint32_t> deniedListUids_;
    std::vector<uint32_t> allowedListUids_;
};
} // namespace nmd
} // namespace OHOS
#endif /* NETMANAGER_BASE_BANDWIDTH_MANAGER_H */
