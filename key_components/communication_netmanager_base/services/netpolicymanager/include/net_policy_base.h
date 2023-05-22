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

#ifndef NET_POLICY_BASE_H
#define NET_POLICY_BASE_H

#include <string>
#include <vector>

#include "event_handler.h"

#include "net_manager_center.h"
#include "net_policy_callback.h"
#include "net_policy_core.h"
#include "net_policy_event_handler.h"
#include "net_policy_file.h"
#include "netmanager_hitrace.h"
#include "netsys_policy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyBase : public std::enable_shared_from_this<NetPolicyBase> {
public:
    NetPolicyBase();
    virtual ~NetPolicyBase();
    virtual void Init() = 0;

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param policyEvent The infomations passed from other core
     */
    virtual void HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent) = 0;

    /**
     * Send events to other policy cores.
     *
     * @param eventId The event id
     * @param delayTime The delay time, if need the message send delay
     */
    void SendEvent(int32_t eventId, int64_t delayTime = 0);

    /**
     * Send events to other policy cores.
     *
     * @param eventId The event id
     * @param eventData The event data
     * @param delayTime The delay time, if need the message send delay
     */
    void SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &policyEvent, int64_t delayTime = 0);

protected:
    std::shared_ptr<NetPolicyCallback> GetCbInst();
    std::shared_ptr<NetPolicyFile> GetFileInst();
    std::shared_ptr<NetsysPolicyWrapper> GetNetsysInst();
    NetManagerCenter &GetNetCenterInst();
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_BASE_H