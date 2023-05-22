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

#ifndef ETHERNET_MANAGEMENT_H
#define ETHERNET_MANAGEMENT_H

#include <map>
#include <mutex>

#include "dev_interface_state.h"
#include "ethernet_configuration.h"
#include "ethernet_dhcp_controller.h"
#include "iservice_registry.h"
#include "netsys_controller_callback.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetManagerStandard {
class EthernetManagement {
    class EhternetDhcpNotifyCallback : public EthernetDhcpCallback {
    public:
        EhternetDhcpNotifyCallback(EthernetManagement &ethernetManagement) : ethernetManagement_(ethernetManagement) {}
        int32_t OnDhcpSuccess(EthernetDhcpCallback::DhcpResult &dhcpResult) override;

    private:
        EthernetManagement &ethernetManagement_;
    };

private:
    class DevInterfaceStateCallback : public NetsysControllerCallback {
    public:
        DevInterfaceStateCallback(EthernetManagement &ethernetManagement) : ethernetManagement_(ethernetManagement) {}
        ~DevInterfaceStateCallback() = default;
        int32_t OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int) override;
        int32_t OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int) override;
        int32_t OnInterfaceAdded(const std::string &iface) override;
        int32_t OnInterfaceRemoved(const std::string &iface) override;
        int32_t OnInterfaceChanged(const std::string &, bool) override;
        int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) override;
        int32_t OnRouteChanged(bool, const std::string &, const std::string &, const std::string &) override;
        int32_t OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult) override;
        int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) override;

    private:
        EthernetManagement &ethernetManagement_;
    };

public:
    EthernetManagement();
    ~EthernetManagement();
    void Init();
    int32_t UpdateDevInterfaceLinkInfo(EthernetDhcpCallback::DhcpResult &dhcpResult);
    void UpdateInterfaceState(const std::string &dev, bool up);
    int32_t UpdateDevInterfaceCfg(const std::string &iface, sptr<InterfaceConfiguration> cfg);
    int32_t GetDevInterfaceCfg(const std::string &iface, sptr<InterfaceConfiguration> &ifaceConfig);
    int32_t IsIfaceActive(const std::string &iface, int32_t &activeStatus);
    int32_t GetAllActiveIfaces(std::vector<std::string> &activeIfaces);
    int32_t ResetFactory();
    void GetDumpInfo(std::string &info);
    void DevInterfaceAdd(const std::string &devName);
    void DevInterfaceRemove(const std::string &devName);

private:
    void StartDhcpClient(const std::string &dev, sptr<DevInterfaceState> &devState);
    void StopDhcpClient(const std::string &dev, sptr<DevInterfaceState> &devState);
    void StartSetDevUpThd();

private:
    std::map<std::string, std::set<NetCap>> devCaps_;
    std::map<std::string, sptr<InterfaceConfiguration>> devCfgs_;
    std::map<std::string, sptr<DevInterfaceState>> devs_;
    std::unique_ptr<EthernetConfiguration> ethConfiguration_ = nullptr;
    std::unique_ptr<EthernetDhcpController> ethDhcpController_ = nullptr;
    sptr<EhternetDhcpNotifyCallback> ethDhcpNotifyCallback_ = nullptr;
    sptr<NetsysControllerCallback> ethDevInterfaceStateCallback_ = nullptr;
    std::mutex mutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // ETHERNET_MANAGEMENT_H
