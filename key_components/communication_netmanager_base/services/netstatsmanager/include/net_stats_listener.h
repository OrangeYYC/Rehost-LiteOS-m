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

#ifndef NET_STATS_LISTENER_H
#define NET_STATS_LISTENER_H

#include "common_event_subscriber.h"
#include "common_event_support.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsListener : public EventFwk::CommonEventSubscriber {
public:
    using StatsCallback = std::function<int32_t(const EventFwk::Want &want)>;
    explicit NetStatsListener(const EventFwk::CommonEventSubscribeInfo &sp);

    void RegisterStatsCallback(const std::string &event, StatsCallback callback);

    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    std::map<std::string, StatsCallback> callbackMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_LISTENER_H