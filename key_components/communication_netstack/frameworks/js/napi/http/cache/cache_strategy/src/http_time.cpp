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

#include "http_time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

static constexpr const char *GMT_TIME = "%a, %d %b %Y %H:%M:%S GMT";

static constexpr const int MAX_TIME_LEN = 128;

namespace OHOS::NetStack {
time_t HttpTime::StrTimeToTimestamp(const std::string &time_str)
{
    std::tm tm = {0};
    std::stringstream ss(time_str);
    ss >> std::get_time(&tm, GMT_TIME);
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    auto tmp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    return tmp.count();
}

time_t HttpTime::GetNowTimeSeconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

std::string HttpTime::GetNowTimeGMT()
{
    auto now = std::chrono::system_clock::now();
    time_t timeSeconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::tm timeInfo = {0};
#ifdef WINDOWS_PLATFORM
    if (gmtime_s(&timeInfo, &timeSeconds) == 0) {
#else
    if (gmtime_r(&timeSeconds, &timeInfo) == nullptr) {
#endif
        return {};
    }
    char s[MAX_TIME_LEN] = {0};
    if (strftime(s, sizeof(s), GMT_TIME, &timeInfo) == 0) {
        return {};
    }
    return s;
}
} // namespace OHOS::NetStack
