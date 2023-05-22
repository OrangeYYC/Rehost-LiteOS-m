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

#ifndef INCLUDE_SHARING_MANAGER_H
#define INCLUDE_SHARING_MANAGER_H

#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "iptables_wrapper.h"
#include "network_sharing.h"

namespace OHOS {
namespace nmd {
class SharingManager {
public:
    SharingManager();
    ~SharingManager() = default;

    /*
     * @brief Enable IP forwarding
     *
     * @param requestor
     * @return NETMANAGER_ERROR code
     */
    int32_t IpEnableForwarding(const std::string &requestor);

    /*
     * @brief Disable IP forwarding
     *
     * @param requestor
     * @return NETMANAGER_ERROR code
     */
    int32_t IpDisableForwarding(const std::string &requestor);

    /*
     * @brief Enable network address forwarding
     *
     * @param downstreamIface
     * @param upstreamIface
     * @return NETMANAGER_ERROR code
     */
    int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface);

    /*
     * @brief Disable network address forwarding
     *
     * @param downstreamIface
     * @param upstreamIface
     * @return NETMANAGER_ERROR code
     */
    int32_t DisableNat(const std::string &downstramIface, const std::string &upstreamIface);

    /*
     * @brief According to the network cark configuration rules of iptables
     *
     * @param fromIface
     * @param toIface
     * @return NETMANAGER_ERROR code
     */
    int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface);

    /*
     * @brief According to the network cark configuration rules of iptables
     *
     * @param fromIface
     * @param toIface
     * @return NETMANAGER_ERROR code
     */
    int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface);

    /*
     * @brief Get trafic of network sharing
     *
     * @param downIface
     * @param upIface
     * @param traffic
     * @return NETMANAGER_ERROR code
     */
    int32_t GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                     NetworkSharingTraffic &traffic);

private:
    std::set<std::string> forwardingRequests_;
    std::set<std::string> interfaceForwards_;
    std::shared_ptr<IptablesWrapper> iptablesWrapper_ = nullptr;
    bool inited_ = false;
    std::mutex initedMutex_;

    void InitChildChains();
    void CheckInited();
    int32_t SetIpFwdEnable();
    int32_t SetForwardRules(bool set, const std::string &cmds);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_SHARING_MANAGER_H
