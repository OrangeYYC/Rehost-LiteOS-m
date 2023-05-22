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

#ifndef I_SHARING_EVENT_CALLBACK_H
#define I_SHARING_EVENT_CALLBACK_H

#include <string>
#include <iremote_broker.h>

#include "net_handle.h"
#include "net_manager_ext_constants.h"

namespace OHOS {
namespace NetManagerStandard {
class ISharingEventCallback : public IRemoteBroker {
public:
    virtual void OnSharingStateChanged(const bool &isRunning) = 0;
    virtual void OnInterfaceSharingStateChanged(const SharingIfaceType &type, const std::string &iface,
                                                const SharingIfaceState &state) = 0;
    virtual void OnSharingUpstreamChanged(const sptr<NetHandle> netHandle) = 0;

    enum class Message {
        GLOBAL_SHARING_STATE_CHANGED,
        INTERFACE_SHARING_STATE_CHANGED,
        SHARING_UPSTREAM_CHANGED,
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetworkShareService.ISharingEventCallback");
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_SHARING_EVENT_CALLBACK_H
