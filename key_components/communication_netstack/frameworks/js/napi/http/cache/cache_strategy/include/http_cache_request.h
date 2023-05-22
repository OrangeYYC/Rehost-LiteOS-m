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

#ifndef HTTP_CACHE_REQUEST_H
#define HTTP_CACHE_REQUEST_H

#include <cstring>
#include <iostream>
#include <map>

#include "http_time.h"

namespace OHOS::NetStack {
class HttpCacheRequest {
public:
    HttpCacheRequest();

    ~HttpCacheRequest();

    void ParseRequestHeader(const std::map<std::string, std::string> &requestHeader);

    void SetRequestTime(const std::string &requestTime);

    [[nodiscard]] time_t GetIfModifiedSince() const;

    [[nodiscard]] time_t GetRequestTime() const;

    [[nodiscard]] time_t GetMaxAgeSeconds() const;

    [[nodiscard]] time_t GetMaxStaleSeconds() const;

    [[nodiscard]] time_t GetMinFreshSeconds() const;

    [[nodiscard]] bool IsNoCache() const;

    [[nodiscard]] bool IsNoStore() const;

    [[nodiscard]] bool IsNoTransform() const;

    [[nodiscard]] bool IsOnlyIfCached() const;

    [[nodiscard]] std::string GetIfNoneMatch() const;

private:
    void ParseCacheControl(const std::string &cacheControl);

private:
    std::string requestTime_;
    std::string ifModifiedSince_;
    std::string ifNoneMatch_;

    std::string maxAge_;
    std::string maxStale_;
    std::string minFresh_;

    bool noCache_ = false;
    bool noStore_ = false;
    bool noTransform_ = false;
    bool onlyIfCached_ = false;
};
} // namespace OHOS::NetStack
#endif // HTTP_CACHE_REQUEST_H
