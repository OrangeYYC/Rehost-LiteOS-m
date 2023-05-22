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
#ifndef NETMANAGER_I_NET_ADJ_SERVICE_H
#define NETMANAGER_I_NET_ADJ_SERVICE_H

#include <list>

#include "iremote_broker.h"
#include "i_net_adj_callback.h"
#include "net_adj_iface_info.h"

namespace OHOS {
namespace NetManagerStandard {
class INetAdjService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetAdjService");
    enum {
        CMD_NM_ADJ_SYSTEM_READY,
        CMD_NM_ADJ_ADD_IFACE,
        CMD_NM_ADJ_REMOVE_ADJ_IFACE,
        CMD_NM_ADJ_REGISTER_ADJ_CALLBACK,
        CMD_NM_ADJ_UNREGISTER_ADJ_CALLBACK,
        CMD_NM_ADJ_UPDATE_ADJ_INFO,
        CMD_NM_ADJ_END,
    };

public:
    virtual int32_t SystemReady() = 0;
    virtual int32_t AddNetAdjIface(const sptr<NetAdjIfaceInfo> &adjIface) = 0;
    virtual int32_t RemoveNetAdjIface(const std::string &ifaceName) = 0;
    virtual int32_t RegisterAdjIfaceCallback(const sptr<INetAdjCallback> &callback) = 0;
    virtual int32_t UnregisterAdjIfaceCallback(const sptr<INetAdjCallback> &callback) = 0;
    virtual int32_t UpdateNetAdjInfo(const std::string &iface, const sptr<NetAdjIfaceInfo> &adjIfaceINfo) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_I_NET_ADJ_SERVICE_H
