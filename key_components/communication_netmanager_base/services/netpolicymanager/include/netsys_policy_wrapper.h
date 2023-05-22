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

#ifndef NETSYS_POLICY_WRAPPER_H
#define NETSYS_POLICY_WRAPPER_H

#include <iostream>

#include "singleton.h"

#include "net_mgr_log_wrapper.h"
#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
class NetsysPolicyWrapper {
    DECLARE_DELAYED_SINGLETON(NetsysPolicyWrapper)

public:
    int32_t RegisterNetsysCallback(sptr<NetsysControllerCallback> callback);
    int32_t UnregisterNetsysCallback(sptr<NetsysControllerCallback> callback);
    int32_t BandwidthEnableDataSaver(bool enable);
    int32_t BandwidthSetIfaceQuota(const std::string &iface, int64_t bytes);
    int32_t BandwidthRemoveIfaceQuota(const std::string &iface);
    int32_t BandwidthAddDeniedList(uint32_t uid);
    int32_t BandwidthRemoveDeniedList(uint32_t uid);
    int32_t BandwidthAddAllowedList(uint32_t uid);
    int32_t BandwidthRemoveAllowedList(uint32_t uid);
    int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule);
    int32_t FirewallEnableChain(uint32_t chain, bool enable);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETSYS_POLICY_WRAPPER_H
