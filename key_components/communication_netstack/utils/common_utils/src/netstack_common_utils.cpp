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

#include "netstack_common_utils.h"

#include <algorithm>
#include <cerrno>
#include <string>
#include <unistd.h>
#include <vector>

#include "curl/curl.h"
#include "netstack_log.h"

namespace OHOS::NetStack::CommonUtils {
std::vector<std::string> Split(const std::string &str, const std::string &sep)
{
    std::string s = str;
    std::vector<std::string> res;
    while (!s.empty()) {
        auto pos = s.find(sep);
        if (pos == std::string::npos) {
            res.emplace_back(s);
            break;
        }
        res.emplace_back(s.substr(0, pos));
        s = s.substr(pos + sep.size());
    }
    return res;
}

std::vector<std::string> Split(const std::string &str, const std::string &sep, size_t size)
{
    std::string s = str;
    std::vector<std::string> res;
    while (!s.empty()) {
        if (res.size() + 1 == size) {
            res.emplace_back(s);
            break;
        }

        auto pos = s.find(sep);
        if (pos == std::string::npos) {
            res.emplace_back(s);
            break;
        }
        res.emplace_back(s.substr(0, pos));
        s = s.substr(pos + sep.size());
    }
    return res;
}

std::string Strip(const std::string &str, char ch)
{
    int64_t i = 0;
    while (static_cast<size_t>(i) < str.size() && str[i] == ch) {
        ++i;
    }
    int64_t j = static_cast<int64_t>(str.size()) - 1;
    while (j > 0 && str[j] == ch) {
        --j;
    }
    if (i >= 0 && static_cast<size_t>(i) < str.size() && j >= 0 && static_cast<size_t>(j) < str.size() &&
        j - i + 1 > 0) {
        return str.substr(i, j - i + 1);
    }
    return "";
}

std::string ToLower(const std::string &s)
{
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), tolower);
    return res;
}

bool HasInternetPermission()
{
#ifndef OH_CORE_NETSTACK_PERMISSION_CHECK
    int testSock = socket(AF_INET, SOCK_STREAM, 0);
    if (testSock < 0 && errno == EPERM) {
        NETSTACK_LOGE("make tcp testSock failed errno is %{public}d %{public}s", errno, strerror(errno));
        return false;
    }
    if (testSock > 0) {
        close(testSock);
    }
    return true;
#else
    constexpr int inetGroup = 40002003; // 3003 in gateway shell.
    int groupNum = getgroups(0, nullptr);
    if (groupNum <= 0) {
        NETSTACK_LOGE("no group of INTERNET permission");
        return false;
    }
    auto groups = (gid_t*) malloc(groupNum * sizeof(gid_t));
    groupNum = getgroups(groupNum, groups);
    for (int i = 0; i < groupNum; i++) {
        if (groups[i] == inetGroup) {
            free(groups);
            return true;
        }
    }
    free(groups);
    NETSTACK_LOGE("INTERNET permission denied by group");
    return false;
#endif
}
} // namespace OHOS::NetStack::CommonUtils