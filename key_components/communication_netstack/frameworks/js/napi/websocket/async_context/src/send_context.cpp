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

#include "send_context.h"

#include "constant.h"
#include "netstack_log.h"
#include "napi_utils.h"
#include "securec.h"

static constexpr size_t MAX_LIMIT = 5 * 1024 * 1024;

namespace OHOS::NetStack {
SendContext::SendContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager), data(nullptr), length(0), protocol(LWS_WRITE_TEXT)
{
}

void SendContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETSTACK_LOGE("SendContext Parse Failed");
        if (paramsCount == FUNCTION_PARAM_ONE) {
            if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_function) {
                SetCallback(params[0]);
            }
            return;
        }

        if (paramsCount == FUNCTION_PARAM_TWO) {
            if (NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function) {
                SetCallback(params[1]);
            }
            return;
        }
        return;
    }

    if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_string) {
        NETSTACK_LOGI("SendContext NapiUtils::GetValueType(GetEnv(), params[0]) == napi_string");
        std::string str = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[0]);
        // must have PRE and POST
        size_t dataLen = LWS_SEND_BUFFER_PRE_PADDING + str.length() + LWS_SEND_BUFFER_POST_PADDING;
        if (dataLen == 0 || dataLen > MAX_LIMIT) {
            NETSTACK_LOGE("SendContext data is exceeded the limit");
            return;
        }
        data = malloc(dataLen);
        if (data == nullptr) {
            NETSTACK_LOGE("no memory");
            return;
        }
        if (memcpy_s(reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(data) + LWS_SEND_BUFFER_PRE_PADDING),
                     str.length(), str.c_str(), str.length()) < 0) {
            NETSTACK_LOGE("copy failed");
            return;
        }
        length = str.length();
        protocol = LWS_WRITE_TEXT;
    } else {
        NETSTACK_LOGI("SendContext data is ArrayBuffer");
        size_t len = 0;
        void *mem = NapiUtils::GetInfoFromArrayBufferValue(GetEnv(), params[0], &len);
        if (mem == nullptr || len == 0) {
            NETSTACK_LOGE("no memory");
            return;
        }
        // must have PRE and POST
        size_t dataLen = LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING;
        if (dataLen == 0 || dataLen > MAX_LIMIT) {
            NETSTACK_LOGE("SendContext data is exceeded the limit");
            return;
        }
        data = malloc(dataLen);
        if (data == nullptr) {
            NETSTACK_LOGE("no memory");
            return;
        }
        if (memcpy_s(reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(data) + LWS_SEND_BUFFER_PRE_PADDING), len,
                     mem, len) < 0) {
            NETSTACK_LOGE("copy failed");
            return;
        }
        length = len;
        protocol = LWS_WRITE_BINARY;
    }

    if (NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function) {
        NETSTACK_LOGI("SendContext NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function");
        return SetParseOK(SetCallback(params[1]) == napi_ok);
    }

    NETSTACK_LOGI("SendContext SetParseOK");
    return SetParseOK(true);
}

bool SendContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == FUNCTION_PARAM_ONE) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_string ||
               NapiUtils::ValueIsArrayBuffer(GetEnv(), params[0]);
    }

    if (paramsCount == FUNCTION_PARAM_TWO) {
        return (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_string ||
                NapiUtils::ValueIsArrayBuffer(GetEnv(), params[0])) &&
               NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function;
    }

    return false;
}

int32_t SendContext::GetErrorCode() const
{
    if (BaseContext::IsPermissionDenied()) {
        return PERMISSION_DENIED_CODE;
    }

    auto err = BaseContext::GetErrorCode();
    if (err == PARSE_ERROR_CODE) {
        return PARSE_ERROR_CODE;
    }
    if (WEBSOCKET_ERR_MAP.find(err) != WEBSOCKET_ERR_MAP.end()) {
        return err;
    }
    return WEBSOCKET_UNKNOWN_OTHER_ERROR;
}

std::string SendContext::GetErrorMessage() const
{
    if (BaseContext::IsPermissionDenied()) {
        return PERMISSION_DENIED_MSG;
    }

    auto err = BaseContext::GetErrorCode();
    if (err == PARSE_ERROR_CODE) {
        return PARSE_ERROR_MSG;
    }
    auto it = WEBSOCKET_ERR_MAP.find(err);
    if (it != WEBSOCKET_ERR_MAP.end()) {
        return it->second;
    }
    it = WEBSOCKET_ERR_MAP.find(WEBSOCKET_UNKNOWN_OTHER_ERROR);
    if (it != WEBSOCKET_ERR_MAP.end()) {
        return it->second;
    }
    return {};
}
} // namespace OHOS::NetStack
