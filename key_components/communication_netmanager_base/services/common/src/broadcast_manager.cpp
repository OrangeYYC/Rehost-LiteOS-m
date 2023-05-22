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
#include "broadcast_manager.h"

#include "common_event_manager.h"
#include "common_event_support.h"
#include "want.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
BroadcastManager::BroadcastManager() {}

BroadcastManager::~BroadcastManager() {}

bool BroadcastManager::SendBroadcast(const BroadcastInfo &info, const std::map<std::string, bool> &param)
{
    return SendBroadcastEx(info, param);
}

bool BroadcastManager::SendBroadcast(const BroadcastInfo &info, const std::map<std::string, int32_t> &param)
{
    return SendBroadcastEx(info, param);
}

bool BroadcastManager::SendBroadcast(const BroadcastInfo &info, const std::map<std::string, int64_t> &param)
{
    return SendBroadcastEx(info, param);
}

bool BroadcastManager::SendBroadcast(const BroadcastInfo &info, const std::map<std::string, std::string> &param)
{
    return SendBroadcastEx(info, param);
}

template <typename T>
bool BroadcastManager::SendBroadcastEx(const BroadcastInfo &info, const std::map<std::string, T> &param)
{
    if (info.action.empty()) {
        NETMGR_LOG_E("The parameter of action is empty");
        return false;
    }

    AAFwk::Want want;
    want.SetAction(info.action);
    for (const auto &x : param) {
        want.SetParam(x.first, x.second);
    }

    EventFwk::CommonEventData eventData;
    eventData.SetWant(want);
    eventData.SetCode(info.code);
    eventData.SetData(info.data);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(info.ordered);
    if (!info.permission.empty()) {
        publishInfo.SetSubscriberPermissions({info.permission});
    }

    bool publishResult = EventFwk::CommonEventManager::PublishCommonEvent(eventData, publishInfo, nullptr);
    if (publishResult) {
        NETMGR_LOG_D("Send broadcast is successfull");
    } else {
        NETMGR_LOG_D("Send broadcast is failed");
    }

    return publishResult;
}
} // namespace NetManagerStandard
} // namespace OHOS
