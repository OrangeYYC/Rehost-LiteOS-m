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

#include "get_iface_stats_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_manager_constants.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *IFACE = "iface";
constexpr const char *START_TIME = "startTime";
constexpr const char *END_TIME = "endTime";
} // namespace

GetIfaceStatsContext::GetIfaceStatsContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void GetIfaceStatsContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    bool hasIface = NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_0], IFACE);
    bool hasStart = NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_0], START_TIME);
    bool hasEnd = NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_0], END_TIME);
    if (!(hasIface && hasStart && hasEnd)) {
        NETMANAGER_BASE_LOGE("param error hasIface is %{public}d, hasStart is %{public}d, hasEnd is %{public}d",
                             hasIface, hasStart, hasEnd);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    bool checkIfaceType = NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0],
                                                                                        IFACE)) == napi_string;
    bool checkStartType = NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0],
                                                                                        START_TIME)) == napi_number;
    bool checkEndType = NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0],
                                                                                      END_TIME)) == napi_number;
    if (!(checkIfaceType && checkStartType && checkEndType)) {
        NETMANAGER_BASE_LOGE("param napi_type error");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    interfaceName_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), params[ARG_INDEX_0], IFACE);
    start_ = NapiUtils::GetUint32Property(GetEnv(), params[ARG_INDEX_0], START_TIME);
    end_ = NapiUtils::GetUint32Property(GetEnv(), params[ARG_INDEX_0], END_TIME);

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool GetIfaceStatsContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object;
    }
    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object;
    }
    return false;
}

void GetIfaceStatsContext::SetInterfaceName(std::string interfaceName)
{
    interfaceName_ = interfaceName;
}

void GetIfaceStatsContext::SetStatsInfo(NetStatsInfo statsInfo)
{
    statsInfo_ = statsInfo;
}

void GetIfaceStatsContext::SetStart(uint32_t start)
{
    start_ = start;
}

void GetIfaceStatsContext::SetEnd(uint32_t end)
{
    end_ = end;
}

std::string GetIfaceStatsContext::GetInterfaceName() const
{
    return interfaceName_;
}

NetStatsInfo &GetIfaceStatsContext::GetStatsInfo()
{
    return statsInfo_;
}

uint32_t GetIfaceStatsContext::GetStart() const
{
    return start_;
}

uint32_t GetIfaceStatsContext::GetEnd() const
{
    return end_;
}
} // namespace NetManagerStandard
} // namespace OHOS
