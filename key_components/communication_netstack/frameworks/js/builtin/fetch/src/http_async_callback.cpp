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

#include <algorithm>
#include <memory>

#include "http_constant.h"
#include "http_request.h"
#include "http_request_utils.h"

#include "http_async_callback.h"

namespace OHOS {
namespace ACELite {

HttpAsyncCallback::HttpAsyncCallback(JSIValue thisValue) : thisVal(thisValue) {}

void HttpAsyncCallback::AsyncExecHttpRequest(void *data)
{
    std::unique_ptr<HttpAsyncCallback, decltype(&HttpAsyncCallback::AsyncCallbackDeleter)> asyncCallback(
        static_cast<HttpAsyncCallback *>(data), HttpAsyncCallback::AsyncCallbackDeleter);
    if (asyncCallback == nullptr) {
        return;
    }

    ResponseData responseData;
    bool success = HttpRequest::Request(&asyncCallback->requestData, &responseData);
    if (success) {
        HTTP_REQUEST_INFO("http status line: %s", responseData.GetStatusLine().c_str());
        asyncCallback->OnSuccess(responseData);
    } else {
        asyncCallback->OnFail(responseData.GetErrString().c_str(), responseData.GetCode());
    }
    asyncCallback->OnComplete();
}

void HttpAsyncCallback::AsyncCallbackDeleter(HttpAsyncCallback *asyncCallback)
{
    if (asyncCallback->responseCallback[CB_SUCCESS]) {
        JSI::ReleaseValue(asyncCallback->responseCallback[CB_SUCCESS]);
    }
    if (asyncCallback->responseCallback[CB_FAIL]) {
        JSI::ReleaseValue(asyncCallback->responseCallback[CB_FAIL]);
    }
    if (asyncCallback->responseCallback[CB_COMPLETE]) {
        JSI::ReleaseValue(asyncCallback->responseCallback[CB_COMPLETE]);
    }
    delete asyncCallback;
}

JSIValue HttpAsyncCallback::ResponseDataToJsValue(const ResponseData &responseData)
{
    JSIValue object = JSI::CreateObject();
    if (object == nullptr) {
        return nullptr;
    }

    JSI::SetNumberProperty(object, HttpConstant::KEY_HTTP_RESPONSE_CODE, responseData.GetCode());

    HTTP_REQUEST_INFO("response body size = %zu", responseData.GetData().size());
    std::string responseType = requestData.GetResponseType();
    std::transform(responseType.begin(), responseType.end(), responseType.begin(), tolower);

    if (responseType == HttpConstant::HTTP_RESPONSE_TYPE_JSON) {
        std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> jsonObj(JSI::JsonParse(responseData.GetData().c_str()),
                                                                      JSI::ReleaseValue);
        if (jsonObj != nullptr && !JSI::ValueIsUndefined(jsonObj.get()) && JSI::ValueIsObject(jsonObj.get())) {
            JSI::SetNamedProperty(object, HttpConstant::KEY_HTTP_RESPONSE_DATA, jsonObj.get());
        }
    } else {
        JSI::SetStringPropertyWithBufferSize(object, HttpConstant::KEY_HTTP_RESPONSE_DATA,
                                             responseData.GetData().c_str(), responseData.GetData().size());
    }

    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> headers(JSI::CreateObject(), JSI::ReleaseValue);
    if (headers == nullptr) {
        JSI::ReleaseValue(object);
        return JSI::CreateUndefined();
    }
    for (const auto &p : responseData.GetHeaders()) {
        JSI::SetStringProperty(headers.get(), p.first.c_str(), p.second.c_str());
    }
    JSI::SetNamedProperty(object, HttpConstant::KEY_HTTP_RESPONSE_HEADERS, headers.get());

    return object;
}

void HttpAsyncCallback::OnSuccess(const ResponseData &responseData)
{
    JSIValue success = responseCallback[CB_SUCCESS];
    if (success == nullptr || JSI::ValueIsUndefined(success) || !JSI::ValueIsFunction(success)) {
        return;
    }

    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> obj(ResponseDataToJsValue(responseData), JSI::ReleaseValue);
    if (obj == nullptr || JSI::ValueIsUndefined(obj.get()) || !JSI::ValueIsObject(obj.get())) {
        return;
    }

    JSIValue arg[ARGC_ONE] = {obj.get()};
    JSI::CallFunction(success, thisVal, arg, ARGC_ONE);
}

void HttpAsyncCallback::OnFail(const char *errData, int32_t errCode)
{
    JSIValue fail = responseCallback[CB_FAIL];
    if (fail == nullptr || JSI::ValueIsUndefined(fail) || !JSI::ValueIsFunction(fail)) {
        return;
    }
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> errInfo(JSI::CreateString(errData), JSI::ReleaseValue);
    std::unique_ptr<JSIVal, decltype(&JSI::ReleaseValue)> retCode(JSI::CreateNumber(errCode), JSI::ReleaseValue);

    JSIValue argv[ARGC_TWO] = {errInfo.get(), retCode.get()};
    JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
}

void HttpAsyncCallback::OnComplete()
{
    JSIValue complete = responseCallback[CB_COMPLETE];
    if (complete == nullptr || JSI::ValueIsUndefined(complete) || !JSI::ValueIsFunction(complete)) {
        return;
    }
    JSI::CallFunction(complete, thisVal, nullptr, 0);
}

} // namespace ACELite
} // namespace OHOS
