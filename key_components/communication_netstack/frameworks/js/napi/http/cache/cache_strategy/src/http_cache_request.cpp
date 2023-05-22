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

#include "http_cache_request.h"

#include "casche_constant.h"
#include "netstack_common_utils.h"

namespace OHOS::NetStack {
void HttpCacheRequest::ParseCacheControl(const std::string &cacheControl)
{
    auto vec = CommonUtils::Split(cacheControl, SPLIT);
    for (const auto &str : vec) {
        if (str == NO_CACHE) {
            noCache_ = true;
        } else if (str == NO_STORE) {
            noStore_ = true;
        } else if (str == NO_TRANSFORM) {
            noTransform_ = true;
        } else if (str == ONLY_IF_CACHED) {
            onlyIfCached_ = true;
        }
        auto pos = str.find(EQUAL);
        if (pos != std::string::npos) {
            std::string key = str.substr(0, pos);
            std::string value = str.substr(pos + 1);
            if (key == MAX_AGE) {
                maxAge_ = value;
            } else if (key == MAX_STALE) {
                maxStale_ = value;
            } else if (key == MIN_FRESH) {
                minFresh_ = value;
            }
        }
    }
}

void HttpCacheRequest::ParseRequestHeader(const std::map<std::string, std::string> &requestHeader)
{
    if (requestHeader.empty()) {
        return;
    }

    for (const auto &iterRequest : requestHeader) {
        std::string key = CommonUtils::ToLower(iterRequest.first);
        std::string value = iterRequest.second;

        if (key == CACHE_CONTROL) {
            ParseCacheControl(value);
        } else if (key == IF_MODIFIED_SINCE) {
            ifModifiedSince_ = value;
        } else if (key == IF_NONE_MATCH) {
            ifNoneMatch_ = value;
        }
    }
}

void HttpCacheRequest::SetRequestTime(const std::string &requestTime)
{
    requestTime_ = requestTime;
}

time_t HttpCacheRequest::GetIfModifiedSince() const
{
    if (ifModifiedSince_.empty()) {
        return INVALID_TIME;
    }
    return HttpTime::StrTimeToTimestamp(ifModifiedSince_);
}

time_t HttpCacheRequest::GetRequestTime() const
{
    if (requestTime_.empty()) {
        return INVALID_TIME;
    }
    return HttpTime::StrTimeToTimestamp(requestTime_);
}

time_t HttpCacheRequest::GetMaxAgeSeconds() const
{
    if (maxAge_.empty()) {
        return INVALID_TIME;
    }

    return std::strtol(maxAge_.c_str(), nullptr, DECIMAL);
}

time_t HttpCacheRequest::GetMaxStaleSeconds() const
{
    if (maxStale_.empty()) {
        return INVALID_TIME;
    }

    return std::strtol(maxStale_.c_str(), nullptr, DECIMAL);
}

time_t HttpCacheRequest::GetMinFreshSeconds() const
{
    if (minFresh_.empty()) {
        return INVALID_TIME;
    }
    return std::strtol(minFresh_.c_str(), nullptr, DECIMAL);
}

bool HttpCacheRequest::IsNoCache() const
{
    return noCache_;
}

bool HttpCacheRequest::IsNoStore() const
{
    return noStore_;
}

bool HttpCacheRequest::IsNoTransform() const
{
    return noTransform_;
}

bool HttpCacheRequest::IsOnlyIfCached() const
{
    return onlyIfCached_;
}

std::string HttpCacheRequest::GetIfNoneMatch() const
{
    return ifNoneMatch_;
}

HttpCacheRequest::HttpCacheRequest() : noCache_(false), noStore_(false), noTransform_(false), onlyIfCached_(false) {}

HttpCacheRequest::~HttpCacheRequest() = default;
} // namespace OHOS::NetStack
