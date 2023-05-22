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

#ifndef SHARING_EVENT_CALLBACK_PROXY_H
#define SHARING_EVENT_CALLBACK_PROXY_H

#include <string>

#include "i_sharing_event_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class SharingEventCallbackProxy : public IRemoteProxy<ISharingEventCallback> {
public:
    explicit SharingEventCallbackProxy(const sptr<IRemoteObject> &object);
    virtual ~SharingEventCallbackProxy() = default;

    virtual void OnSharingStateChanged(const bool &isRunning) override;
    virtual void OnInterfaceSharingStateChanged(const SharingIfaceType &type, const std::string &iface,
                                                const SharingIfaceState &state) override;
    virtual void OnSharingUpstreamChanged(const sptr<NetHandle> netHandle) override;

private:
    static inline BrokerDelegator<SharingEventCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // SHARING_EVENT_CALLBACK_PROXY_H
