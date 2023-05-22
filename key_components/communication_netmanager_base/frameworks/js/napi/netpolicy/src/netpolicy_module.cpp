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

#include <napi/native_api.h>
#include <napi/native_common.h>

#include "constant.h"
#include "get_net_quota_policies_context.h"
#include "get_policy_by_uid_context.h"
#include "get_uids_by_policy_context.h"
#include "is_uid_net_allowed_context.h"
#include "module_template.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
#include "netpolicy_async_work.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_constants.h"
#include "policy_observer_wrapper.h"
#include "restore_all_policies_context.h"
#include "set_bg_policy_context.h"
#include "set_device_idle_allow_list_context.h"
#include "set_net_quota_policies_context.h"
#include "set_policy_by_uid_context.h"
#include "update_remind_policy_context.h"

#define DEFINE_REMIND(REMIND) \
    DECLARE_NAPI_STATIC_PROPERTY(#REMIND, NapiUtils::CreateUint32(env, static_cast<uint32_t>(RemindType::REMIND)))

#define DEFINE_UID_POLICY(POLICY) \
    DECLARE_NAPI_STATIC_PROPERTY(#POLICY, NapiUtils::CreateInt32(env, static_cast<int32_t>(NetUidPolicy::POLICY)))

#define DEFINE_NET_UID_RULE(RULE) \
    DECLARE_NAPI_STATIC_PROPERTY(#RULE, NapiUtils::CreateInt32(env, static_cast<int32_t>(NetUidRule::RULE)))

#define DEFINE_METERING_MODE(METERING_MODE)      \
    DECLARE_NAPI_STATIC_PROPERTY(#METERING_MODE, \
                                 NapiUtils::CreateInt32(env, static_cast<int32_t>(MeteringMode::METERING_MODE)))

#define DEFINE_BACKGROUND_POLICY(BACKGROUND_POLICY) \
    DECLARE_NAPI_STATIC_PROPERTY(                   \
        #BACKGROUND_POLICY, NapiUtils::CreateInt32(env, static_cast<int32_t>(NetBackgroundPolicy::BACKGROUND_POLICY)))

#define DEFINE_LIMIT_ACTION(ACTION) \
    DECLARE_NAPI_STATIC_PROPERTY(#ACTION, NapiUtils::CreateInt32(env, static_cast<int32_t>(LimitAction::ACTION)))

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *NETPOLICY_MODULE_NAME = "net.policy";

constexpr const char *FUNCTION_SET_POLICY_BY_UID = "setPolicyByUid";
constexpr const char *FUNCTION_GET_POLICY_BY_UID = "getPolicyByUid";
constexpr const char *FUNCTION_GET_UID_BY_POLICY = "getUidsByPolicy";
constexpr const char *FUNCTION_SET_NET_QUOTA_POLICIES = "setNetQuotaPolicies";
constexpr const char *FUNCTION_GET_NET_QUOTA_POLICIES = "getNetQuotaPolicies";
constexpr const char *FUNCTION_RESTORE_ALL_POLICIES = "restoreAllPolicies";
constexpr const char *FUNCTION_SET_BACKGROUND_POLICY = "setBackgroundAllowed";
constexpr const char *FUNCTION_GET_BACKGROUND_POLICY = "isBackgroundAllowed";
constexpr const char *FUNCTION_IS_UID_NET_ACCESS = "isUidNetAllowed";
constexpr const char *FUNCTION_SET_DEVICE_IDLE_ALLOWLIST = "setDeviceIdleAllowList";
constexpr const char *FUNCTION_GET_DEVICE_IDLE_ALLOWLIST = "getDeviceIdleAllowList";
constexpr const char *FUNCTION_GET_BACKGROUND_POLICY_BY_UID = "getBackgroundPolicyByUid";
constexpr const char *FUNCTION_RESET_POLICIES = "resetPolicies";
constexpr const char *FUNCTION_UPDATE_REMIND_POLICY = "updateRemindPolicy";
constexpr const char *FUNCTION_ON = "on";
constexpr const char *FUNCTION_OFF = "off";
constexpr const char *REMIND_TYPE = "RemindType";
constexpr const char *METERING_MODE = "MeteringMode";
constexpr const char *NET_UID_RULE = "NetUidRule";
constexpr const char *NET_UID_POLICY = "NetUidPolicy";
constexpr const char *NET_LIMIT_ACTION = "LimitAction";
constexpr const char *NET_BACKGROUND_POLICY = "NetBackgroundPolicy";

enum MeteringMode {
    /* non metering */
    UN_METERED = 0,
    /* metering */
    METERED,
};

enum ApplicationType {
    /* common application */
    COMMON = 0,
    /* system application */
    SYSTEM,
};
} // namespace

napi_value SetPolicyByUid(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<SetPolicyByUidContext>(env, info, FUNCTION_SET_POLICY_BY_UID, nullptr,
                                                            NetPolicyAsyncWork::ExecSetPolicyByUid,
                                                            NetPolicyAsyncWork::SetPolicyByUidCallback);
}

napi_value GetPolicyByUid(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetPolicyByUidContext>(env, info, FUNCTION_GET_POLICY_BY_UID, nullptr,
                                                            NetPolicyAsyncWork::ExecGetPolicyByUid,
                                                            NetPolicyAsyncWork::GetPolicyByUidCallback);
}

napi_value GetUidsByPolicy(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetUidsByPolicyContext>(env, info, FUNCTION_GET_UID_BY_POLICY, nullptr,
                                                             NetPolicyAsyncWork::ExecGetUidsByPolicy,
                                                             NetPolicyAsyncWork::GetUidsByPolicyCallback);
}

napi_value SetNetQuotaPolicies(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<SetNetQuotaPoliciesContext>(env, info, FUNCTION_SET_NET_QUOTA_POLICIES, nullptr,
                                                                 NetPolicyAsyncWork::ExecSetNetQuotaPolicies,
                                                                 NetPolicyAsyncWork::SetNetQuotaPoliciesCallback);
}

napi_value GetNetQuotaPolicies(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetNetQuotaPoliciesContext>(env, info, FUNCTION_GET_NET_QUOTA_POLICIES, nullptr,
                                                                 NetPolicyAsyncWork::ExecGetNetQuotaPolicies,
                                                                 NetPolicyAsyncWork::GetNetQuotaPoliciesCallback);
}

napi_value RestoreAllPolicies(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<RestoreAllPoliciesContext>(env, info, FUNCTION_RESTORE_ALL_POLICIES, nullptr,
                                                                NetPolicyAsyncWork::ExecRestoreAllPolicies,
                                                                NetPolicyAsyncWork::RestoreAllPoliciesCallback);
}

napi_value SetBackgroundPolicy(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<SetBackgroundPolicyContext>(env, info, FUNCTION_SET_BACKGROUND_POLICY, nullptr,
                                                                 NetPolicyAsyncWork::ExecSetBackgroundPolicy,
                                                                 NetPolicyAsyncWork::SetBackgroundPolicyCallback);
}

napi_value GetBackgroundPolicy(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetBackgroundPolicyContext>(env, info, FUNCTION_GET_BACKGROUND_POLICY, nullptr,
                                                                 NetPolicyAsyncWork::ExecGetBackgroundPolicy,
                                                                 NetPolicyAsyncWork::GetBackgroundPolicyCallback);
}

napi_value IsUidNetAllowed(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<IsUidNetAllowedContext>(env, info, FUNCTION_IS_UID_NET_ACCESS, nullptr,
                                                             NetPolicyAsyncWork::ExecIsUidNetAllowed,
                                                             NetPolicyAsyncWork::IsUidNetAllowedCallback);
}

napi_value SetDeviceIdleAllowList(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<SetDeviceIdleAllowListContext>(
        env, info, FUNCTION_SET_DEVICE_IDLE_ALLOWLIST, nullptr, NetPolicyAsyncWork::ExecSetDeviceIdleAllowList,
        NetPolicyAsyncWork::SetDeviceIdleAllowListCallback);
}

napi_value GetDeviceIdleAllowList(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetDeviceIdleAllowListContext>(
        env, info, FUNCTION_GET_DEVICE_IDLE_ALLOWLIST, nullptr, NetPolicyAsyncWork::ExecGetDeviceIdleAllowList,
        NetPolicyAsyncWork::GetDeviceIdleAllowListCallback);
}

napi_value GetBackgroundPolicyByUid(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetBackgroundPolicyByUidContext>(
        env, info, FUNCTION_GET_BACKGROUND_POLICY_BY_UID, nullptr, NetPolicyAsyncWork::ExecGetBackgroundPolicyByUid,
        NetPolicyAsyncWork::GetBackgroundPolicyByUidCallback);
}

napi_value ResetPolicies(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<ResetPoliciesContext>(env, info, FUNCTION_RESET_POLICIES, nullptr,
                                                           NetPolicyAsyncWork::ExecResetPolicies,
                                                           NetPolicyAsyncWork::ResetPoliciesCallback);
}

napi_value UpdateRemindPolicy(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<UpdateRemindPolicyContext>(env, info, FUNCTION_UPDATE_REMIND_POLICY, nullptr,
                                                                NetPolicyAsyncWork::ExecUpdateRemindPolicy,
                                                                NetPolicyAsyncWork::UpdateRemindPolicyCallback);
}

napi_value On(napi_env env, napi_callback_info info)
{
    return DelayedSingleton<PolicyObserverWrapper>::GetInstance()->On(
        env, info,
        {EVENT_POLICY_UID_POLICY, EVENT_POLICY_UID_RULE, EVENT_POLICY_METERED_IFACES, EVENT_POLICY_QUOTA_POLICY,
         EVENT_POLICY_BACKGROUND_POLICY, EVENT_POLICY_STRATEGY_SWITCH},
        false);
}

napi_value Off(napi_env env, napi_callback_info info)
{
    return DelayedSingleton<PolicyObserverWrapper>::GetInstance()->Off(
        env, info,
        {EVENT_POLICY_UID_POLICY, EVENT_POLICY_UID_RULE, EVENT_POLICY_METERED_IFACES, EVENT_POLICY_QUOTA_POLICY,
         EVENT_POLICY_BACKGROUND_POLICY, EVENT_POLICY_STRATEGY_SWITCH},
        false);
}

static void CreateBackgroundPolicy(napi_env env, napi_value exports)
{
    napi_value result = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, result,
                                {
                                    DEFINE_BACKGROUND_POLICY(NET_BACKGROUND_POLICY_NONE),
                                    DEFINE_BACKGROUND_POLICY(NET_BACKGROUND_POLICY_ENABLE),
                                    DEFINE_BACKGROUND_POLICY(NET_BACKGROUND_POLICY_DISABLE),
                                    DEFINE_BACKGROUND_POLICY(NET_BACKGROUND_POLICY_ALLOWEDLIST),
                                });
    NapiUtils::SetNamedProperty(env, exports, NET_BACKGROUND_POLICY, result);
}

static void CreateMeteringMode(napi_env env, napi_value exports)
{
    napi_value result = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, result,
                                {
                                    DEFINE_METERING_MODE(UN_METERED),
                                    DEFINE_METERING_MODE(METERED),
                                });
    NapiUtils::SetNamedProperty(env, exports, METERING_MODE, result);
}

static void CreateRemindType(napi_env env, napi_value exports)
{
    napi_value type = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, type,
                                {
                                    DEFINE_REMIND(REMIND_TYPE_WARNING),
                                    DEFINE_REMIND(REMIND_TYPE_LIMIT),
                                });
    NapiUtils::SetNamedProperty(env, exports, REMIND_TYPE, type);
}

static void CreateNetUidRule(napi_env env, napi_value exports)
{
    napi_value result = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, result,
                                {
                                    DEFINE_NET_UID_RULE(NET_RULE_NONE),
                                    DEFINE_NET_UID_RULE(NET_RULE_ALLOW_METERED_FOREGROUND),
                                    DEFINE_NET_UID_RULE(NET_RULE_ALLOW_METERED),
                                    DEFINE_NET_UID_RULE(NET_RULE_REJECT_METERED),
                                    DEFINE_NET_UID_RULE(NET_RULE_ALLOW_ALL),
                                    DEFINE_NET_UID_RULE(NET_RULE_REJECT_ALL),
                                });
    NapiUtils::SetNamedProperty(env, exports, NET_UID_RULE, result);
}

static void CreateNetUidPolicy(napi_env env, napi_value exports)
{
    napi_value result = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, result,
                                {
                                    DEFINE_UID_POLICY(NET_POLICY_NONE),
                                    DEFINE_UID_POLICY(NET_POLICY_ALLOW_METERED_BACKGROUND),
                                    DEFINE_UID_POLICY(NET_POLICY_REJECT_METERED_BACKGROUND),
                                });
    NapiUtils::SetNamedProperty(env, exports, NET_UID_POLICY, result);
}

static void CreateLimitAction(napi_env env, napi_value exports)
{
    napi_value result = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, result,
                                {
                                    DEFINE_LIMIT_ACTION(LIMIT_ACTION_NONE),
                                    DEFINE_LIMIT_ACTION(LIMIT_ACTION_DISABLE),
                                    DEFINE_LIMIT_ACTION(LIMIT_ACTION_AUTO_BILL),
                                });
    NapiUtils::SetNamedProperty(env, exports, NET_LIMIT_ACTION, result);
}

napi_value InitPolicyModule(napi_env env, napi_value exports)
{
    NapiUtils::DefineProperties(
        env, exports,
        {
            DECLARE_NAPI_FUNCTION(FUNCTION_SET_POLICY_BY_UID, SetPolicyByUid),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_POLICY_BY_UID, GetPolicyByUid),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_UID_BY_POLICY, GetUidsByPolicy),
            DECLARE_NAPI_FUNCTION(FUNCTION_SET_NET_QUOTA_POLICIES, SetNetQuotaPolicies),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_NET_QUOTA_POLICIES, GetNetQuotaPolicies),
            DECLARE_NAPI_FUNCTION(FUNCTION_RESTORE_ALL_POLICIES, RestoreAllPolicies),
            DECLARE_NAPI_FUNCTION(FUNCTION_SET_BACKGROUND_POLICY, SetBackgroundPolicy),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_BACKGROUND_POLICY, GetBackgroundPolicy),
            DECLARE_NAPI_FUNCTION(FUNCTION_IS_UID_NET_ACCESS, IsUidNetAllowed),
            DECLARE_NAPI_FUNCTION(FUNCTION_SET_DEVICE_IDLE_ALLOWLIST, SetDeviceIdleAllowList),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_DEVICE_IDLE_ALLOWLIST, GetDeviceIdleAllowList),
            DECLARE_NAPI_FUNCTION(FUNCTION_GET_BACKGROUND_POLICY_BY_UID, GetBackgroundPolicyByUid),
            DECLARE_NAPI_FUNCTION(FUNCTION_RESET_POLICIES, ResetPolicies),
            DECLARE_NAPI_FUNCTION(FUNCTION_UPDATE_REMIND_POLICY, UpdateRemindPolicy),
            DECLARE_NAPI_FUNCTION(FUNCTION_RESET_POLICIES, ResetPolicies),
            DECLARE_NAPI_FUNCTION(FUNCTION_UPDATE_REMIND_POLICY, UpdateRemindPolicy),
            DECLARE_NAPI_FUNCTION(FUNCTION_ON, On),
            DECLARE_NAPI_FUNCTION(FUNCTION_OFF, Off),
        });
    CreateRemindType(env, exports);
    CreateLimitAction(env, exports);
    CreateNetUidPolicy(env, exports);
    CreateBackgroundPolicy(env, exports);
    CreateMeteringMode(env, exports);
    CreateNetUidRule(env, exports);
    return exports;
}

static napi_module g_policyModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitPolicyModule,
    .nm_modname = NETPOLICY_MODULE_NAME,
    .nm_priv = nullptr,
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterNetPolicyModule(void)
{
    napi_module_register(&g_policyModule);
}
} // namespace NetManagerStandard
} // namespace OHOS
