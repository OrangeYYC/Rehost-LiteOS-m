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
#ifndef NET_CONN_SERVICE_IFACE_H
#define NET_CONN_SERVICE_IFACE_H

#include <set>

#include "net_all_capabilities.h"
#include "net_conn_base_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetConnServiceIface : public NetConnBaseService {
public:
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) override;
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) override;
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
        uint32_t &supplierId) override;
    int32_t UnregisterNetSupplier(uint32_t supplierId) override;
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) override;
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) override;
    int32_t RestrictBackgroundChanged(bool isRestrictBackground) override;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_SERVICE_IFACE_H