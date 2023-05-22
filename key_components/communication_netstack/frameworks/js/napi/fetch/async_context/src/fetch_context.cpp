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

#include "fetch_context.h"

#include "constant.h"
#include "fetch_exec.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
FetchContext::FetchContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager), successCallback_(nullptr), failCallback_(nullptr), completeCallback_(nullptr)
{
}

void FetchContext::ParseParams(napi_value *params, size_t paramsCount)
{
    SetNeedPromise(false);

    bool valid = CheckParamsType(params, paramsCount);
    if (!valid) {
        NETSTACK_LOGE("fetch CheckParamsType failed");
        return;
    }

    request.SetUrl(NapiUtils::GetStringPropertyUtf8(GetEnv(), params[0], FetchConstant::PARAM_KEY_URL));
    request.SetMethod(NapiUtils::GetStringPropertyUtf8(GetEnv(), params[0], FetchConstant::PARAM_KEY_METHOD));
    if (request.GetMethod().empty()) {
        request.SetMethod(FetchConstant::HTTP_METHOD_GET);
    }
    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], FetchConstant::PARAM_KEY_RESPONSE_TYPE)) {
        responseType_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), params[0], FetchConstant::PARAM_KEY_RESPONSE_TYPE);
    }
    ParseHeader(params[0]);
    ParseData(params[0]);

    napi_value success = NapiUtils::GetNamedProperty(GetEnv(), params[0], FetchConstant::PARAM_KEY_SUCCESS);
    if (NapiUtils::GetValueType(GetEnv(), success) == napi_function) {
        if (SetSuccessCallback(success) != napi_ok) {
            NETSTACK_LOGE("SetSuccessCallback failed");
            return;
        }
    }
    napi_value fail = NapiUtils::GetNamedProperty(GetEnv(), params[0], FetchConstant::PARAM_KEY_FAIL);
    if (NapiUtils::GetValueType(GetEnv(), fail) == napi_function) {
        if (SetFailCallback(fail) != napi_ok) {
            NETSTACK_LOGE("SetFailCallback failed");
            return;
        }
    }
    napi_value complete = NapiUtils::GetNamedProperty(GetEnv(), params[0], FetchConstant::PARAM_KEY_COMPLETE);
    if (NapiUtils::GetValueType(GetEnv(), complete) == napi_function) {
        if (SetCompleteCallback(complete) != napi_ok) {
            NETSTACK_LOGE("SetCompleteCallback failed");
            return;
        }
    }
    NETSTACK_LOGI("fetch parse OK");
    SetParseOK(true);
}

bool FetchContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == 1) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object;
    }
    return false;
}

void FetchContext::ParseHeader(napi_value optionsValue)
{
    if (!NapiUtils::HasNamedProperty(GetEnv(), optionsValue, FetchConstant::PARAM_KEY_HEADER)) {
        return;
    }
    napi_value header = NapiUtils::GetNamedProperty(GetEnv(), optionsValue, FetchConstant::PARAM_KEY_HEADER);
    if (NapiUtils::GetValueType(GetEnv(), header) != napi_object) {
        return;
    }
    auto names = NapiUtils::GetPropertyNames(GetEnv(), header);
    std::for_each(names.begin(), names.end(), [header, this](const std::string &name) {
        auto value = NapiUtils::GetStringPropertyUtf8(GetEnv(), header, name);
        if (!value.empty()) {
            request.SetHeader(CommonUtils::ToLower(name), value);
        }
    });
}

bool FetchContext::ParseData(napi_value optionsValue)
{
    if (!NapiUtils::HasNamedProperty(GetEnv(), optionsValue, FetchConstant::PARAM_KEY_DATA)) {
        NETSTACK_LOGI("no extraData");
        return true;
    }
    napi_value data = NapiUtils::GetNamedProperty(GetEnv(), optionsValue, FetchConstant::PARAM_KEY_DATA);

    if (FetchExec::MethodForGet(request.GetMethod())) {
        return GetUrlParam(data);
    }
    if (FetchExec::MethodForPost(request.GetMethod())) {
        return GetRequestBody(data);
    }

    NETSTACK_LOGE("ParseData failed, method is not supported %{public}s", request.GetMethod().c_str());
    return false;
}

bool FetchContext::GetRequestBody(napi_value data)
{
    /* if body is empty, return false, or curl will wait for body */

    napi_valuetype type = NapiUtils::GetValueType(GetEnv(), data);
    if (type == napi_string) {
        auto body = NapiUtils::GetStringFromValueUtf8(GetEnv(), data);
        if (body.empty()) {
            NETSTACK_LOGE("body napi_string is empty");
            return false;
        }
        request.SetBody(body.c_str(), body.size());
        return true;
    }
    if (type == napi_object) {
        std::string body = NapiUtils::GetStringFromValueUtf8(GetEnv(), NapiUtils::JsonStringify(GetEnv(), data));
        if (body.empty()) {
            NETSTACK_LOGE("body napi_object is empty");
            return false;
        }
        request.SetBody(body.c_str(), body.size());
        return true;
    }

    NETSTACK_LOGE("Body just support string and object");
    return false;
}

bool FetchContext::GetUrlParam(napi_value data)
{
    std::string url = request.GetUrl();
    if (url.empty()) {
        NETSTACK_LOGE("url is empty!");
        return false;
    }

    std::string param;
    auto index = url.find(FetchConstant::HTTP_URL_PARAM_START);
    if (index != std::string::npos) {
        param = url.substr(index + 1);
        url = url.substr(0, index);
    }

    napi_valuetype type = NapiUtils::GetValueType(GetEnv(), data);
    if (type == napi_string) {
        std::string extraParam = NapiUtils::GetStringFromValueUtf8(GetEnv(), data);

        request.SetUrl(FetchExec::MakeUrl(url, param, extraParam));
        return true;
    }
    if (type != napi_object) {
        return true;
    }

    std::string extraParam;
    auto names = NapiUtils::GetPropertyNames(GetEnv(), data);
    std::for_each(names.begin(), names.end(), [this, data, &extraParam](std::string name) {
        auto value = NapiUtils::GetStringPropertyUtf8(GetEnv(), data, name);
        NETSTACK_LOGI("url param name = %{public}s, value = %{public}s", name.c_str(), value.c_str());
        if (!name.empty() && !value.empty()) {
            bool encodeName = FetchExec::EncodeUrlParam(name);
            bool encodeValue = FetchExec::EncodeUrlParam(value);
            if (encodeName || encodeValue) {
                request.SetHeader(CommonUtils::ToLower(FetchConstant::HTTP_CONTENT_TYPE),
                                  FetchConstant::HTTP_CONTENT_TYPE_URL_ENCODE);
            }
            extraParam +=
                name + FetchConstant::HTTP_URL_NAME_VALUE_SEPARATOR + value + FetchConstant::HTTP_URL_PARAM_SEPARATOR;
        }
    });
    if (!extraParam.empty()) {
        extraParam.pop_back(); // remove the last &
    }

    request.SetUrl(FetchExec::MakeUrl(url, param, extraParam));
    return true;
}

napi_status FetchContext::SetSuccessCallback(napi_value callback)
{
    if (successCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), successCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &successCallback_);
}

napi_status FetchContext::SetFailCallback(napi_value callback)
{
    if (failCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), failCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &failCallback_);
}

napi_status FetchContext::SetCompleteCallback(napi_value callback)
{
    if (completeCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), completeCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &completeCallback_);
}

napi_value FetchContext::GetSuccessCallback() const
{
    if (successCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), successCallback_, &callback));
    return callback;
}

napi_value FetchContext::GetFailCallback() const
{
    if (failCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), failCallback_, &callback));
    return callback;
}

napi_value FetchContext::GetCompleteCallback() const
{
    if (completeCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), completeCallback_, &callback));
    return callback;
}

std::string FetchContext::GetResponseType() const
{
    return responseType_;
}
} // namespace OHOS::NetStack
