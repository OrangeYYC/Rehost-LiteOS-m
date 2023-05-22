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

#include "http_cache_response.h"

#include "casche_constant.h"
#include "netstack_common_utils.h"

namespace OHOS::NetStack {
void HttpCacheResponse::ParseCacheControl(const std::string &cacheControl)
{
    auto vec = CommonUtils::Split(cacheControl, SPLIT);

    for (const auto &str : vec) {
        if (str == NO_CACHE) {
            noCache_ = true;
        } else if (str == NO_STORE) {
            noStore_ = true;
        } else if (str == NO_TRANSFORM) {
            noTransform_ = true;
        } else if (str == MUST_REVALIDATE) {
            mustRevalidate_ = true;
        } else if (str == PUBLIC) {
            publicCache_ = true;
        } else if (str == PRIVATE) {
            privateCache_ = true;
        } else if (str == PROXY_REVALIDATE) {
            proxyRevalidate_ = true;
        }
        auto pos = str.find('=');
        if (pos != std::string::npos) {
            std::string key = str.substr(0, pos);
            std::string value = str.substr(pos + 1);
            if (key == MAX_AGE) {
                maxAge_ = value;
            } else if (key == S_MAXAGE) {
                sMaxAge_ = value;
            }
        }
    }
}

void HttpCacheResponse::ParseCacheResponseHeader(const std::map<std::string, std::string> &cacheResponseHeader)
{
    if (cacheResponseHeader.empty()) {
        return;
    }
    for (const auto &iterCacheResponse : cacheResponseHeader) {
        std::string key = CommonUtils::ToLower(iterCacheResponse.first);
        std::string value = iterCacheResponse.second;

        if (key == CACHE_CONTROL) {
            ParseCacheControl(value);
        } else if (key == EXPIRES) {
            expires_ = value;
        } else if (key == LAST_MODIFIED) {
            lastModified_ = value;
        } else if (key == ETAG) {
            etag_ = value;
        } else if (key == AGE) {
            age_ = value;
        } else if (key == DATE) {
            date_ = value;
        }
    }
}

time_t HttpCacheResponse::GetDate() const
{
    if (date_.empty()) {
        return INVALID_TIME;
    }

    return HttpTime::StrTimeToTimestamp(date_);
}

time_t HttpCacheResponse::GetExpires() const
{
    if (expires_.empty()) {
        return INVALID_TIME;
    }

    return HttpTime::StrTimeToTimestamp(expires_);
}

time_t HttpCacheResponse::GetLastModified() const
{
    if (lastModified_.empty()) {
        return INVALID_TIME;
    }

    return HttpTime::StrTimeToTimestamp(lastModified_);
}

std::string HttpCacheResponse::GetLastModifiedStr() const
{
    return lastModified_;
}

std::string HttpCacheResponse::GetDateStr() const
{
    return date_;
}

std::string HttpCacheResponse::GetEtag() const
{
    return etag_;
}

std::string HttpCacheResponse::GetAge() const
{
    return age_;
}

time_t HttpCacheResponse::GetAgeSeconds() const
{
    if (age_.empty()) {
        return INVALID_TIME;
    }
    return std::strtol(age_.c_str(), nullptr, DECIMAL);
}

time_t HttpCacheResponse::GetMaxAgeSeconds() const
{
    if (maxAge_.empty()) {
        return INVALID_TIME;
    }
    return std::strtol(maxAge_.c_str(), nullptr, DECIMAL);
}

time_t HttpCacheResponse::GetSMaxAgeSeconds() const
{
    if (sMaxAge_.empty()) {
        return INVALID_TIME;
    }
    return std::strtol(sMaxAge_.c_str(), nullptr, DECIMAL);
}

time_t HttpCacheResponse::GetResponseTime() const
{
    if (responseTime_.empty()) {
        return INVALID_TIME;
    }
    return HttpTime::StrTimeToTimestamp(responseTime_);
}

bool HttpCacheResponse::IsMustRevalidate() const
{
    return mustRevalidate_;
}

bool HttpCacheResponse::IsNoCache() const
{
    return noCache_;
}

bool HttpCacheResponse::IsNoStore() const
{
    return noStore_;
}

bool HttpCacheResponse::IsPublicCache() const
{
    return publicCache_;
}

bool HttpCacheResponse::IsPrivateCache() const
{
    return privateCache_;
}

bool HttpCacheResponse::IsProxyRevalidate() const
{
    return proxyRevalidate_;
}

bool HttpCacheResponse::IsNoTransform() const
{
    return noTransform_;
}

ResponseCode HttpCacheResponse::GetRespCode() const
{
    return respCode_;
}

void HttpCacheResponse::SetResponseTime(const std::string &responseTime)
{
    responseTime_ = responseTime;
}

void HttpCacheResponse::SetRespCode(ResponseCode respCode)
{
    respCode_ = respCode;
}

void HttpCacheResponse::SetRequestTime(const std::string &requestTime)
{
    requestTime_ = requestTime;
}

time_t HttpCacheResponse::GetRequestTime() const
{
    if (requestTime_.empty()) {
        return INVALID_TIME;
    }
    return HttpTime::StrTimeToTimestamp(requestTime_);
}

HttpCacheResponse::HttpCacheResponse()
    : mustRevalidate_(false),
      noCache_(false),
      noStore_(false),
      publicCache_(false),
      privateCache_(false),
      proxyRevalidate_(false),
      noTransform_(false),
      respCode_(static_cast<ResponseCode>(0))
{
}

HttpCacheResponse::~HttpCacheResponse() = default;
} // namespace OHOS::NetStack
