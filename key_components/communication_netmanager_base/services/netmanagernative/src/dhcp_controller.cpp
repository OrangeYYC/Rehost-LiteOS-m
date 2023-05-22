/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "dhcp_controller.h"

#include "dhcp_result_parcel.h"
#include "netnative_log_wrapper.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace nmd {
constexpr int32_t DHCP_TIMEOUT = 300;
static constexpr const char *DEFAULT_STR_SUBNET = "255.255.255.0";
static constexpr const char *DEFAULT_STR_STARTIP = ".3";
static constexpr const char *DEFAULT_STR_ENDIP = ".254";
DhcpController::DhcpControllerResultNotify::DhcpControllerResultNotify(DhcpController &dhcpController)
    : dhcpController_(dhcpController)
{
}

DhcpController::DhcpControllerResultNotify::~DhcpControllerResultNotify() {}

void DhcpController::DhcpControllerResultNotify::OnSuccess(int status, const std::string &ifname,
                                                           OHOS::Wifi::DhcpResult &result)
{
    NETNATIVE_LOG_D(
        "Enter DhcpController::DhcpControllerResultNotify::OnSuccess "
        "ifname=[%{public}s], iptype=[%{public}d], strYourCli=[%{public}s], "
        "strServer=[%{public}s], strSubnet=[%{public}s], strDns1=[%{public}s], "
        "strDns2=[%{public}s] strRouter1=[%{public}s] strRouter2=[%{public}s]",
        ifname.c_str(), result.iptype, result.strYourCli.c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strServer).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strSubnet).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strDns1).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strDns2).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strRouter1).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result.strRouter2).c_str());
    dhcpController_.Process(ifname, result);
}

void DhcpController::DhcpControllerResultNotify::OnFailed(int status, const std::string &ifname,
                                                          const std::string &reason)
{
    NETNATIVE_LOGE("Enter DhcpController::DhcpControllerResultNotify::OnFailed");
}

void DhcpController::DhcpControllerResultNotify::OnSerExitNotify(const std::string &ifname)
{
    NETNATIVE_LOGE("DhcpController::DhcpControllerResultNotify::OnSerExitNotify");
}

DhcpController::DhcpController()
{
    dhcpResultNotify_ = std::make_unique<DhcpControllerResultNotify>(*this);
}

DhcpController::~DhcpController() {}

int32_t DhcpController::RegisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback)
{
    NETNATIVE_LOGI("DhcpController RegisterNotifyCallback");
    callback_ = callback;
    return 0;
}

void DhcpController::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOGI("DhcpController StartDhcpClient iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    if (OHOS::Wifi::DhcpServiceApi::GetInstance() == nullptr) {
        NETNATIVE_LOGE("OHOS::Wifi::DhcpServiceApi is nullptr");
        return;
    }
    OHOS::Wifi::DhcpServiceApi::GetInstance()->StartDhcpClient(iface, bIpv6);
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->GetDhcpResult(iface, dhcpResultNotify_.get(), DHCP_TIMEOUT) != 0) {
        NETNATIVE_LOGE(" Dhcp connection failed");
    }
}

void DhcpController::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOGI("DhcpController StopDhcpClient iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    if (OHOS::Wifi::DhcpServiceApi::GetInstance() == nullptr) {
        NETNATIVE_LOGE("OHOS::Wifi::DhcpServiceApi is nullptr");
        return;
    }
    OHOS::Wifi::DhcpServiceApi::GetInstance()->StopDhcpClient(iface, bIpv6);
}

void DhcpController::Process(const std::string &iface, OHOS::Wifi::DhcpResult &result)
{
    NETNATIVE_LOGI("DhcpController Process");
    sptr<OHOS::NetsysNative::DhcpResultParcel> ptr = new (std::nothrow) OHOS::NetsysNative::DhcpResultParcel();
    if (ptr == nullptr) {
        return;
    }
    
    ptr->iface_ = iface;
    ptr->ipAddr_ = result.strYourCli;
    ptr->gateWay_ = result.strServer;
    ptr->subNet_ = result.strSubnet;
    ptr->route1_ = result.strRouter1;
    ptr->route2_ = result.strRouter2;
    ptr->dns1_ = result.strDns1;
    ptr->dns2_ = result.strDns2;
    callback_->OnDhcpSuccess(ptr);
}

bool DhcpController::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    constexpr int32_t IP_V4 = 0;
    if (OHOS::Wifi::DhcpServiceApi::GetInstance() == nullptr) {
        NETNATIVE_LOGE("OHOS::Wifi::DhcpServiceApi is nullptr");
        return false;
    }
    std::string ipAddr = ipv4addr;
    std::string::size_type pos = ipAddr.rfind(".");
    if (pos == std::string::npos) {
        return false;
    }
    std::string ipHead = ipAddr.substr(0, pos);
    OHOS::Wifi::DhcpRange range;
    range.iptype = IP_V4;
    range.strStartip = ipHead + DEFAULT_STR_STARTIP;
    range.strEndip = ipHead + DEFAULT_STR_ENDIP;
    range.strSubnet = DEFAULT_STR_SUBNET;
    range.strTagName = iface;
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->SetDhcpRange(iface, range) != 0) {
        return false;
    }
    NETNATIVE_LOGI(
        "Set dhcp range : ifaceName[%{public}s] TagName[%{public}s] start ip[%{public}s] end ip[%{public}s]",
        iface.c_str(), range.strTagName.c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(range.strStartip).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(range.strEndip).c_str());
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->StartDhcpServer(iface) != 0) {
        return false;
    }
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->GetDhcpSerProExit(iface, dhcpResultNotify_.get())) {
        return false;
    }
    return true;
}
bool DhcpController::StopDhcpService(const std::string &iface)
{
    if (OHOS::Wifi::DhcpServiceApi::GetInstance() == nullptr) {
        NETNATIVE_LOGE("OHOS::Wifi::DhcpServiceApi is nullptr");
        return false;
    }
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->RemoveAllDhcpRange(iface) != 0) {
        NETNATIVE_LOGI("failed to remove [%{public}s] dhcp range.", iface.c_str());
    }
    if (OHOS::Wifi::DhcpServiceApi::GetInstance()->StopDhcpServer(iface) != 0) {
        NETNATIVE_LOGI("Stop dhcp server failed!");
        return false;
    }
    return true;
}
} // namespace nmd
} // namespace OHOS
