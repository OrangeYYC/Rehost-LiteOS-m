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

#include "http_cache_strategy.h"

#include <algorithm>
#include <cstring>

#include "casche_constant.h"
#include "constant.h"
#include "http_cache_request.h"
#include "http_cache_response.h"
#include "http_time.h"
#include "netstack_log.h"

static constexpr int64_t ONE_DAY_MILLISECONDS = 24 * 60 * 60 * 1000L;
static constexpr int64_t CONVERT_TO_MILLISECONDS = 1000;
static constexpr const char *KEY_RANGE = "range";

// RFC 7234

namespace OHOS::NetStack {
HttpCacheStrategy::HttpCacheStrategy(HttpRequestOptions &requestOptions) : requestOptions_(requestOptions)
{
    cacheRequest_.ParseRequestHeader(requestOptions_.GetHeader());
    cacheRequest_.SetRequestTime(requestOptions_.GetRequestTime());
}

bool HttpCacheStrategy::CouldUseCache()
{
    return requestOptions_.GetMethod() == HttpConstant::HTTP_METHOD_GET ||
           requestOptions_.GetMethod() == HttpConstant::HTTP_METHOD_HEAD ||
           requestOptions_.GetHeader().find(KEY_RANGE) != requestOptions_.GetHeader().end();
}

CacheStatus HttpCacheStrategy::RunStrategy(HttpResponse &response)
{
    cacheResponse_.ParseCacheResponseHeader(response.GetHeader());
    cacheResponse_.SetRespCode(static_cast<ResponseCode>(response.GetResponseCode()));
    cacheResponse_.SetResponseTime(response.GetResponseTime());
    cacheResponse_.SetRequestTime(response.GetRequestTime());
    return RunStrategyInternal(response);
}

bool HttpCacheStrategy::IsCacheable(const HttpResponse &response)
{
    if (!CouldUseCache()) {
        return false;
    }
    HttpCacheResponse tempCacheResponse;
    tempCacheResponse.ParseCacheResponseHeader(response.GetHeader());
    tempCacheResponse.SetRespCode(static_cast<ResponseCode>(response.GetResponseCode()));
    return IsCacheable(tempCacheResponse);
}

int64_t HttpCacheStrategy::CacheResponseAgeMillis()
{
    // 4.2.3. Calculating Age

    /* The following data is used for the age calculation:

    age_value
    The term "age_value" denotes the value of the Age header field (Section 5.1), in a form appropriate for arithmetic
    operation; or 0, if not available.

    date_value
    The term "date_value" denotes the value of the Date header field, in a form appropriate for arithmetic operations.
    See Section 7.1.1.2 of [RFC7231] for the definition of the Date header field, and for requirements regarding
    responses without it.

    now
    The term "now" means "the current value of the clock at the host performing the calculation". A host ought to use
    NTP ([RFC5905]) or some similar protocol to synchronize its clocks to Coordinated Universal Time.

    request_time
    The current value of the clock at the host at the time the request resulting in the stored response was made.

    response_time
    The current value of the clock at the host at the time the response was received.
    A response's age can be calculated in two entirely independent ways:

    the "apparent_age": response_time minus date_value, if the local clock is reasonably well synchronized to the origin
    server's clock. If the result is negative, the result is replaced by zero. the "corrected_age_value", if all of the
    caches along the response path implement HTTP/1.1. A cache MUST interpret this value relative to the time the
    request was initiated, not the time that the response was received. apparent_age = max(0, response_time -
    date_value);

    response_delay = response_time - request_time;
    corrected_age_value = age_value + response_delay;
    These are combined as

    corrected_initial_age = max(apparent_age, corrected_age_value);
    unless the cache is confident in the value of the Age header field (e.g., because there are no HTTP/1.0 hops in the
    Via header field), in which case the corrected_age_value MAY be used as the corrected_initial_age.

    The current_age of a stored response can then be calculated by adding the amount of time (in seconds) since the
    stored response was last validated by the origin server to the corrected_initial_age.

    resident_time = now - response_time;
    current_age = corrected_initial_age + resident_time; */

    int64_t age = std::max<int64_t>(0, cacheResponse_.GetAgeSeconds());
    int64_t dateTime = std::max<int64_t>(0, cacheResponse_.GetDate());
    int64_t nowTime = std::max<int64_t>(0, HttpTime::GetNowTimeSeconds());
    int64_t requestTime = std::max<int64_t>(0, cacheResponse_.GetRequestTime());
    int64_t responseTime = std::max<int64_t>(0, cacheResponse_.GetResponseTime());

    NETSTACK_LOGI("CacheResponseAgeMillis: %{public}lld, %{public}lld, %{public}lld, %{public}lld, %{public}lld",
                  static_cast<long long>(age), static_cast<long long>(dateTime), static_cast<long long>(nowTime),
                  static_cast<long long>(requestTime), static_cast<long long>(responseTime));

    int64_t apparentAge = std::max<int64_t>(0, responseTime - dateTime);
    int64_t responseDelay = std::max<int64_t>(0, responseTime - requestTime);
    int64_t correctedAgeValue = age + responseDelay;
    int64_t correctedInitialAge = std::max<int64_t>(apparentAge, correctedAgeValue);

    int64_t residentTime = std::max<int64_t>(0, nowTime - responseTime);

    return (correctedInitialAge + residentTime) * CONVERT_TO_MILLISECONDS;
}

int64_t HttpCacheStrategy::ComputeFreshnessLifetimeSecondsInternal()
{
    int64_t sMaxAge = cacheResponse_.GetSMaxAgeSeconds();
    if (sMaxAge != INVALID_TIME) {
        // If the cache is shared and the s-maxage response directive (Section 5.2.2.9) is present, use its value
        return sMaxAge;
    }

    int64_t maxAge = cacheResponse_.GetMaxAgeSeconds();
    if (maxAge != INVALID_TIME) {
        // If the max-age response directive (Section 5.2.2.8) is present, use its value
        return maxAge;
    }

    if (cacheResponse_.GetExpires() != INVALID_TIME) {
        // If the Expires response header field (Section 5.3) is present, use its value minus the value of the Date
        // response header field
        int64_t responseTime = cacheResponse_.GetResponseTime();
        if (cacheResponse_.GetDate() != INVALID_TIME) {
            responseTime = cacheResponse_.GetDate();
        }
        int64_t delta = cacheResponse_.GetExpires() - responseTime;
        return std::max<int64_t>(delta, 0);
    }

    if (cacheResponse_.GetLastModified() != INVALID_TIME) {
        // 4.2.2. Calculating Heuristic Freshness
        int64_t requestTime = cacheRequest_.GetRequestTime();
        if (cacheResponse_.GetDate() != INVALID_TIME) {
            requestTime = cacheResponse_.GetDate();
        }
        int64_t delta = requestTime - cacheResponse_.GetLastModified();
        return std::max<int64_t>(delta / DECIMAL, 0);
    }

    return 0;
}

int64_t HttpCacheStrategy::ComputeFreshnessLifetimeMillis()
{
    int64_t lifeTime = ComputeFreshnessLifetimeSecondsInternal();

    int64_t reqMaxAge = cacheRequest_.GetMaxAgeSeconds();
    if (reqMaxAge != INVALID_TIME) {
        lifeTime = std::min(lifeTime, reqMaxAge);
    }

    NETSTACK_LOGI("lifeTime=%{public}lld", static_cast<long long>(lifeTime));
    return lifeTime * CONVERT_TO_MILLISECONDS;
}

void HttpCacheStrategy::UpdateRequestHeader(const std::string &etag,
                                            const std::string &lastModified,
                                            const std::string &date)
{
    if (!etag.empty()) {
        requestOptions_.SetHeader(IF_NONE_MATCH, etag);
    } else if (!lastModified.empty()) {
        requestOptions_.SetHeader(IF_MODIFIED_SINCE, lastModified);
    } else if (!date.empty()) {
        requestOptions_.SetHeader(IF_MODIFIED_SINCE, date);
    }
}

bool HttpCacheStrategy::IsCacheable(const HttpCacheResponse &cacheResponse)
{
    switch (cacheResponse.GetRespCode()) {
        case ResponseCode::OK:
        case ResponseCode::NOT_AUTHORITATIVE:
        case ResponseCode::NO_CONTENT:
        case ResponseCode::MULT_CHOICE:
        case ResponseCode::MOVED_PERM:
        case ResponseCode::NOT_FOUND:
        case ResponseCode::BAD_METHOD:
        case ResponseCode::GONE:
        case ResponseCode::REQ_TOO_LONG:
        case ResponseCode::NOT_IMPLEMENTED:
            // These codes can be cached unless headers forbid it.
            break;

        case ResponseCode::MOVED_TEMP:
            if (cacheResponse.GetExpires() != INVALID_TIME || cacheResponse.GetMaxAgeSeconds() != INVALID_TIME ||
                cacheResponse.IsPublicCache() || cacheResponse.IsPrivateCache()) {
                break;
            }
            return false;

        default:
            return false;
    }

    return !cacheResponse.IsNoStore() && !cacheRequest_.IsNoStore();
}

std::tuple<int64_t, int64_t, int64_t, int64_t> HttpCacheStrategy::GetFreshness()
{
    int64_t ageMillis = CacheResponseAgeMillis();

    int64_t lifeTime = ComputeFreshnessLifetimeMillis();

    int64_t minFreshMillis = std::max<int64_t>(0, cacheRequest_.GetMinFreshSeconds() * CONVERT_TO_MILLISECONDS);

    int64_t maxStaleMillis = 0;
    if (!cacheResponse_.IsMustRevalidate()) {
        maxStaleMillis = std::max<int64_t>(0, cacheRequest_.GetMaxStaleSeconds() * CONVERT_TO_MILLISECONDS);
    }

    NETSTACK_LOGI("GetFreshness: %{public}lld, %{public}lld, %{public}lld, %{public}lld",
                  static_cast<long long>(ageMillis), static_cast<long long>(minFreshMillis),
                  static_cast<long long>(lifeTime), static_cast<long long>(maxStaleMillis));
    return {ageMillis, minFreshMillis, lifeTime, maxStaleMillis};
}

CacheStatus HttpCacheStrategy::RunStrategyInternal(HttpResponse &response)
{
    if (cacheRequest_.IsNoCache()) {
        NETSTACK_LOGI("return DENY");
        return DENY;
    }

    if (cacheResponse_.IsNoCache()) {
        NETSTACK_LOGI("return STALE");
        return STALE;
    }

    if (cacheRequest_.IsOnlyIfCached()) {
        NETSTACK_LOGI("return FRESH");
        return FRESH;
    }

    // T.B.D https and TLS handshake
    if (!IsCacheable(cacheResponse_)) {
        NETSTACK_LOGI("return DENY");
        return DENY;
    }

    if (cacheRequest_.GetIfModifiedSince() != INVALID_TIME || !cacheRequest_.GetIfNoneMatch().empty()) {
        NETSTACK_LOGI("return DENY");
        return DENY;
    }

    auto [ageMillis, minFreshMillis, lifeTime, maxStaleMillis] = GetFreshness();

    if (ageMillis + minFreshMillis < lifeTime + maxStaleMillis) {
        if (ageMillis + minFreshMillis >= lifeTime) {
            response.SetWarning("110 \"Response is STALE\"");
        }

        if (ageMillis > ONE_DAY_MILLISECONDS && cacheRequest_.GetMaxAgeSeconds() == INVALID_TIME &&
            cacheResponse_.GetExpires() == INVALID_TIME) {
            response.SetWarning("113 \"Heuristic expiration\"");
        }

        NETSTACK_LOGI("return FRESH");
        return FRESH;
    }

    // The cache has expired and the request needs to be re-initialized
    UpdateRequestHeader(cacheResponse_.GetEtag(), cacheResponse_.GetLastModifiedStr(), cacheResponse_.GetDateStr());

    NETSTACK_LOGI("return STALE");
    return STALE;
}
} // namespace OHOS::NetStack
