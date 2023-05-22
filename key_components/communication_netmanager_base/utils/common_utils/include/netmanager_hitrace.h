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

#ifndef NETMANAGER_HITRACE_H
#define NETMANAGER_HITRACE_H

#include <string>
#include <vector>

#include "hitrace_meter.h"

namespace OHOS {
namespace NetManagerStandard {
class NetmanagerHiTrace {
public:
    static void NetmanagerStartSyncTrace(const std::string &value);
    static void NetmanagerFinishSyncTrace(const std::string &value);
    static void NetmanagerStartAsyncTrace(const std::string &value, int32_t taskId);
    static void NetmanagerFinishAsyncTrace(const std::string &value, int32_t taskId);

private:
    NetmanagerHiTrace() = default;
    ~NetmanagerHiTrace() = default;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_HITRACE_H
