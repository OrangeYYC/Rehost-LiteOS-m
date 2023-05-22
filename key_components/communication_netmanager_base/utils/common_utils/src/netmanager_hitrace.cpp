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

#include "netmanager_hitrace.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
void NetmanagerHiTrace::NetmanagerStartSyncTrace(const std::string &value)
{
    StartTrace(HITRACE_TAG_NET, value);
    NETMGR_LOG_I("NetmanagerStartSyncTrace value:%{public}s", value.c_str());
}

void NetmanagerHiTrace::NetmanagerFinishSyncTrace(const std::string &value)
{
    FinishTrace(HITRACE_TAG_NET);
    NETMGR_LOG_I("NetmanagerFinishSyncTrace value:%{public}s", value.c_str());
}

void NetmanagerHiTrace::NetmanagerStartAsyncTrace(const std::string &value, int32_t taskId)
{
    StartAsyncTrace(HITRACE_TAG_NET, value, taskId);
    NETMGR_LOG_I("NetmanagerStartAsyncTrace value:%{public}s, taskId:%{public}d", value.c_str(), taskId);
}

void NetmanagerHiTrace::NetmanagerFinishAsyncTrace(const std::string &value, int32_t taskId)
{
    FinishAsyncTrace(HITRACE_TAG_NET, value, taskId);
    NETMGR_LOG_I("NetmanagerFinishAsyncTrace value:%{public}s, taskId:%{public}d", value.c_str(), taskId);
}
} // namespace NetManagerStandard
} // namespace OHOS
