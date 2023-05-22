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

#include "net_policy_callback.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_constants.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyCallback::NetPolicyCallback() = default;

NetPolicyCallback::~NetPolicyCallback() = default;

int32_t NetPolicyCallback::RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    std::unique_lock<std::mutex> lock(callbackMutex_);
    if (callback == nullptr || callback->AsObject() == nullptr || callback->AsObject().GetRefPtr() == nullptr) {
        NETMGR_LOG_E("The parameter callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    uint32_t callbackCounts = callbacks_.size();
    NETMGR_LOG_D("callback counts [%{public}u]", callbackCounts);
    if (callbackCounts >= LIMIT_CALLBACK_NUM) {
        NETMGR_LOG_E("callback counts cannot more than [%{public}u]", LIMIT_CALLBACK_NUM);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    for (uint32_t i = 0; i < callbackCounts; i++) {
        if (callback->AsObject().GetRefPtr() == callbacks_[i]->AsObject().GetRefPtr()) {
            NETMGR_LOG_W("netPolicyCallback_ had this callback");
            return NETMANAGER_ERR_PARAMETER_ERROR;
        }
    }

    callbacks_.emplace_back(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    std::unique_lock<std::mutex> lock(callbackMutex_);
    if (callback == nullptr || callback->AsObject() == nullptr || callback->AsObject().GetRefPtr() == nullptr) {
        NETMGR_LOG_E("The parameter of callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    auto it = std::remove_if(callbacks_.begin(), callbacks_.end(),
                             [callback](const sptr<INetPolicyCallback> &tempCallback) -> bool {
                                 if (tempCallback == nullptr || tempCallback->AsObject() == nullptr ||
                                     tempCallback->AsObject().GetRefPtr() == nullptr) {
                                     return true;
                                 }
                                 return callback->AsObject().GetRefPtr() == tempCallback->AsObject().GetRefPtr();
                             });
    callbacks_.erase(it, callbacks_.end());

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::NotifyNetUidPolicyChange(uint32_t uid, uint32_t policy)
{
    NETMGR_LOG_D("NotifyNetUidPolicyChange uid[%{public}u] policy[%{public}u]", uid, policy);

    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->NetUidPolicyChange(uid, policy);
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::NotifyNetUidRuleChange(uint32_t uid, uint32_t rule)
{
    NETMGR_LOG_D("NotifyNetUidRuleChange uid[%{public}u] rule[%{public}u]", uid, rule);

    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->NetUidRuleChange(uid, rule);
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::NotifyNetBackgroundPolicyChange(bool isAllowed)
{
    NETMGR_LOG_D("NotifyNetBackgroundPolicyChange  isAllowed[%{public}d]", isAllowed);

    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->NetBackgroundPolicyChange(isAllowed);
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::NotifyNetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("NotifyNetQuotaPolicyChange quotaPolicies empty");
        return POLICY_ERR_QUOTA_POLICY_NOT_EXIST;
    }
    NETMGR_LOG_D("NotifyNetQuotaPolicyChange quotaPolicies.size[%{public}zu]", quotaPolicies.size());

    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->NetQuotaPolicyChange(quotaPolicies);
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallback::NotifyNetMeteredIfacesChange(std::vector<std::string> &ifaces)
{
    NETMGR_LOG_D("NotifyNetMeteredIfacesChange iface size[%{public}zu]", ifaces.size());

    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->NetMeteredIfacesChange(ifaces);
        }
    }

    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
