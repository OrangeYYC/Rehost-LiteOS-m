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

#include "net_policy_service_common.h"

#include "net_policy_service.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t NetPolicyServiceCommon::ResetPolicies()
{
    return DelayedSingleton<NetPolicyService>::GetInstance()->ResetPolicies("");
}

bool NetPolicyServiceCommon::IsUidNetAllowed(uint32_t uid, bool metered)
{
    bool isAllowed = false;
    DelayedSingleton<NetPolicyService>::GetInstance()->IsUidNetAllowed(uid, metered, isAllowed);
    return isAllowed;
}
} // namespace NetManagerStandard
} // namespace OHOS
