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

#include "http_async_callback.h"
#include "http_constant.h"
#include "http_request_utils.h"
#include "js_async_work.h"
#include "securec.h"

#include "fetch_module.h"

namespace OHOS {
namespace ACELite {

void InitFetchModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, FetchModule::HTTP_API_KEY_FETCH, FetchModule::Fetch);
}

static void FreeString(char *s)
{
    JSI::ReleaseString(s);
}

const char *const FetchModule::HTTP_API_KEY_FETCH = "fetch";
const char *const FetchModule::HTTP_API_KEY_STRING_TO_ARRAY_BUFFER = "stringToArrayBuffer";

JSIValue FetchModule::Fetch(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (argsNum < 1) {
        return JSI::CreateUndefined();
    }

    auto asyncCallback = new HttpAsyncCallback(thisVal);
    if (JsObjectToRequestData(args[0], &asyncCallback->requestData)) {
        asyncCallback->responseCallback[CB_SUCCESS] = JSI::GetNamedProperty(args[0], CB_SUCCESS);
        asyncCallback->responseCallback[CB_FAIL] = JSI::GetNamedProperty(args[0], CB_FAIL);
        asyncCallback->responseCallback[CB_COMPLETE] = JSI::GetNamedProperty(args[0], CB_COMPLETE);
        if (!JsAsyncWork::DispatchAsyncWork(HttpAsyncCallback::AsyncExecHttpRequest,
                                            static_cast<void *>(asyncCallback))) {
            delete asyncCallback;
            return JSI::CreateUndefined();
        }
    } else {
        delete asyncCallback;
    }

    return JSI::CreateUndefined();
}

bool FetchModule::JsObjectToRequestData(JSIValue options, RequestData *req)
{
    if (options == nullptr || JSI::ValueIsUndefined(options) || !JSI::ValueIsObject(options)) {
        return false;
    }

    std::unique_ptr<char, decltype(&FreeString)> urlString(
        JSI::GetStringProperty(options, HttpConstant::KEY_HTTP_REQUEST_URL), FreeString);
    if (urlString == nullptr) {
        return false;
    }
    HTTP_REQUEST_INFO("request url is ...");
    req->SetUrl(urlString.get());

    req->SetMethod(GetMethodFromOptions(options));
    HTTP_REQUEST_INFO("request method is %s", req->GetMethod().c_str());

    req->SetResponseType(GetResponseTypeFromOptions(options));

    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> jsHeaders(
        JSI::GetNamedProperty(options, HttpConstant::KEY_HTTP_REQUEST_HEADER), JSI::ReleaseValue);
    if (jsHeaders != nullptr && !JSI::ValueIsUndefined(jsHeaders.get()) && JSI::ValueIsObject(jsHeaders.get())) {
        GetNameValue(jsHeaders.get(), const_cast<std::map<std::string, std::string> &>(req->GetHeader()));
    }

    ParseExtraData(options, req);

    return true;
}

void FetchModule::GetNameValue(JSIValue nv, std::map<std::string, std::string> &map)
{
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> keys(JSI::GetObjectKeys(nv), JSI::ReleaseValue);
    if (keys == nullptr || JSI::ValueIsUndefined(keys.get()) || !JSI::ValueIsArray(keys.get())) {
        HTTP_REQUEST_ERROR("get name value failed");
        return;
    }

    uint32_t length = JSI::GetArrayLength(keys.get());
    for (uint32_t index = 0; index < length; ++index) {
        std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> key(JSI::GetPropertyByIndex(keys.get(), index),
                                                                  JSI::ReleaseValue);
        if (key == nullptr || JSI::ValueIsUndefined(key.get()) || !JSI::ValueIsString(key.get())) {
            continue;
        }

        std::unique_ptr<char, decltype(&FreeString)> keyStr(JSI::ValueToString(key.get()), FreeString);
        if (keyStr == nullptr) {
            HTTP_REQUEST_ERROR("key to string failed");
            continue;
        }

        std::unique_ptr<char, decltype(&FreeString)> valStr(JSI::GetStringProperty(nv, keyStr.get()), FreeString);
        if (valStr == nullptr) {
            HTTP_REQUEST_ERROR("get val failed");
            continue;
        }

        map[keyStr.get()] = valStr.get();
    }
}

void FetchModule::GetRequestBody(JSIValue options, RequestData *requestData)
{
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> body(
        JSI::GetNamedProperty(options, HttpConstant::KEY_HTTP_REQUEST_DATA), JSI::ReleaseValue);
    if (body == nullptr || JSI::ValueIsUndefined(body.get())) {
        HTTP_REQUEST_ERROR("get body failed");
        return;
    }

    if (JSI::ValueIsString(body.get())) {
        size_t size = 0;
        std::unique_ptr<char, decltype(&FreeString)> bodyStr(JSI::ValueToStringWithBufferSize(body.get(), size),
                                                             FreeString);
        if (bodyStr == nullptr) {
            HTTP_REQUEST_ERROR("get body str failed");
            return;
        }
        std::string str(bodyStr.get(), size);
        requestData->SetBody(str);
        HTTP_REQUEST_INFO("body is string, size %zu", str.size());
        return;
    }

    if (JSI::ValueIsObject(body.get())) {
        std::unique_ptr<char, decltype(&FreeString)> jsonString(JSI::JsonStringify(body.get()), FreeString);
        if (jsonString == nullptr) {
            return;
        }
        requestData->SetBody(jsonString.get());
        HTTP_REQUEST_INFO("body is object, stringify size %zu", requestData->GetBody().size());
        requestData->SetHeader(HttpConstant::HTTP_HEADER_KEY_CONTENT_TYPE, HttpConstant::HTTP_CONTENT_TYPE_JSON);
        return;
    }

    HTTP_REQUEST_ERROR("body should be string or object");
}

void FetchModule::ParseExtraData(JSIValue options, RequestData *requestData)
{
    if (MethodForGet(requestData->GetMethod())) {
        std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> extraData(
            JSI::GetNamedProperty(options, HttpConstant::KEY_HTTP_REQUEST_DATA), JSI::ReleaseValue);
        if (extraData == nullptr || JSI::ValueIsUndefined(extraData.get())) {
            return;
        }

        std::string url = requestData->GetUrl();
        std::string param;
        auto index = url.find(HttpConstant::HTTP_URL_PARAM_SEPARATOR);
        if (index != std::string::npos) {
            param = url.substr(index + 1);
            url.resize(index);
        }

        if (JSI::ValueIsString(extraData.get())) {
            std::unique_ptr<char, decltype(&FreeString)> dataStr(JSI::ValueToString(extraData.get()), FreeString);
            if (dataStr == nullptr) {
                return;
            }
            std::string extraParam(dataStr.get());

            requestData->SetUrl(MakeUrl(url, param, extraParam));
            return;
        }

        if (!JSI::ValueIsObject(extraData.get())) {
            return;
        }

        std::map<std::string, std::string> urlPara;
        GetNameValue(extraData.get(), urlPara);
        std::string extraParam;
        for (const auto &p : urlPara) {
            extraParam += p.first + "=" + p.second + HttpConstant::HTTP_URL_PARAM_DELIMITER;
        }
        if (!extraParam.empty()) {
            extraParam.pop_back(); // delete the last &
        }

        requestData->SetUrl(MakeUrl(url, param, extraParam));
        return;
    }

    if (MethodForPost(requestData->GetMethod())) {
        GetRequestBody(options, requestData);
    }
}

std::string FetchModule::GetMethodFromOptions(JSIValue options)
{
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> value(
        JSI::GetNamedProperty(options, HttpConstant::KEY_HTTP_REQUEST_METHOD), JSI::ReleaseValue);
    if (value == nullptr || JSI::ValueIsUndefined(value.get()) || !JSI::ValueIsString(value.get())) {
        return HttpConstant::HTTP_METHOD_GET;
    }

    std::unique_ptr<char, decltype(&FreeString)> methodStr(JSI::ValueToString(value.get()), FreeString);
    return methodStr == nullptr ? HttpConstant::HTTP_METHOD_GET : methodStr.get();
}

JSIValue FetchModule::StringToArrayBuffer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    (void)thisVal;

    if (argsNum < 1 || args[0] == nullptr || JSI::ValueIsUndefined(args[0]) || !JSI::ValueIsString(args[0])) {
        return JSI::CreateUndefined();
    }

    size_t size = 0;
    std::unique_ptr<char, decltype(&FreeString)> str(JSI::ValueToStringWithBufferSize(args[0], size), FreeString);
    if (str == nullptr || size == 0) {
        return JSI::CreateUndefined();
    }

    uint8_t *buffer = nullptr;
    JSIValue arrayBuffer = JSI::CreateArrayBuffer(size, buffer);
    if (buffer == nullptr || arrayBuffer == nullptr) {
        return JSI::CreateUndefined();
    }

    (void)memcpy_s(buffer, size, str.get(), size);
    return arrayBuffer;
}

std::string FetchModule::GetResponseTypeFromOptions(JSIValue options)
{
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> value(
        JSI::GetNamedProperty(options, HttpConstant::KEY_HTTP_REQUEST_RESPONSE_TYPE), JSI::ReleaseValue);
    if (value == nullptr || JSI::ValueIsUndefined(value.get()) || !JSI::ValueIsString(value.get())) {
        return "";
    }

    std::unique_ptr<char, decltype(&FreeString)> responseType(JSI::ValueToString(value.get()), FreeString);
    return responseType == nullptr ? "" : responseType.get();
}

} // namespace ACELite
} // namespace OHOS