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

#include "close_context.h"

#include "constant.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
CloseContext::CloseContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager), code(CLOSE_REASON_NORMAL_CLOSE)
{
}

void CloseContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETSTACK_LOGE("CloseContext Parse Failed");
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

    if (paramsCount == FUNCTION_PARAM_ZERO) {
        NETSTACK_LOGI("CloseContext Parse OK1");
        return SetParseOK(true);
    }

    if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_function) {
        NETSTACK_LOGI("CloseContext Parse OK2");
        return SetParseOK(SetCallback(params[0]) == napi_ok);
    }

    uint32_t closeCode = NapiUtils::GetUint32Property(GetEnv(), params[0], ContextKey::CODE);
    if (closeCode >= CLOSE_REASON_NORMAL_CLOSE && closeCode <= CLOSE_REASON_RESERVED12) {
        code = closeCode;
    }
    reason = NapiUtils::GetStringPropertyUtf8(GetEnv(), params[0], ContextKey::REASON);

    if (paramsCount == FUNCTION_PARAM_TWO) {
        NETSTACK_LOGI("CloseContext Parse OK3");
        return SetParseOK(SetCallback(params[1]) == napi_ok);
    }
    NETSTACK_LOGI("CloseContext Parse OK4");
    return SetParseOK(true);
}

bool CloseContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == FUNCTION_PARAM_ZERO) {
        return true;
    }

    if (paramsCount == FUNCTION_PARAM_ONE) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object ||
               NapiUtils::GetValueType(GetEnv(), params[0]) == napi_function;
    }

    if (paramsCount == FUNCTION_PARAM_TWO) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function;
    }

    return false;
}

int32_t CloseContext::GetErrorCode() const
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

std::string CloseContext::GetErrorMessage() const
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
