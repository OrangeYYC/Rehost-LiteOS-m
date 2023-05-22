/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NET_ADJ_SERVICE_H
#define NET_ADJ_SERVICE_H

#include "singleton.h"
#include "system_ability.h"
#include "net_adj_service_stub.h"
#include "net_adj_iface_info.h"

namespace OHOS {
namespace NetManagerStandard {
/**
 * Network adjacency service is used to establish the adjacency of devices and
 * mask physical layer differences between different devices through IP routing.
 */
class NetAdjService : public SystemAbility, public NetAdjServiceStub, std::enable_shared_from_this<NetAdjService> {
    DECLARE_DELAYED_SINGLETON(NetAdjService);
    DECLARE_SYSTEM_ABILITY(NetAdjService);

public:
     /**
      * Add a network adjacency interface.
      *
      * @param adjIface an instance of adjacency interface.
      * @return ERR_NONE for operation success or errcode for failed.
      */
    int32_t AddNetAdjIface(const sptr<NetAdjIfaceInfo> &adjIface) override;

     /**
      * Remove a network adjacency interface.
      *
      * @param ifaceName is the interface name
      * @return ERR_NONE for operation success or errcode for failed.
      */
    int32_t RemoveNetAdjIface(const std::string &ifaceName) override;

    /**
     * Register a listener for listening on adjacency network interfaces.
     *
     * @param callback calls when network adjacency interfaces added or removed.
     * @return ERR_NONE for operation success or errcode for failed.
     */
    int32_t RegisterAdjIfaceCallback(const sptr<INetAdjCallback> &callback) override;

    /**
     * Unregister the listener for listening on adjacency network interfaces.
     *
     * @param callback the callback object from function {@code NetAdjService::RegisterAdjIfaceCallback}
     * @return ERR_NONE for operation success or errcode for failed.
     */
    int32_t UnregisterAdjIfaceCallback(const sptr<INetAdjCallback> &callback) override;

    int32_t UpdateNetAdjInfo(const std::string &iface, const sptr<NetAdjIfaceInfo> &adjIfaceINfo) override;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_ADJ_SERVICE_H
