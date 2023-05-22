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

#ifndef NETMANAGER_BASE_TSTATSGETUPDATEIFACEUIDSTATS_CONTEXT_H
#define NETMANAGER_BASE_TSTATSGETUPDATEIFACEUIDSTATS_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"
#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
class UpdateIfacesStatsContext final : public BaseContext {
public:
    UpdateIfacesStatsContext() = delete;
    explicit UpdateIfacesStatsContext(napi_env env, EventManager *manager);

    void SetUid(int32_t uid);
    void SetInterfaceName(std::string interfaceName);
    void SetStatsInfo(NetStatsInfo statsInfo);
    void SetStart(uint32_t start);
    void SetEnd(uint32_t end);
    int32_t GetUid() const;
    std::string GetInterfaceName() const;
    NetStatsInfo GetStatsInfo() const;
    uint32_t GetStart() const;
    uint32_t GetEnd() const;

    void ParseParams(napi_value *params, size_t paramsCount);

private:
    int32_t uid_ = 0;
    std::string interfaceName_;
    NetStatsInfo statsInfo_;
    uint32_t start_ = 0;
    uint32_t end_ = 0;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_TSTATSGETUPDATEIFACEUIDSTATS_CONTEXT_H
