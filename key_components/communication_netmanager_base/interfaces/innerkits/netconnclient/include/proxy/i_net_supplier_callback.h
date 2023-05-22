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

#ifndef I_NET_SUPPLIER_CALLBACK_H
#define I_NET_SUPPLIER_CALLBACK_H

#include <string>
#include <set>

#include "iremote_broker.h"

#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
class INetSupplierCallback : public IRemoteBroker {
public:
    virtual ~INetSupplierCallback() = default;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetSupplierCallback");
    enum {
        NET_SUPPLIER_REQUEST_NETWORK = 0,
        NET_SUPPLIER_RELEASE_NETWORK = 1,
    };

public:
    virtual int32_t RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps) = 0;
    virtual int32_t ReleaseNetwork(const std::string &ident, const std::set<NetCap> &netCaps) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_SUPPLIER_CALLBACK_H