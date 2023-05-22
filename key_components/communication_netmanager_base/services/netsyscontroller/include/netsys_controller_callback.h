/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NETSYS_NETSYSCONTROLLER_CALLBACK_H
#define NETSYS_NETSYSCONTROLLER_CALLBACK_H

#include "refbase.h"

namespace OHOS {
namespace NetManagerStandard {
class NetsysControllerCallback : public RefBase {
public:
    struct DhcpResult {
        std::string iface_;
        std::string ipAddr_;
        std::string gateWay_;
        std::string subNet_;
        std::string route1_;
        std::string route2_;
        std::string dns1_;
        std::string dns2_;
    };
public:
    virtual ~NetsysControllerCallback() {};
    virtual int32_t OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int) = 0;
    virtual int32_t OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int) = 0;
    virtual int32_t OnInterfaceAdded(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceRemoved(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceChanged(const std::string &, bool) = 0;
    virtual int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) = 0;
    virtual int32_t OnRouteChanged(bool, const std::string &, const std::string &, const std::string &) = 0;
    virtual int32_t OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult) = 0;
    virtual int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETSYS_NETSYSCONTROLLER_CALLBACK_H
