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

#include "tls_bind_context.h"

#include <string_view>

#include "context_key.h"
#include "event_manager.h"
#include "netstack_log.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
static constexpr std::string_view PARSE_ERROR = "address is not type of NetAddress";

TLSBindContext::TLSBindContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void TLSBindContext::ParseParams(napi_value *params, size_t paramsCount)
{
    bool valid = CheckParamsType(params, paramsCount);
    if (!valid) {
        return;
    }

    std::string addr = NapiUtils::GetStringPropertyUtf8(GetEnv(), params[0], KEY_ADDRESS);
    if (addr.empty()) {
        NETSTACK_LOGE("address is empty");
    }
    address_.SetAddress(addr);
    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_FAMILY)) {
        uint32_t family = NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_FAMILY);
        address_.SetFamilyByJsValue(family);
    }
    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_PORT)) {
        uint16_t port = static_cast<uint16_t>(NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_PORT));
        address_.SetPort(port);
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

int TLSBindContext::GetSocketFd() const
{
    return (int)(uint64_t)manager_->GetData();
}

bool TLSBindContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        if (NapiUtils::GetValueType(GetEnv(), params[0]) != napi_object) {
            NETSTACK_LOGE("first param is not NetAddress");
            SetNeedThrowException(true);
            SetError(PARSE_ERROR_CODE, PARSE_ERROR.data());
            return false;
        }
        return true;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[0]) != napi_object) {
            NETSTACK_LOGE("first param is not NetAddress");
            SetNeedThrowException(true);
            SetError(PARSE_ERROR_CODE, PARSE_ERROR.data());
            return false;
        }
        if (NapiUtils::GetValueType(GetEnv(), params[1]) != napi_function) {
            NETSTACK_LOGE("SendContext second param is not function");
            return false;
        }
        return true;
    }
    return false;
}
} // namespace OHOS::NetStack
