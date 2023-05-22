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
#ifndef NET_STATS_BASE_SERVICE_H
#define NET_STATS_BASE_SERVICE_H

#include "refbase.h"

#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsBaseService : public virtual RefBase {
public:
    virtual int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                        NetStatsInfo &info) = 0;
    virtual int32_t ResetStatsFactory() = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_BASE_SERVICE_H
