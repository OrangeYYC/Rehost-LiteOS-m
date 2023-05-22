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

#ifndef NETWORKSHARE_SERVICE_PROXY_H
#define NETWORKSHARE_SERVICE_PROXY_H

#include <string>

#include "iremote_proxy.h"
#include "i_networkshare_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareServiceProxy : public IRemoteProxy<INetworkShareService> {
public:
    explicit NetworkShareServiceProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetworkShareServiceProxy() = default;

    bool WriteInterfaceToken(MessageParcel &data);
    int32_t IsNetworkSharingSupported(int32_t &supported) override;
    int32_t IsSharing(int32_t &sharingStatus) override;
    int32_t StartNetworkSharing(const SharingIfaceType &type) override;
    int32_t StopNetworkSharing(const SharingIfaceType &type) override;
    int32_t GetSharableRegexs(SharingIfaceType type, std::vector<std::string> &ifaceRegexs) override;
    int32_t GetSharingState(SharingIfaceType type, SharingIfaceState &state) override;
    int32_t GetNetSharingIfaces(const SharingIfaceState &state, std::vector<std::string> &ifaces) override;
    int32_t RegisterSharingEvent(sptr<ISharingEventCallback> callback) override;
    int32_t UnregisterSharingEvent(sptr<ISharingEventCallback> callback) override;
    int32_t GetStatsRxBytes(int32_t &bytes) override;
    int32_t GetStatsTxBytes(int32_t &bytes) override;
    int32_t GetStatsTotalBytes(int32_t &bytes) override;

private:
    int32_t SendRequest(INetworkShareService::MessageCode code, MessageParcel &data, MessageParcel &reply);

private:
    static inline BrokerDelegator<NetworkShareServiceProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_SERVICE_PROXY_H
