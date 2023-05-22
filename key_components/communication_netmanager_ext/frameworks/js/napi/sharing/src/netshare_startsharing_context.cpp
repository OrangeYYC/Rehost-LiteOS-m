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

#include "netshare_startsharing_context.h"

#include "constant.h"
#include "napi_utils.h"
#include "net_manager_constants.h"
#include "netmanager_ext_log.h"

namespace OHOS {
namespace NetManagerStandard {
NetShareStartSharingContext::NetShareStartSharingContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager)
{
}

void NetShareStartSharingContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_EXT_LOGE("CheckParamsType failed");
        SetErrorCode(NETMANAGER_EXT_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    SetParam(NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]));
    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool NetShareStartSharingContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS || paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[0]) == napi_number) {
            return true;
        }
    }
    return false;
}

int32_t NetShareStartSharingContext::GetParam() const
{
    return param_;
}

int32_t NetShareStartSharingContext::GetBytes32() const
{
    return bytes32_;
}

std::vector<std::string> NetShareStartSharingContext::GetIfaces() const
{
    return ifaces_;
}

void NetShareStartSharingContext::SetParam(int32_t param)
{
    param_ = param;
}

void NetShareStartSharingContext::SetIface(std::vector<std::string> ifaces)
{
    ifaces_ = ifaces;
}

void NetShareStartSharingContext::SetBytes32(int32_t bytes32)
{
    bytes32_ = bytes32;
}
} // namespace NetManagerStandard
} // namespace OHOS
