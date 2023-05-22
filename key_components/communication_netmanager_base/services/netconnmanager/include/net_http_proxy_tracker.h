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

#ifndef NET_HTTP_PROXY_TRACKER_H
#define NET_HTTP_PROXY_TRACKER_H

#include "http_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetHttpProxyTracker {
public:
    NetHttpProxyTracker() = default;
    ~NetHttpProxyTracker() = default;

    bool ReadFromSystemParameter(HttpProxy &httpProxy);
    bool WriteToSystemParameter(const HttpProxy &httpProxy);

private:
    std::set<std::string> ParseExclusionList(const std::string &exclusions) const;
    std::string GetExclusionsAsAstring(const std::set<std::string> &exculisonList) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_HTTP_PROXY_TRACKER_H