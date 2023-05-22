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

#include "parameter.h"

#include "base64_utils.h"
#include "net_http_proxy_tracker.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t SYSPARA_MAX_SIZE = 96;
constexpr const char *EXCLUSIONS_SPLIT_SYMBOL = ",";
constexpr const char *DEFAULT_HTTP_PROXY_HOST = "NONE";
constexpr const char *DEFAULT_HTTP_PROXY_PORT = "0";
constexpr const char *DEFAULT_HTTP_PROXY_EXCLUSION_LIST = "NONE";
constexpr const char *HTTP_PROXY_HOST_KEY = "persist.netmanager_base.http_proxy.host";
constexpr const char *HTTP_PROXY_PORT_KEY = "persist.netmanager_base.http_proxy.port";
constexpr const char *HTTP_PROXY_EXCLUSIONS_KEY = "persist.netmanager_base.http_proxy.exclusion_list";
} // namespace

bool NetHttpProxyTracker::ReadFromSystemParameter(HttpProxy &httpProxy)
{
    char httpProxyHost[SYSPARA_MAX_SIZE] = {0};
    char httpProxyPort[SYSPARA_MAX_SIZE] = {0};
    char httpProxyExclusions[SYSPARA_MAX_SIZE] = {0};
    GetParameter(HTTP_PROXY_HOST_KEY, DEFAULT_HTTP_PROXY_HOST, httpProxyHost, sizeof(httpProxyHost));
    GetParameter(HTTP_PROXY_PORT_KEY, DEFAULT_HTTP_PROXY_PORT, httpProxyPort, sizeof(httpProxyPort));
    GetParameter(HTTP_PROXY_EXCLUSIONS_KEY, DEFAULT_HTTP_PROXY_EXCLUSION_LIST, httpProxyExclusions,
                 sizeof(httpProxyExclusions));
    std::string host = Base64::Decode(httpProxyHost);
    host = (host == DEFAULT_HTTP_PROXY_HOST ? "" : host);
    std::set<std::string> exclusionList = ParseExclusionList(httpProxyExclusions);
    uint16_t port = static_cast<uint16_t>(std::atoi(httpProxyPort));
    httpProxy = {host, port, exclusionList};
    return true;
}

bool NetHttpProxyTracker::WriteToSystemParameter(const HttpProxy &httpProxy)
{
    std::string host = Base64::Encode(httpProxy.GetHost());
    if (host.empty()) {
        host = Base64::Encode(DEFAULT_HTTP_PROXY_HOST);
    }
    int32_t ret = SetParameter(HTTP_PROXY_HOST_KEY, host.c_str());
    if (ret) {
        NETMGR_LOG_E("Set host:%{public}s to system parameter:%{public}s failed, ret:%{public}d", host.c_str(),
                     HTTP_PROXY_HOST_KEY, ret);
        return false;
    }

    std::string port = std::to_string(httpProxy.GetPort());
    ret = SetParameter(HTTP_PROXY_PORT_KEY, port.c_str());
    if (ret) {
        NETMGR_LOG_E("Set port:%{public}s to system parameter:%{public}s failed, ret:%{public}d", port.c_str(),
                     HTTP_PROXY_PORT_KEY, ret);
        return false;
    }

    std::string exclusions = GetExclusionsAsAstring(httpProxy.GetExclusionList());
    if (exclusions.empty()) {
        exclusions = DEFAULT_HTTP_PROXY_EXCLUSION_LIST;
    }
    ret = SetParameter(HTTP_PROXY_EXCLUSIONS_KEY, exclusions.c_str());
    if (ret) {
        NETMGR_LOG_E("Set exclusions:%{public}s to system parameter:%{public}s failed, ret:%{public}d",
                     exclusions.c_str(), HTTP_PROXY_EXCLUSIONS_KEY, ret);
        return false;
    }
    return true;
}

std::set<std::string> NetHttpProxyTracker::ParseExclusionList(const std::string &exclusions) const
{
    std::set<std::string> exclusionList;
    if (exclusions.empty() || exclusions == DEFAULT_HTTP_PROXY_EXCLUSION_LIST) {
        return exclusionList;
    }
    size_t startPos = 0;
    size_t searchePos = exclusions.find(EXCLUSIONS_SPLIT_SYMBOL);
    std::string exclusion;
    while (searchePos != std::string::npos) {
        exclusion = exclusions.substr(startPos, (searchePos - startPos));
        exclusionList.insert(exclusion);
        startPos = searchePos + 1;
        searchePos = exclusions.find(EXCLUSIONS_SPLIT_SYMBOL, startPos);
    }
    exclusion = exclusions.substr(startPos, (exclusions.size() - startPos));
    exclusionList.insert(exclusion);
    return exclusionList;
}

std::string NetHttpProxyTracker::GetExclusionsAsAstring(const std::set<std::string> &exculisonList) const
{
    std::string exclusions;
    int32_t index = 0;
    for (auto exclusion : exculisonList) {
        if (exclusions.size() + exclusion.size() >= SYSPARA_MAX_SIZE) {
            break;
        }
        if (index > 0) {
            exclusions = exclusions + EXCLUSIONS_SPLIT_SYMBOL;
        }
        exclusions = exclusions + exclusion;
        index++;
    }
    return exclusions;
}
} // namespace NetManagerStandard
} // namespace OHOS