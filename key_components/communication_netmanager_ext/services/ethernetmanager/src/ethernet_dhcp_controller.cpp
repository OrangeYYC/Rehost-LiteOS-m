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

#include "ethernet_dhcp_controller.h"

#include <string>

#include "dhcp_define.h"
#include "dhcp_service.h"
#include "ethernet_dhcp_callback.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr int32_t DHCP_TIMEOUT = 300;
void EthernetDhcpController::EthernetDhcpControllerResultNotify::OnSuccess(int status, const std::string &ifname,
                                                                           OHOS::Wifi::DhcpResult &result)
{
    ethDhcpController_.OnDhcpSuccess(ifname, result);
}

void EthernetDhcpController::EthernetDhcpControllerResultNotify::OnFailed(int status, const std::string &ifname,
                                                                          const std::string &reason)
{
    return;
}

void EthernetDhcpController::EthernetDhcpControllerResultNotify::OnSerExitNotify(const std::string &ifname)
{
    return;
}

void EthernetDhcpController::RegisterDhcpCallback(sptr<EthernetDhcpCallback> callback)
{
    cbObject_ = callback;
}

void EthernetDhcpController::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_EXT_LOG_D("Start dhcp client iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    dhcpService_->StartDhcpClient(iface, bIpv6);
    if (dhcpService_->GetDhcpResult(iface, dhcpResultNotify_.get(), DHCP_TIMEOUT) != 0) {
        NETMGR_EXT_LOG_D(" Dhcp connection failed.\n");
    }
}

void EthernetDhcpController::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_EXT_LOG_D("Stop dhcp client iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    dhcpService_->StopDhcpClient(iface, bIpv6);
}

void EthernetDhcpController::OnDhcpSuccess(const std::string &iface, OHOS::Wifi::DhcpResult &result)
{
    if (cbObject_ == nullptr) {
        NETMGR_EXT_LOG_E("Error OnDhcpSuccess No Cb!");
        return;
    }
    EthernetDhcpCallback::DhcpResult dhcpResult;
    dhcpResult.iface = iface;
    dhcpResult.ipAddr = result.strYourCli;
    dhcpResult.gateWay = result.strRouter1;
    dhcpResult.subNet = result.strSubnet;
    dhcpResult.route1 = result.strRouter1;
    dhcpResult.route2 = result.strRouter2;
    dhcpResult.dns1 = result.strDns1;
    dhcpResult.dns2 = result.strDns2;
    cbObject_->OnDhcpSuccess(dhcpResult);
}

void EthernetDhcpController::OnDhcpFailed(int status, const std::string &ifname, const std::string &reason) {}
} // namespace NetManagerStandard
} // namespace OHOS
