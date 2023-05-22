/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "net_stats_listener.h"

#include "bundle_constants.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_data_handler.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace OHOS::EventFwk;
NetStatsListener::StatsCallback onUidRemove = [](const Want &want) {
    uint32_t uid = want.GetIntParam(AppExecFwk::Constants::UID, 0);
    auto handler = std::make_unique<NetStatsDataHandler>();
    NETMGR_LOG_D("Net Manager delete uid, uid:[%{public}d]", uid);
    return handler->DeleteByUid(uid);
};
} // namespace

NetStatsListener::NetStatsListener(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp)
{
    callbackMap_[CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED] = onUidRemove;
}

void NetStatsListener::RegisterStatsCallback(const std::string &event, StatsCallback callback)
{
    if (callbackMap_.find(event) != callbackMap_.end()) {
        NETMGR_LOG_W("Key %{public}s has been assigned and will be replaced", event.c_str());
    }
    callbackMap_[event] = callback;
}

void NetStatsListener::OnReceiveEvent(const CommonEventData &data)
{
    NETMGR_LOG_I("NetStatsListener::OnReceiveEvent(), event:[%{public}s], data:[%{public}s], code:[%{public}d]",
                 data.GetWant().GetAction().c_str(), data.GetData().c_str(), data.GetCode());

    auto want = data.GetWant();
    auto callback = callbackMap_.find(want.GetAction());
    if (callback == callbackMap_.end()) {
        return;
    }
    if (callback->second == nullptr) {
        callbackMap_.erase(want.GetAction());
        return;
    }
    auto ret = callback->second(want);
    if (ret != 0) {
        NETMGR_LOG_E("Callback run failed");
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
