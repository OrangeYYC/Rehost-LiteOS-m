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

#include "connect_context.h"

#include "context_key.h"
#include "socket_constant.h"
#include "net_address.h"
#include "event_manager.h"
#include "netstack_log.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
ConnectContext::ConnectContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void ConnectContext::ParseParams(napi_value *params, size_t paramsCount)
{
    bool valid = CheckParamsType(params, paramsCount);
    if (!valid) {
        if (paramsCount == PARAM_JUST_CALLBACK) {
            if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_function) {
                SetCallback(params[0]);
            }
            return;
        }
        if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
            if (NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function) {
                SetCallback(params[1]);
            }
            return;
        }
        return;
    }

    napi_value netAddress = NapiUtils::GetNamedProperty(GetEnv(), params[0], KEY_ADDRESS);

    std::string addr = NapiUtils::GetStringPropertyUtf8(GetEnv(), netAddress, KEY_ADDRESS);
    if (addr.empty()) {
        NETSTACK_LOGE("address is empty");
    }
    options.address.SetAddress(addr);
    if (NapiUtils::HasNamedProperty(GetEnv(), netAddress, KEY_FAMILY)) {
        uint32_t family = NapiUtils::GetUint32Property(GetEnv(), netAddress, KEY_FAMILY);
        options.address.SetFamilyByJsValue(family);
    }
    if (NapiUtils::HasNamedProperty(GetEnv(), netAddress, KEY_PORT)) {
        uint16_t port = static_cast<uint16_t>(NapiUtils::GetUint32Property(GetEnv(), netAddress, KEY_PORT));
        options.address.SetPort(port);
    }

    uint32_t timeout = NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_TIMEOUT);
    if (timeout != 0) {
        options.SetTimeout(timeout);
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

int ConnectContext::GetSocketFd() const
{
    return (int)(uint64_t)manager_->GetData();
}

bool ConnectContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[0], KEY_ADDRESS)) ==
                   napi_object;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[0], KEY_ADDRESS)) ==
                   napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function;
    }
    return false;
}

int32_t ConnectContext::GetErrorCode() const
{
    if (BaseContext::IsPermissionDenied()) {
        return PERMISSION_DENIED_CODE;
    }

    auto err = BaseContext::GetErrorCode();
    if (err == PARSE_ERROR_CODE) {
        return PARSE_ERROR_CODE;
    }
    return err + SOCKET_ERROR_CODE_BASE;
}

std::string ConnectContext::GetErrorMessage() const
{
    if (BaseContext::IsPermissionDenied()) {
        return PERMISSION_DENIED_MSG;
    }

    auto errCode = BaseContext::GetErrorCode();
    if (errCode == PARSE_ERROR_CODE) {
        return PARSE_ERROR_MSG;
    }
    char err[MAX_ERR_NUM] = {0};
    (void)strerror_r(errCode, err, MAX_ERR_NUM);
    return err;
}
} // namespace OHOS::NetStack
