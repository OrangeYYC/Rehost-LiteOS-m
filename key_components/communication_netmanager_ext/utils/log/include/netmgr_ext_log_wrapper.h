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

#ifndef NETMGR_EXT_LOG_WRAPPER_H
#define NETMGR_EXT_LOG_WRAPPER_H

#include <string>
#include "hilog/log.h"

namespace OHOS {
namespace NetManagerStandard {
enum class NetMgrExtLogLevel {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

class NetMgrExtLogWrapper {
public:
    static bool JudgeLevel(const NetMgrExtLogLevel &level);

    static void SetLogLevel(const NetMgrExtLogLevel &level)
    {
        level_ = level;
    }

    static const NetMgrExtLogLevel &GetLogLevel()
    {
        return level_;
    }

    static std::string GetBriefFileName(const std::string &file);

private:
    static NetMgrExtLogLevel level_;
};

#ifndef NETMGR_EXT_LOG_TAG
#define NETMGR_EXT_LOG_TAG "NetMgrExtPart"
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel NET_MGR_LABEL = {LOG_CORE, LOG_DOMAIN, NETMGR_EXT_LOG_TAG};

#define NETMGR_EXT_DEBUG 1

#define MAKE_FILE_NAME (strrchr(__FILE__, '/') + 1)

#ifdef NETMGR_EXT_DEBUG
#define PRINT_LOG(op, fmt, ...)                                                                               \
    (void)OHOS::HiviewDFX::HiLog::op(NET_MGR_LABEL, "[%{public}s-(%{public}s:%{public}d)]" fmt, __FUNCTION__, \
        MAKE_FILE_NAME, __LINE__, ##__VA_ARGS__)
#else
#define PRINT_LOG(op, fmt, ...)
#endif

#define NETMGR_EXT_LOG_D(fmt, ...) PRINT_LOG(Debug, fmt, ##__VA_ARGS__)
#define NETMGR_EXT_LOG_E(fmt, ...) PRINT_LOG(Error, fmt, ##__VA_ARGS__)
#define NETMGR_EXT_LOG_W(fmt, ...) PRINT_LOG(Warn, fmt, ##__VA_ARGS__)
#define NETMGR_EXT_LOG_I(fmt, ...) PRINT_LOG(Info, fmt, ##__VA_ARGS__)
#define NETMGR_EXT_LOG_F(fmt, ...) PRINT_LOG(Fatal, fmt, ##__VA_ARGS__)
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMGR_EXT_LOG_WRAPPER_H
