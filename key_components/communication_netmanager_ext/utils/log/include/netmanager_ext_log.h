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

#ifndef COMMUNICATIONNETMANAGER_EXT_NETMANAGER_EXT_LOG
#define COMMUNICATIONNETMANAGER_EXT_NETMANAGER_EXT_LOG

#include <cstring>
#include <string>

#define MAKE_FILE_NAME (strrchr(__FILE__, '/') + 1)

#if !defined(_WIN32) && !defined(__APPLE__)

#include "hilog/log.h"

#define NETMANAGER_EXT_LOG_TAG "NetMgrSubsystem"

#define NETMANAGER_EXT_LOG_DOMAIN 0xD0015B0

static constexpr OHOS::HiviewDFX::HiLogLabel NETMANAGER_EXT_LOG_LABEL = {LOG_CORE, \
    NETMANAGER_EXT_LOG_DOMAIN, NETMANAGER_EXT_LOG_TAG};

#define NETMANAGER_EXT_HILOG_PRINT(Level, fmt, ...)                                             \
    (void)OHOS::HiviewDFX::HiLog::Level(NETMANAGER_EXT_LOG_LABEL,                               \
        "NETMANAGER_EXT [%{public}s %{public}d] " fmt, MAKE_FILE_NAME, __LINE__, ##__VA_ARGS__)

#else

#include <stdarg.h>
#include <stdio.h>

#include "securec.h"

static constexpr uint32_t NETMANAGER_EXT_MAX_BUFFER_SIZE = 4096;

static void NetManagerStandardStripFormatString(const std::string &prefix, std::string &strFmt)
{
    for (auto pos = strFmt.find(prefix, 0); pos != std::string::npos; pos = strFmt.find(prefix, pos)) {
        strFmt.erase(pos, prefix.size());
    }
}

static void NetManagerStandardPrintLog(const char *fmt, ...)
{
    std::string newFmt(fmt);
    NetManagerStandardStripFormatString("{public}", newFmt);
    NetManagerStandardStripFormatString("{private}", newFmt);

    va_list args;
    va_start(args, fmt);

    char buf[NETMANAGER_EXT_MAX_BUFFER_SIZE] = {0};
    int ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, newFmt.c_str(), args);
    if (ret < 0) {
        va_end(args);
        return;
    }
    va_end(args);

    printf("%s\r\n", buf);
    fflush(stdout);
}

#define NETMANAGER_EXT_HILOG_PRINT(Level, fmt, ...) \
    NetManagerStandardPrintLog("NETMANAGER_EXT %s [%s %d] " fmt, #Level, MAKE_FILE_NAME, __LINE__, ##__VA_ARGS__)

#endif /* !defined(_WIN32) && !defined(__APPLE__) */

#define NETMANAGER_EXT_LOGE(fmt, ...) NETMANAGER_EXT_HILOG_PRINT(Error, fmt, ##__VA_ARGS__)

#define NETMANAGER_EXT_LOGI(fmt, ...) NETMANAGER_EXT_HILOG_PRINT(Info, fmt, ##__VA_ARGS__)

#endif /* COMMUNICATIONNETMANAGER_EXT_NETMANAGER_EXT_LOG */