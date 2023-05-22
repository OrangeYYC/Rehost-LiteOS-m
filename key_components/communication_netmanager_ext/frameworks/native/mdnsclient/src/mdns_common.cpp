/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "mdns_common.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr const char *MDNS_TYPE_TCP = "_tcp";
constexpr const char *MDNS_TYPE_UDP = "_udp";
constexpr const char *MDNS_TYPE_PREFIX = "_";
constexpr size_t MDNS_TYPE_SEGMENT = 2;
constexpr size_t MDNS_INSTANCE_SEGMENT = 3;
constexpr size_t MDNS_NAME_IDX = 0;
constexpr size_t MDNS_TYPE1_IDX = 1;
constexpr size_t MDNS_TYPE2_IDX = 2;

} // namespace

bool EndsWith(const std::string_view &str, const std::string_view &pat)
{
    return std::mismatch(pat.rbegin(), pat.rend(), str.rbegin()).first == pat.rend();
}

bool StartsWith(const std::string_view &str, const std::string_view &pat)
{
    return std::mismatch(pat.begin(), pat.end(), str.begin()).first == pat.end();
}

std::vector<std::string_view> Split(const std::string_view &s, char seperator)
{
    std::vector<std::string_view> output;
    std::string::size_type prev = 0;
    std::string::size_type pos = 0;
    while ((pos = s.find(seperator, pos)) != std::string::npos) {
        if (pos - prev > 0) {
            output.push_back(s.substr(prev, pos - prev));
        }
        prev = ++pos;
    }
    if (prev < s.size()) {
        output.push_back(s.substr(prev));
    }
    return output;
}

bool IsNameValid(const std::string &name)
{
    return 0 < name.size() && name.size() <= MDNS_MAX_DOMAIN_LABEL &&
           name.find(MDNS_DOMAIN_SPLITER) == std::string::npos;
}

bool IsTypeValid(const std::string &type)
{
    auto views = Split(type, MDNS_DOMAIN_SPLITER);
    return views.size() == MDNS_TYPE_SEGMENT && views[0].size() <= MDNS_MAX_DOMAIN_LABEL &&
           StartsWith(views[0], MDNS_TYPE_PREFIX) && (views[1] == MDNS_TYPE_UDP || views[1] == MDNS_TYPE_TCP);
}

bool IsPortValid(int port)
{
    return 0 <= port && port <= UINT16_MAX;
}

bool IsInstanceValid(const std::string &instance)
{
    auto views = Split(instance, MDNS_DOMAIN_SPLITER);
    return views.size() == MDNS_INSTANCE_SEGMENT && views[MDNS_NAME_IDX].size() <= MDNS_MAX_DOMAIN_LABEL &&
           views[MDNS_TYPE1_IDX].size() <= MDNS_MAX_DOMAIN_LABEL &&
           StartsWith(views[MDNS_TYPE1_IDX], MDNS_TYPE_PREFIX) &&
           (views[MDNS_TYPE2_IDX] == MDNS_TYPE_UDP || views[MDNS_TYPE2_IDX] == MDNS_TYPE_TCP);
}

bool IsDomainValid(const std::string &domain)
{
    return domain.size() + static_cast<int>(!EndsWith(domain, MDNS_DOMAIN_SPLITER_STR)) <= MDNS_MAX_DOMAIN;
}

void ExtractNameAndType(const std::string &instance, std::string &name, std::string &type)
{
    auto views = Split(instance, MDNS_DOMAIN_SPLITER);
    if (views.size() == MDNS_INSTANCE_SEGMENT || views.size() == MDNS_INSTANCE_SEGMENT + 1) {
        name = std::string(views[MDNS_NAME_IDX].begin(), views[MDNS_NAME_IDX].end());
        type = std::string(views[MDNS_TYPE1_IDX].begin(), views[MDNS_TYPE2_IDX].end());
    }
}

} // namespace NetManagerStandard
} // namespace OHOS
