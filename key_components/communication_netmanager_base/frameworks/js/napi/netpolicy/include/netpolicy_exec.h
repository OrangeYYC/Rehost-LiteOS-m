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

#ifndef COMMUNICATIONNETMANAGERBASE_POLICY_EXEC_H
#define COMMUNICATIONNETMANAGERBASE_POLICY_EXEC_H

#include <napi/native_api.h>

#include "get_policy_by_uid_context.h"
#include "get_uids_by_policy_context.h"
#include "get_net_quota_policies_context.h"
#include "is_uid_net_allowed_context.h"
#include "net_quota_policy.h"
#include "restore_all_policies_context.h"
#include "set_bg_policy_context.h"
#include "set_net_quota_policies_context.h"
#include "set_policy_by_uid_context.h"
#include "set_device_idle_allow_list_context.h"
#include "update_remind_policy_context.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyExec final {
public:
    NetPolicyExec() = delete;
    ~NetPolicyExec() = delete;

    static bool ExecSetPolicyByUid(SetPolicyByUidContext *context);
    static bool ExecGetPolicyByUid(GetPolicyByUidContext *context);
    static bool ExecGetUidsByPolicy(GetUidsByPolicyContext *context);
    static bool ExecSetBackgroundPolicy(SetBackgroundPolicyContext *context);
    static bool ExecGetBackgroundPolicy(GetBackgroundPolicyContext *context);
    static bool ExecGetNetQuotaPolicies(GetNetQuotaPoliciesContext *context);
    static bool ExecSetNetQuotaPolicies(SetNetQuotaPoliciesContext *context);
    static bool ExecRestoreAllPolicies(RestoreAllPoliciesContext *context);
    static bool ExecIsUidNetAllowed(IsUidNetAllowedContext *context);
    static bool ExecSetDeviceIdleAllowList(SetDeviceIdleAllowListContext *context);
    static bool ExecGetDeviceIdleAllowList(GetDeviceIdleAllowListContext *context);
    static bool ExecGetBackgroundPolicyByUid(GetBackgroundPolicyByUidContext *context);
    static bool ExecResetPolicies(ResetPoliciesContext *context);
    static bool ExecUpdateRemindPolicy(UpdateRemindPolicyContext *context);

    static napi_value SetPolicyByUidCallback(SetPolicyByUidContext *context);
    static napi_value GetPolicyByUidCallback(GetPolicyByUidContext *context);
    static napi_value GetUidsByPolicyCallback(GetUidsByPolicyContext *context);
    static napi_value SetBackgroundPolicyCallback(SetBackgroundPolicyContext *context);
    static napi_value GetBackgroundPolicyCallback(GetBackgroundPolicyContext *context);
    static napi_value GetNetQuotaPoliciesCallback(GetNetQuotaPoliciesContext *context);
    static napi_value SetNetQuotaPoliciesCallback(SetNetQuotaPoliciesContext *context);
    static napi_value RestoreAllPoliciesCallback(RestoreAllPoliciesContext *context);
    static napi_value IsUidNetAllowedCallback(IsUidNetAllowedContext *context);
    static napi_value SetDeviceIdleAllowListCallback(SetDeviceIdleAllowListContext *context);
    static napi_value GetDeviceIdleAllowListCallback(GetDeviceIdleAllowListContext *context);
    static napi_value GetBackgroundPolicyByUidCallback(GetBackgroundPolicyByUidContext *context);
    static napi_value ResetPoliciesCallback(ResetPoliciesContext *context);
    static napi_value UpdateRemindPolicyCallback(UpdateRemindPolicyContext *context);

    static napi_value CreateNetQuotaPolicy(napi_env env, const NetQuotaPolicy &netQuotaPolicy);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_POLICY_EXEC_H
