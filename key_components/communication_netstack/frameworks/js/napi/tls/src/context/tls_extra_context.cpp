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

#include "tls_extra_context.h"

#include <string_view>

#include "context_key.h"
#include "event_manager.h"
#include "napi_utils.h"
#include "netstack_log.h"

namespace OHOS::NetStack {
static constexpr std::string_view PARSE_ERROR = "first param is not type of TCPExtraOptions";

TLSSetExtraOptionsContext::TLSSetExtraOptionsContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void TLSSetExtraOptionsContext::ParseParams(napi_value *params, size_t paramsCount)
{
    bool valid = CheckParamsType(params, paramsCount);
    if (!valid) {
        return;
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_RECEIVE_BUFFER_SIZE)) {
        options_.SetReceiveBufferSize(NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_RECEIVE_BUFFER_SIZE));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_SEND_BUFFER_SIZE)) {
        options_.SetSendBufferSize(NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_SEND_BUFFER_SIZE));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_REUSE_ADDRESS)) {
        options_.SetReuseAddress(NapiUtils::GetBooleanProperty(GetEnv(), params[0], KEY_REUSE_ADDRESS));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_SOCKET_TIMEOUT)) {
        options_.SetSocketTimeout(NapiUtils::GetUint32Property(GetEnv(), params[0], KEY_SOCKET_TIMEOUT));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_KEEP_ALIVE)) {
        options_.SetKeepAlive(NapiUtils::GetBooleanProperty(GetEnv(), params[0], KEY_KEEP_ALIVE));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_OOB_INLINE)) {
        options_.SetOOBInline(NapiUtils::GetBooleanProperty(GetEnv(), params[0], KEY_OOB_INLINE));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_TCP_NO_DELAY)) {
        options_.SetTCPNoDelay(NapiUtils::GetBooleanProperty(GetEnv(), params[0], KEY_TCP_NO_DELAY));
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[0], KEY_SOCKET_LINGER)) {
        napi_value socketLinger = NapiUtils::GetNamedProperty(GetEnv(), params[0], KEY_SOCKET_LINGER);
        if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object) {
            if (NapiUtils::HasNamedProperty(GetEnv(), socketLinger, KEY_SOCKET_LINGER_ON)) {
                options_.socketLinger.SetOn(
                    NapiUtils::GetBooleanProperty(GetEnv(), socketLinger, KEY_SOCKET_LINGER_ON));
            }
            if (NapiUtils::HasNamedProperty(GetEnv(), socketLinger, KEY_SOCKET_LINGER_LINGER)) {
                options_.socketLinger.SetLinger(
                    NapiUtils::GetUint32Property(GetEnv(), socketLinger, KEY_SOCKET_LINGER_LINGER));
            }
        }
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

int TLSSetExtraOptionsContext::GetSocketFd() const
{
    return (int)(uint64_t)manager_->GetData();
}

bool TLSSetExtraOptionsContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        if (NapiUtils::GetValueType(GetEnv(), params[0]) != napi_object) {
            NETSTACK_LOGE("first param is not TCPExtraOptions");
            SetNeedThrowException(true);
            SetError(PARSE_ERROR_CODE, PARSE_ERROR.data());
            return false;
        }
        return true;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[0]) != napi_object) {
            NETSTACK_LOGE("first param is not TCPExtraOptions");
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
