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

#include "get_iface_uid_stats_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_manager_constants.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *IFACE_INFO = "ifaceInfo";
constexpr const char *UID = "uid";
constexpr const char *IFACE = "iface";
constexpr const char *START_TIME = "startTime";
constexpr const char *END_TIME = "endTime";
} // namespace

GetIfaceUidStatsContext::GetIfaceUidStatsContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void GetIfaceUidStatsContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    napi_value ifaceInfo = NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0], IFACE_INFO);
    bool hasUid = NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_0], UID);
    bool hasIface = NapiUtils::HasNamedProperty(GetEnv(), ifaceInfo, IFACE);
    bool hasStart = NapiUtils::HasNamedProperty(GetEnv(), ifaceInfo, START_TIME);
    bool hasEnd = NapiUtils::HasNamedProperty(GetEnv(), ifaceInfo, END_TIME);
    if (!(hasUid && hasIface && hasStart && hasEnd)) {
        NETMANAGER_BASE_LOGE(
            "param error hasIface is %{public}d, hasStart is %{public}d, hasEnd is %{public}d"
            "hasUid is %{public}d",
            hasIface, hasStart, hasEnd, hasUid);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    bool checkUidType = NapiUtils::GetValueType(
                            GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0], UID)) == napi_number;
    bool checkIfaceType =
        NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), ifaceInfo, IFACE)) == napi_string;
    bool checkStartType =
        NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), ifaceInfo, START_TIME)) == napi_number;
    bool checkEndType =
        NapiUtils::GetValueType(GetEnv(), NapiUtils::GetNamedProperty(GetEnv(), ifaceInfo, END_TIME)) == napi_number;

    if (!(checkUidType && checkIfaceType && checkStartType && checkEndType)) {
        NETMANAGER_BASE_LOGE("param napi_type error");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    interfaceName_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), ifaceInfo, IFACE);
    start_ = NapiUtils::GetUint32Property(GetEnv(), ifaceInfo, START_TIME);
    end_ = NapiUtils::GetUint32Property(GetEnv(), ifaceInfo, END_TIME);
    uid_ = NapiUtils::GetInt32Property(GetEnv(), params[ARG_INDEX_0], UID);

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool GetIfaceUidStatsContext::CheckParamsType(napi_value *params, size_t paramsCount)
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

void GetIfaceUidStatsContext::SetUid(int32_t uid)
{
    uid_ = uid;
}

void GetIfaceUidStatsContext::SetInterfaceName(std::string interfaceName)
{
    interfaceName_ = interfaceName;
}

void GetIfaceUidStatsContext::SetStatsInfo(NetStatsInfo statsInfo)
{
    statsInfo_ = statsInfo;
}

void GetIfaceUidStatsContext::SetStart(uint32_t start)
{
    start_ = start;
}

void GetIfaceUidStatsContext::SetEnd(uint32_t end)
{
    end_ = end;
}

int32_t GetIfaceUidStatsContext::GetUid() const
{
    return uid_;
}
std::string GetIfaceUidStatsContext::GetInterfaceName() const
{
    return interfaceName_;
}

NetStatsInfo &GetIfaceUidStatsContext::GetStatsInfo()
{
    return statsInfo_;
}

uint32_t GetIfaceUidStatsContext::GetStart() const
{
    return start_;
}

uint32_t GetIfaceUidStatsContext::GetEnd() const
{
    return end_;
}
} // namespace NetManagerStandard
} // namespace OHOS
