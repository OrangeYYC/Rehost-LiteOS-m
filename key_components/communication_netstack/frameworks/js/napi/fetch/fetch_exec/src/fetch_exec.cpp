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

#include "fetch_exec.h"

#include <algorithm>
#include <cstring>
#include <memory>

#include "constant.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "napi_utils.h"
#include "securec.h"

#define NETSTACK_CURL_EASY_SET_OPTION(handle, opt, data, asyncContext)                                   \
    do {                                                                                                 \
        CURLcode result = curl_easy_setopt(handle, opt, data);                                           \
        if (result != CURLE_OK) {                                                                        \
            const char *err = curl_easy_strerror(result);                                                \
            NETSTACK_LOGE("Failed to set option: %{public}s, %{public}s %{public}d", #opt, err, result); \
            (asyncContext)->SetErrorCode(result);                                                        \
            return false;                                                                                \
        }                                                                                                \
    } while (0)

#define NETSTACK_CURL_EASY_PERFORM(handle, asyncContext)                                                 \
    do {                                                                                                 \
        CURLcode result = curl_easy_perform(handle);                                                     \
        if (result != CURLE_OK) {                                                                        \
            NETSTACK_LOGE("request fail, url:%{public}s, %{public}s %{public}d",                         \
                          (asyncContext)->request.GetUrl().c_str(), curl_easy_strerror(result), result); \
            (asyncContext)->SetErrorCode(result);                                                        \
            return false;                                                                                \
        }                                                                                                \
    } while (0)

#define NETSTACK_CURL_EASY_GET_INFO(handle, opt, data, asyncContext)                                   \
    do {                                                                                               \
        CURLcode result = curl_easy_getinfo(handle, opt, data);                                        \
        if (result != CURLE_OK) {                                                                      \
            const char *err = curl_easy_strerror(result);                                              \
            NETSTACK_LOGE("Failed to get info: %{public}s, %{public}s %{public}d", #opt, err, result); \
            (asyncContext)->SetErrorCode(result);                                                      \
            return false;                                                                              \
        }                                                                                              \
    } while (0)

static constexpr const int FAIL_CALLBACK_PARAM = 2;

namespace OHOS::NetStack {
std::mutex FetchExec::mutex_;

bool FetchExec::initialized_ = false;

bool FetchExec::ExecFetch(FetchContext *context)
{
    if (!initialized_) {
        NETSTACK_LOGE("curl not init");
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> handle(curl_easy_init(), curl_easy_cleanup);

    if (!handle) {
        NETSTACK_LOGE("Failed to create fetch task");
        return false;
    }

    NETSTACK_LOGI("final url: %{public}s", context->request.GetUrl().c_str());

    std::vector<std::string> vec;
    std::for_each(context->request.GetHeader().begin(), context->request.GetHeader().end(),
                  [&vec](const std::pair<std::string, std::string> &p) {
                      vec.emplace_back(p.first + FetchConstant::HTTP_HEADER_SEPARATOR + p.second);
                  });
    std::unique_ptr<struct curl_slist, decltype(&curl_slist_free_all)> header(MakeHeaders(vec), curl_slist_free_all);

    if (!SetOption(handle.get(), context, header.get())) {
        NETSTACK_LOGE("set option failed");
        return false;
    }

    NETSTACK_CURL_EASY_PERFORM(handle.get(), context);

    int32_t responseCode;
    NETSTACK_CURL_EASY_GET_INFO(handle.get(), CURLINFO_RESPONSE_CODE, &responseCode, context);

    context->response.SetResponseCode(responseCode);
    context->response.ParseHeaders();

    return true;
}

napi_value FetchExec::FetchCallback(FetchContext *context)
{
    if (context->IsExecOK()) {
        napi_value success = context->GetSuccessCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), success) == napi_function) {
            napi_value response = MakeResponse(context);
            napi_value undefined = NapiUtils::GetUndefined(context->GetEnv());
            napi_value argv[1] = {response};
            (void)NapiUtils::CallFunction(context->GetEnv(), undefined, success, 1, argv);
        }
    } else {
        napi_value fail = context->GetFailCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), fail) == napi_function) {
            napi_value errData = NapiUtils::GetUndefined(context->GetEnv());
            napi_value errCode = NapiUtils::CreateUint32(context->GetEnv(), context->GetErrorCode());
            napi_value undefined = NapiUtils::GetUndefined(context->GetEnv());
            napi_value argv[FAIL_CALLBACK_PARAM] = {errData, errCode};
            (void)NapiUtils::CallFunction(context->GetEnv(), undefined, fail, FAIL_CALLBACK_PARAM, argv);
        }
    }

    napi_value complete = context->GetCompleteCallback();
    if (NapiUtils::GetValueType(context->GetEnv(), complete) == napi_function) {
        napi_value undefined = NapiUtils::GetUndefined(context->GetEnv());
        (void)NapiUtils::CallFunction(context->GetEnv(), undefined, complete, 0, nullptr);
    }

    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value FetchExec::MakeResponse(FetchContext *context)
{
    napi_value object = NapiUtils::CreateObject(context->GetEnv());
    if (NapiUtils::GetValueType(context->GetEnv(), object) != napi_object) {
        return nullptr;
    }

    NapiUtils::SetUint32Property(context->GetEnv(), object, FetchConstant::RESPONSE_KEY_CODE,
                                 context->response.GetResponseCode());

    napi_value header = MakeResponseHeader(context);
    if (NapiUtils::GetValueType(context->GetEnv(), header) == napi_object) {
        NapiUtils::SetNamedProperty(context->GetEnv(), object, FetchConstant::RESPONSE_KEY_HEADERS, header);
    }

    if (CommonUtils::ToLower(context->GetResponseType()) == FetchConstant::HTTP_RESPONSE_TYPE_JSON) {
        napi_value data = NapiUtils::JsonParse(
            context->GetEnv(), NapiUtils::CreateStringUtf8(context->GetEnv(), context->response.GetData()));
        NapiUtils::SetNamedProperty(context->GetEnv(), object, FetchConstant::RESPONSE_KEY_DATA, data);
        return object;
    }

    /* now just support utf8 */
    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), object, FetchConstant::RESPONSE_KEY_DATA,
                                     context->response.GetData());
    return object;
}

std::string FetchExec::MakeUrl(const std::string &url, std::string param, const std::string &extraParam)
{
    if (param.empty()) {
        param += extraParam;
    } else {
        param += FetchConstant::HTTP_URL_PARAM_SEPARATOR;
        param += extraParam;
    }

    if (param.empty()) {
        return url;
    }

    return url + FetchConstant::HTTP_URL_PARAM_START + param;
}

bool FetchExec::MethodForGet(const std::string &method)
{
    return (method == FetchConstant::HTTP_METHOD_HEAD || method == FetchConstant::HTTP_METHOD_OPTIONS ||
            method == FetchConstant::HTTP_METHOD_DELETE || method == FetchConstant::HTTP_METHOD_TRACE ||
            method == FetchConstant::HTTP_METHOD_GET || method == FetchConstant::HTTP_METHOD_CONNECT);
}

bool FetchExec::MethodForPost(const std::string &method)
{
    return (method == FetchConstant::HTTP_METHOD_POST || method == FetchConstant::HTTP_METHOD_PUT);
}

bool FetchExec::EncodeUrlParam(std::string &str)
{
    char encoded[4];
    std::string encodeOut;
    for (size_t i = 0; i < strlen(str.c_str()); ++i) {
        auto c = static_cast<uint8_t>(str.c_str()[i]);
        if (IsUnReserved(c)) {
            encodeOut += static_cast<char>(c);
        } else {
            if (sprintf_s(encoded, sizeof(encoded), "%%%02X", c) < 0) {
                return false;
            }
            encodeOut += encoded;
        }
    }

    if (str == encodeOut) {
        return false;
    }
    str = encodeOut;
    return true;
}

bool FetchExec::Initialize()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) {
        return true;
    }
    NETSTACK_LOGI("call curl_global_init");
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        NETSTACK_LOGE("Failed to initialize 'curl'");
        return false;
    }
    initialized_ = true;
    return initialized_;
}

bool FetchExec::SetOption(CURL *curl, FetchContext *context, struct curl_slist *requestHeader)
{
    const std::string &method = context->request.GetMethod();
    if (!MethodForGet(method) && !MethodForPost(method)) {
        NETSTACK_LOGE("method %{public}s not supported", method.c_str());
        return false;
    }

    if (context->request.GetMethod() == FetchConstant::HTTP_METHOD_HEAD) {
        NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_NOBODY, 1L, context);
    }

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_URL, context->request.GetUrl().c_str(), context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_CUSTOMREQUEST, method.c_str(), context);

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_WRITEFUNCTION, OnWritingMemoryBody, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_WRITEDATA, context, context);

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_HEADERFUNCTION, OnWritingMemoryHeader, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_HEADERDATA, context, context);

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_HTTPHEADER, requestHeader, context);

    // Some servers don't like requests that are made without a user-agent field, so we provide one
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_USERAGENT, FetchConstant::HTTP_DEFAULT_USER_AGENT, context);

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_FOLLOWLOCATION, 1L, context);

    /* first #undef CURL_DISABLE_COOKIES in curl config */
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_COOKIEFILE, "", context);

#if NETSTACK_USE_PROXY
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_PROXY, NETSTACK_PROXY_URL_PORT, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_PROXYTYPE, NETSTACK_PROXY_TYPE, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_HTTPPROXYTUNNEL, 1L, context);
#ifdef NETSTACK_PROXY_PASS
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_PROXYUSERPWD, NETSTACK_PROXY_PASS, context);
#endif // NETSTACK_PROXY_PASS
#endif // NETSTACK_USE_PROXY

#if NO_SSL_CERTIFICATION
    // in real life, you should buy a ssl certification and rename it to /etc/ssl/cert.pem
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_SSL_VERIFYHOST, 0L, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_SSL_VERIFYPEER, 0L, context);
#else
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_CAINFO, FetchConstant::HTTP_DEFAULT_CA_PATH, context);
#endif // NO_SSL_CERTIFICATION

    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_NOPROGRESS, 1L, context);
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_NOSIGNAL, 1L, context);
#if HTTP_CURL_PRINT_VERBOSE
    NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_VERBOSE, 1L, context);
#endif

    if (MethodForPost(method) && !context->request.GetBody().empty()) {
        NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_POST, 1L, context);
        NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_POSTFIELDS, context->request.GetBody().c_str(), context);
        NETSTACK_CURL_EASY_SET_OPTION(curl, CURLOPT_POSTFIELDSIZE, context->request.GetBody().size(), context);
    }

    return true;
}

size_t FetchExec::OnWritingMemoryBody(const void *data, size_t size, size_t memBytes, void *userData)
{
    auto context = static_cast<FetchContext *>(userData);
    context->response.AppendData(data, size * memBytes);
    return size * memBytes;
}

size_t FetchExec::OnWritingMemoryHeader(const void *data, size_t size, size_t memBytes, void *userData)
{
    auto context = static_cast<FetchContext *>(userData);
    context->response.AppendRawHeader(data, size * memBytes);
    return size * memBytes;
}

struct curl_slist *FetchExec::MakeHeaders(const std::vector<std::string> &vec)
{
    struct curl_slist *header = nullptr;
    std::for_each(vec.begin(), vec.end(), [&header](const std::string &s) {
        if (!s.empty()) {
            header = curl_slist_append(header, s.c_str());
        }
    });
    return header;
}

napi_value FetchExec::MakeResponseHeader(FetchContext *context)
{
    napi_value header = NapiUtils::CreateObject(context->GetEnv());
    if (NapiUtils::GetValueType(context->GetEnv(), header) == napi_object) {
        std::for_each(context->response.GetHeader().begin(), context->response.GetHeader().end(),
                      [context, header](const std::pair<std::string, std::string> &p) {
                          if (!p.first.empty() && !p.second.empty()) {
                              NapiUtils::SetStringPropertyUtf8(context->GetEnv(), header, p.first, p.second);
                          }
                      });
    }
    return header;
}

bool FetchExec::IsUnReserved(unsigned char in)
{
    if ((in >= '0' && in <= '9') || (in >= 'a' && in <= 'z') || (in >= 'A' && in <= 'Z')) {
        return true;
    }
    switch (in) {
        case '-':
        case '.':
        case '_':
        case '~':
            return true;
        default:
            break;
    }
    return false;
}
} // namespace OHOS::NetStack
