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

#include <memory>

#include "http_constant.h"
#include "http_request_utils.h"

#include "http_request.h"

#define ACE_CURL_EASY_SET_OPTION(handle, opt, data, respData)              \
    do {                                                                   \
        CURLcode result = curl_easy_setopt(handle, opt, data);             \
        if (result != CURLE_OK) {                                          \
            const char *err = curl_easy_strerror(result);                  \
            HTTP_REQUEST_ERROR("Failed to set option: %s, %s", #opt, err); \
            (respData)->SetErrString(err);                                 \
            (respData)->SetCode(result);                                   \
            return false;                                                  \
        }                                                                  \
    } while (0)

#define ACE_CURL_EASY_PERFORM(handle, reqData, respData)                                         \
    do {                                                                                         \
        CURLcode result = curl_easy_perform(handle);                                             \
        if (result != CURLE_OK) {                                                                \
            const char *err = curl_easy_strerror(result);                                        \
            HTTP_REQUEST_ERROR("Failed to fetch, url:%s, %s", (reqData)->GetUrl().c_str(), err); \
            (respData)->SetErrString(err);                                                       \
            (respData)->SetCode(result);                                                         \
            return false;                                                                        \
        }                                                                                        \
    } while (0)

#define ACE_CURL_EASY_GET_INFO(handle, opt, data, respData)              \
    do {                                                                 \
        CURLcode result = curl_easy_getinfo(handle, opt, data);          \
        if (result != CURLE_OK) {                                        \
            const char *err = curl_easy_strerror(result);                \
            HTTP_REQUEST_ERROR("Failed to get info: %s, %s", #opt, err); \
            (respData)->SetErrString(err);                               \
            (respData)->SetCode(result);                                 \
            return false;                                                \
        }                                                                \
    } while (0)

namespace OHOS {
namespace ACELite {

std::mutex HttpRequest::mutex;

bool HttpRequest::initialized = false;

bool HttpRequest::Initialize()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (initialized) {
        return true;
    }
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        HTTP_REQUEST_ERROR("Failed to initialize 'curl'");
        return false;
    }
    initialized = true;
    return true;
}

bool HttpRequest::Request(RequestData *requestData, ResponseData *responseData)
{
    if (!Initialize()) {
        return false;
    }
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> handle(curl_easy_init(), curl_easy_cleanup);
    if (!handle) {
        HTTP_REQUEST_ERROR("Failed to create fetch task");
        return false;
    }

#if NO_SSL_CERTIFICATION
    // in real life, you should buy a ssl certification and rename it to /etc/ssl/cert.pem
    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_SSL_VERIFYPEER, 0L, responseData);
#endif // NO_SSL_CERTIFICATION

    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_WRITEFUNCTION, OnWritingMemoryBody, responseData);
    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_WRITEDATA, responseData, responseData);

    std::string responseHeader;
    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_HEADERFUNCTION, OnWritingMemoryHeader, responseData);
    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_HEADERDATA, &responseHeader, responseData);

    // Some servers don't like requests that are made without a user-agent field, so we provide one
    ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_USERAGENT, HttpConstant::HTTP_DEFAULT_USER_AGENT, responseData);

    if (!SetOption(requestData, handle.get(), responseData)) {
        HTTP_REQUEST_ERROR("set option failed");
        return false;
    }

    std::vector<std::string> vec(responseData->GetHeaders().size());
    for (const auto &p : requestData->GetHeader()) {
        vec.emplace_back(p.first + HttpConstant::HTTP_HEADER_SEPARATOR + p.second);
    }
    std::unique_ptr<struct curl_slist, decltype(&curl_slist_free_all)> header(MakeHeaders(vec), curl_slist_free_all);
    if (header != nullptr) {
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_HTTPHEADER, header.get(), responseData);
    }

    ACE_CURL_EASY_PERFORM(handle.get(), requestData, responseData);

    int32_t responseCode;
    ACE_CURL_EASY_GET_INFO(handle.get(), CURLINFO_RESPONSE_CODE, &responseCode, responseData);

    responseData->SetCode(responseCode);
    responseData->ParseHeaders(responseHeader);

    return true;
}

bool HttpRequest::SetOption(RequestData *requestData, CURL *curl, ResponseData *responseData)
{
    const std::string &method = requestData->GetMethod();
    if (!MethodForGet(method) && !MethodForPost(method)) {
        HTTP_REQUEST_ERROR("method %s not supported", method.c_str());
        return false;
    }
    ACE_CURL_EASY_SET_OPTION(curl, CURLOPT_CUSTOMREQUEST, method.c_str(), responseData);

    if (MethodForPost(method)) {
        ACE_CURL_EASY_SET_OPTION(curl, CURLOPT_POST, 1L, responseData);
    }

    if (!requestData->GetBody().empty()) {
        ACE_CURL_EASY_SET_OPTION(curl, CURLOPT_POSTFIELDS, requestData->GetBody().c_str(), responseData);
        ACE_CURL_EASY_SET_OPTION(curl, CURLOPT_POSTFIELDSIZE, requestData->GetBody().size(), responseData);
    }

    if (EncodeUrlParam(curl, const_cast<std::string &>(requestData->GetUrl())) && MethodForGet(method)) {
        requestData->SetHeader(HttpConstant::HTTP_HEADER_KEY_CONTENT_TYPE, HttpConstant::HTTP_CONTENT_TYPE_URL_ENCODE);
    }

    HTTP_REQUEST_INFO("final url: ...");
    ACE_CURL_EASY_SET_OPTION(curl, CURLOPT_URL, requestData->GetUrl().c_str(), responseData);
    return true;
}

size_t HttpRequest::OnWritingMemoryBody(const void *data, size_t size, size_t memBytes, void *userData)
{
    static_cast<ResponseData *>(userData)->AppendData(static_cast<const char *>(data), size * memBytes);
    return size * memBytes;
}

size_t HttpRequest::OnWritingMemoryHeader(const void *data, size_t size, size_t memBytes, void *userData)
{
    static_cast<std::string *>(userData)->append(static_cast<const char *>(data), size * memBytes);
    return size * memBytes;
}

struct curl_slist *HttpRequest::MakeHeaders(const std::vector<std::string> &vec)
{
    struct curl_slist *header = nullptr;
    for (const auto &s : vec) {
        header = curl_slist_append(header, s.c_str());
    }
    return header;
}

} // namespace ACELite
} // namespace OHOS
