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

#ifndef COMMUNICATIONNETSTACK_REQUEST_CONTEXT_H
#define COMMUNICATIONNETSTACK_REQUEST_CONTEXT_H

#include "curl/curl.h"
#include "base_context.h"
#include "http_request_options.h"
#include "http_response.h"

namespace OHOS::NetStack {
class RequestContext final : public BaseContext {
public:
    RequestContext() = delete;

    RequestContext(napi_env env, EventManager *manager);

    ~RequestContext() override;

    void ParseParams(napi_value *params, size_t paramsCount) override;

    HttpRequestOptions options;

    HttpResponse response;

    [[nodiscard]] bool IsUsingCache() const;

    void SetCurlHeaderList(struct curl_slist *curlHeaderList);

    struct curl_slist *GetCurlHeaderList();

    napi_env GetEnv();

    void SetCacheResponse(const HttpResponse &cacheResponse);

    void SetResponseByCache();

    [[nodiscard]] int32_t GetErrorCode() const override;

    [[nodiscard]] std::string GetErrorMessage() const override;

    void EnableRequest2();

    [[nodiscard]] bool IsRequest2();

    void SetTotalLen(curl_off_t totalLen);

    curl_off_t GetTotalLen();

    void SetNowLen(curl_off_t nowLen);

    curl_off_t GetNowLen();

    void SetTempData(const void *data, size_t size);

    std::string &GetTempData();

private:
    bool usingCache_;
    bool request2_;
    curl_off_t totalLen_;
    curl_off_t nowLen_;
    std::string tempData_;

    struct curl_slist *curlHeaderList_;

    HttpResponse cacheResponse_;

    bool CheckParamsType(napi_value *params, size_t paramsCount);

    void ParseNumberOptions(napi_value optionsValue);

    void ParseHeader(napi_value optionsValue);

    bool ParseExtraData(napi_value optionsValue);

    void ParseUsingHttpProxy(napi_value optionsValue);

    bool GetRequestBody(napi_value extraData);

    void UrlAndOptions(napi_value urlValue, napi_value optionsValue);

    static std::map<RequestContext *, napi_env> envMap_;
    static std::mutex envMutex_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_REQUEST_CONTEXT_H */
