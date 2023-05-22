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
#ifndef I_NOTIFY_CALLBACK_H
#define I_NOTIFY_CALLBACK_H

#include <string>

#include "dhcp_result_parcel.h"
#include "iremote_broker.h"

namespace OHOS {
namespace NetsysNative {
class INotifyCallback : public IRemoteBroker {
public:
    virtual ~INotifyCallback() = default;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetsysNative.INotifyCallback");
    enum {
        ON_INTERFACE_ADDRESS_UPDATED = 0,
        ON_INTERFACE_ADDRESS_REMOVED,
        ON_INTERFACE_ADDED,
        ON_INTERFACE_REMOVED,
        ON_INTERFACE_CHANGED,
        ON_INTERFACE_LINK_STATE_CHANGED,
        ON_ROUTE_CHANGED,
        ON_DHCP_SUCCESS,
        ON_BANDWIDTH_REACHED_LIMIT,
    };

public:
    virtual int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int flags,
                                              int scope) = 0;
    virtual int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int flags,
                                              int scope) = 0;
    virtual int32_t OnInterfaceAdded(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceRemoved(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceChanged(const std::string &ifName, bool up) = 0;
    virtual int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) = 0;
    virtual int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                                   const std::string &ifName) = 0;
    virtual int32_t OnDhcpSuccess(sptr<DhcpResultParcel> &dhcpResult) = 0;
    virtual int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) = 0;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // I_NOTIFY_CALLBACK_H
