/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_CALLBACK_PROXY_H
#define NET_STATS_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "i_net_stats_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCallbackProxy : public IRemoteProxy<INetStatsCallback> {
public:
    explicit NetStatsCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetStatsCallbackProxy();
    int32_t NetIfaceStatsChanged(const std::string &iface) override;
    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<NetStatsCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CALLBACK_PROXY_H