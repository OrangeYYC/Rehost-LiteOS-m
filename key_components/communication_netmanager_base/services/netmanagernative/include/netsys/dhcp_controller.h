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

#ifndef INCLUDE_DHCP_CONTROLLER_H
#define INCLUDE_DHCP_CONTROLLER_H

#include "dhcp_service_api.h"
#include "i_dhcp_result_notify.h"
#include "i_notify_callback.h"

namespace OHOS {
namespace nmd {
class DhcpController {
public:
    class DhcpControllerResultNotify : public OHOS::Wifi::IDhcpResultNotify {
    public:
        explicit DhcpControllerResultNotify(DhcpController &dhcpController);
        virtual ~DhcpControllerResultNotify() override;
        void OnSuccess(int status, const std::string &ifname, OHOS::Wifi::DhcpResult &result) override;
        void OnFailed(int status, const std::string &ifname, const std::string &reason) override;
        void OnSerExitNotify(const std::string &ifname) override;

    private:
        DhcpController &dhcpController_;
    };

public:
    DhcpController();
    ~DhcpController();

    int32_t RegisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback);
    void StartDhcpClient(const std::string &iface, bool bIpv6);
    void StopDhcpClient(const std::string &iface, bool bIpv6);
    bool StartDhcpService(const std::string &iface, const std::string &ipv4addr);
    bool StopDhcpService(const std::string &iface);

    void Process(const std::string &iface, OHOS::Wifi::DhcpResult &result);

private:
    std::unique_ptr<DhcpControllerResultNotify> dhcpResultNotify_ = nullptr;
    sptr<OHOS::NetsysNative::INotifyCallback> callback_ = nullptr;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_DHCP_CONTROLLER_H
