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

#ifndef NETWORKSHARE_SERVICE_H
#define NETWORKSHARE_SERVICE_H

#include "singleton.h"
#include "system_ability.h"

#include "errorcode_convertor.h"
#include "networkshare_service_stub.h"
#include "networkshare_tracker.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareService : public SystemAbility,
                            public NetworkShareServiceStub,
                            public std::enable_shared_from_this<NetworkShareService> {
    DECLARE_DELAYED_SINGLETON(NetworkShareService)
    DECLARE_SYSTEM_ABILITY(NetworkShareService)

    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

public:
    /**
     * service start
     */
    void OnStart() override;

    /**
     * service stop
     */
    void OnStop() override;

    /**
     * is surpport share network
     */
    int32_t IsNetworkSharingSupported(int32_t &supported) override;

    /**
     * has shared network
     */
    int32_t IsSharing(int32_t &sharingStatus) override;

    /**
     * start network by type
     */
    int32_t StartNetworkSharing(const SharingIfaceType &type) override;

    /**
     * stop network by type
     */
    int32_t StopNetworkSharing(const SharingIfaceType &type) override;

    /**
     * get sharable regex
     */
    int32_t GetSharableRegexs(SharingIfaceType type, std::vector<std::string> &ifaceRegexs) override;

    /**
     * get sharing type
     */
    int32_t GetSharingState(SharingIfaceType type, SharingIfaceState &state) override;

    /**
     * get sharing ifaces
     */
    int32_t GetNetSharingIfaces(const SharingIfaceState &state, std::vector<std::string> &ifaces) override;

    /**
     * register callback
     */
    int32_t RegisterSharingEvent(sptr<ISharingEventCallback> callback) override;

    /**
     * unregister callback
     */
    int32_t UnregisterSharingEvent(sptr<ISharingEventCallback> callback) override;

    /**
     * get downlink data bytes
     */
    int32_t GetStatsRxBytes(int32_t &bytes) override;

    /**
     * get uplink data bytes
     */
    int32_t GetStatsTxBytes(int32_t &bytes) override;

    /**
     * get total data bytes
     */
    int32_t GetStatsTotalBytes(int32_t &bytes) override;

    /**
     * dump function
     */
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

private:
    bool Init();
    void GetDumpMessage(std::string &message);
    void GetSharingType(const SharingIfaceType &type, const std::string &typeContent, std::string &sharingType);
    void GetShareRegexsContent(const SharingIfaceType &type, std::string &shareRegexsContent);

private:
    ServiceRunningState state_ = ServiceRunningState::STATE_STOPPED;
    bool registerToService_ = false;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_SERVICE_H
