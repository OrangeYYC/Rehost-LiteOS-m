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

#include "constant.h"
#include "curl/curl.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"

#include "http_request_options.h"

namespace OHOS::NetStack {
static constexpr const uint32_t MIN_PRIORITY = 0;
static constexpr const uint32_t MAX_PRIORITY = 1000;

HttpRequestOptions::HttpRequestOptions()
    : method_(HttpConstant::HTTP_METHOD_GET),
      readTimeout_(HttpConstant::DEFAULT_READ_TIMEOUT),
      connectTimeout_(HttpConstant::DEFAULT_CONNECT_TIMEOUT),
      usingProtocol_(HttpProtocol::HTTP_NONE),
      dataType_(HttpDataType::NO_DATA_TYPE),
      priority_(0),
      usingHttpProxyType_(UsingHttpProxyType::NOT_USE),
      httpProxyPort_(0)
{
    header_[CommonUtils::ToLower(HttpConstant::HTTP_CONTENT_TYPE)] = HttpConstant::HTTP_CONTENT_TYPE_JSON; // default
}

void HttpRequestOptions::SetUrl(const std::string &url)
{
    url_ = url;
}

void HttpRequestOptions::SetMethod(const std::string &method)
{
    method_ = method;
}

void HttpRequestOptions::SetBody(const void *data, size_t length)
{
    body_.append(static_cast<const char *>(data), length);
}

void HttpRequestOptions::SetHeader(const std::string &key, const std::string &val)
{
    header_[key] = val;
}

void HttpRequestOptions::SetReadTimeout(uint32_t readTimeout)
{
    readTimeout_ = readTimeout;
}

void HttpRequestOptions::SetConnectTimeout(uint32_t connectTimeout)
{
    connectTimeout_ = connectTimeout;
}

const std::string &HttpRequestOptions::GetUrl() const
{
    return url_;
}

const std::string &HttpRequestOptions::GetMethod() const
{
    return method_;
}

const std::string &HttpRequestOptions::GetBody() const
{
    return body_;
}

const std::map<std::string, std::string> &HttpRequestOptions::GetHeader() const
{
    return header_;
}

uint32_t HttpRequestOptions::GetReadTimeout() const
{
    return readTimeout_;
}

uint32_t HttpRequestOptions::GetConnectTimeout() const
{
    return connectTimeout_;
}

void HttpRequestOptions::SetUsingProtocol(HttpProtocol httpProtocol)
{
    usingProtocol_ = httpProtocol;
}

uint32_t HttpRequestOptions::GetHttpVersion() const
{
    if (usingProtocol_ == HttpProtocol::HTTP2) {
        NETSTACK_LOGI("CURL_HTTP_VERSION_2_0");
        return CURL_HTTP_VERSION_2_0;
    }
    if (usingProtocol_ == HttpProtocol::HTTP1_1) {
        NETSTACK_LOGI("CURL_HTTP_VERSION_1_1");
        return CURL_HTTP_VERSION_1_1;
    }
    return CURL_HTTP_VERSION_NONE;
}

void HttpRequestOptions::SetRequestTime(const std::string &time)
{
    requestTime_ = time;
}

const std::string &HttpRequestOptions::GetRequestTime() const
{
    return requestTime_;
}

void HttpRequestOptions::SetHttpDataType(HttpDataType dataType)
{
    if (dataType != HttpDataType::STRING && dataType != HttpDataType::ARRAY_BUFFER &&
        dataType != HttpDataType::OBJECT) {
        return;
    }
    dataType_ = dataType;
}
HttpDataType HttpRequestOptions::GetHttpDataType() const
{
    return dataType_;
}

void HttpRequestOptions::SetPriority(uint32_t priority)
{
    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        return;
    }
    priority_ = priority;
}

uint32_t HttpRequestOptions::GetPriority() const
{
    return priority_;
}

void HttpRequestOptions::SetUsingHttpProxyType(UsingHttpProxyType type)
{
    usingHttpProxyType_ = type;
}

UsingHttpProxyType HttpRequestOptions::GetUsingHttpProxyType() const
{
    return usingHttpProxyType_;
}

void HttpRequestOptions::SetSpecifiedHttpProxy(const std::string &host, int32_t port, const std::string &exclusionList)
{
    httpProxyHost_ = host;
    httpProxyPort_ = port;
    httpProxyExclusions_ = exclusionList;
}

void HttpRequestOptions::GetSpecifiedHttpProxy(std::string &host, int32_t &port, std::string &exclusionList)
{
    host = httpProxyHost_;
    port = httpProxyPort_;
    exclusionList = httpProxyExclusions_;
}
} // namespace OHOS::NetStack