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

#ifndef NET_LINK_INFO_H
#define NET_LINK_INFO_H

#include <list>

#include "inet_addr.h"
#include "net_specifier.h"
#include "route.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetLinkInfo : public Parcelable {
    std::string ifaceName_;
    std::string domain_;
    std::list<INetAddr> netAddrList_;
    std::list<INetAddr> dnsList_;
    std::list<Route> routeList_;
    uint16_t mtu_ = 0;
    std::string tcpBufferSizes_;

    NetLinkInfo() = default;
    NetLinkInfo(const NetLinkInfo &cap);
    NetLinkInfo &operator=(const NetLinkInfo &cap);

    bool Marshalling(Parcel &parcel) const override;
    static sptr<NetLinkInfo> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<NetLinkInfo> &object);
    void Initialize();
    std::string ToString(const std::string &tab) const;
    std::string ToStringAddr(const std::string &tab) const;
    std::string ToStringDns(const std::string &tab) const;
    std::string ToStringRoute(const std::string &tab) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_LINK_INFO_H
