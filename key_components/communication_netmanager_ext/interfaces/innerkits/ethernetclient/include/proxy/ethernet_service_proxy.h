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

#ifndef ETHERNET_SERVICE_PROXY_H
#define ETHERNET_SERVICE_PROXY_H

#include <string>

#include "i_ethernet_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class EthernetServiceProxy : public IRemoteProxy<IEthernetService> {
public:
    explicit EthernetServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IEthernetService>(impl) {}
    virtual ~EthernetServiceProxy() = default;
    bool WriteInterfaceToken(MessageParcel &data);

    int32_t SetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ic) override;
    int32_t GetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ifaceConfig) override;
    int32_t IsIfaceActive(const std::string &iface, int32_t &activeStatus) override;
    int32_t GetAllActiveIfaces(std::vector<std::string> &activeIfaces) override;
    int32_t ResetFactory() override;
    int32_t SetInterfaceUp(const std::string &iface) override;
    int32_t SetInterfaceDown(const std::string &iface) override;
    int32_t GetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &cfg) override;
    int32_t SetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &cfg) override;

private:
    static inline BrokerDelegator<EthernetServiceProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // ETHERNET_SERVICE_PROXY_H
