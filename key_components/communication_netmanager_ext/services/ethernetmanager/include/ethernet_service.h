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

#ifndef ETHERNET_SERVICE_H
#define ETHERNET_SERVICE_H

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "ethernet_management.h"
#include "ethernet_service_common.h"
#include "ethernet_service_stub.h"
#include "refbase.h"
#include "singleton.h"
#include "system_ability.h"

namespace OHOS {
namespace NetManagerStandard {
class EthernetService : public SystemAbility,
                        public EthernetServiceStub,
                        public std::enable_shared_from_this<EthernetService> {
    DECLARE_DELAYED_SINGLETON(EthernetService)
    DECLARE_SYSTEM_ABILITY(EthernetService)

public:
    void OnStart() override;
    void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

    int32_t SetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ic) override;
    int32_t GetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ifaceConfig) override;
    int32_t IsIfaceActive(const std::string &iface, int32_t &activeStatus) override;
    int32_t GetAllActiveIfaces(std::vector<std::string> &activeIfaces) override;
    int32_t ResetFactory() override;
    int32_t SetInterfaceUp(const std::string &iface) override;
    int32_t SetInterfaceDown(const std::string &iface) override;
    int32_t GetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &config) override;
    int32_t SetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &cfg) override;

protected:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };
    bool Init();
    void InitManagement();

private:
    ServiceRunningState state_ = ServiceRunningState::STATE_STOPPED;
    bool registerToService_ = false;
    uint16_t dependentServiceState_ = 0;
    std::unique_ptr<EthernetManagement> ethManagement_;
    sptr<EthernetServiceCommon> serviceComm_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // ETHERNET_SERVICE_H
