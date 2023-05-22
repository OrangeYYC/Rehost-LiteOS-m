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

#ifndef NET_STATS_CALLBACK_H
#define NET_STATS_CALLBACK_H

#include <string>
#include <vector>

#include "i_net_stats_callback.h"
#include "netsys_controller_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCallback : public virtual RefBase {
public:
    NetStatsCallback() = default;
    ~NetStatsCallback() = default;
    void RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback);
    void UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback);
    int32_t NotifyNetIfaceStatsChanged(const std::string &iface);
    int32_t NotifyNetUidStatsChanged(const std::string &iface, uint32_t uid);

private:
    std::vector<sptr<INetStatsCallback>> netStatsCallback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CALLBACK_H