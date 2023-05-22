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

#ifndef NET_POLICY_SERVICE_COMMON_H
#define NET_POLICY_SERVICE_COMMON_H

#include "net_policy_base_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyServiceCommon : public NetPolicyBaseService {
public:
    /**
     * Reset net policies.
     *
     * @return Returns 0 success.
     */
    int32_t ResetPolicies() override;

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     *
     * @param uid The specified UID of application.
     * @param metered Indicates metered network or non-metered network.
     * @return Returns it's allowed or not to access the network.
     */
    bool IsUidNetAllowed(uint32_t uid, bool metered) override;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_SERVICE_COMMON_H