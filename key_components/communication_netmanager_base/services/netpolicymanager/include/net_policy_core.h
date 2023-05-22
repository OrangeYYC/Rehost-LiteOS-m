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

#ifndef NET_POLICY_CORE_H
#define NET_POLICY_CORE_H

#include <string>
#include <vector>

#include "event_handler.h"

#include "net_manager_center.h"
#include "net_policy_callback.h"
#include "net_policy_file.h"
#include "net_policy_inner_define.h"
#include "netsys_policy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyEventHandler;
class NetPolicyBase;

struct PolicyEvent {
    int32_t eventId = 0;
    std::shared_ptr<NetPolicyBase> sender = nullptr;
    bool deviceIdleMode = false;
    std::vector<uint32_t> deviceIdleList;
    bool powerSaveMode = false;
    uint32_t deletedUid = 0;
};

class NetPolicyCore : public EventFwk::CommonEventSubscriber, public std::enable_shared_from_this<NetPolicyCore> {
    DECLARE_DELAYED_SINGLETON(NetPolicyCore);

public:
    template <typename NetPolicyBase> std::shared_ptr<NetPolicyBase> CreateCore()
    {
        std::shared_ptr<NetPolicyBase> core = std::make_shared<NetPolicyBase>();
        core->Init();
        cores_.push_back(core);
        return core;
    }
    void Init(std::shared_ptr<NetPolicyEventHandler> &handler);

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param policyEvent The infomations passed from other core
     */
    void HandleEvent(const AppExecFwk::InnerEvent::Pointer &event);

    /**
     * Send events to other policy cores.
     *
     * @param eventId The event id
     * @param eventData The event data
     * @param delayTime The delay time, if need the message send delay
     */
    void SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &eventData, int64_t delayTime = 0);

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);

private:
    void SubscribeCommonEvent();
    std::vector<std::shared_ptr<NetPolicyBase>> cores_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<NetPolicyEventHandler> handler_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CORE_H