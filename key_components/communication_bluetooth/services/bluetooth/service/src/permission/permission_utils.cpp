/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "permission_utils.h"
#include "auth_center.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace bluetooth {
using namespace OHOS;

int PermissionUtils::VerifyUseBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyUseBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyDiscoverBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyDiscoverBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyManageBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyManageBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyLocationPermission()
{
    return AuthCenter::GetInstance().VerifyLocationPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyApproximatelyPermission()
{
    return AuthCenter::GetInstance().VerifyApproximatelyPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyUseBluetoothPermission(const std::uint32_t  &tokenID)
{
    return AuthCenter::GetInstance().VerifyUseBluetoothPermission(tokenID);
}

int PermissionUtils::VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID)
{
    return AuthCenter::GetInstance().VerifyDiscoverBluetoothPermission(tokenID);
}

bool PermissionUtils::CheckSystemHapApp()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum callingType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    uint64_t fullTokenId = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApp = TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
    HILOGI("tokenId:0x%{public}x, callingType:0x%{public}x, fullTokenId:0x%{public}llx, isSystemApp:%{public}d",
        tokenId, callingType, static_cast<unsigned long long>(fullTokenId), isSystemApp);
    // Only the system app can invoke the system interface.
    if (callingType == TOKEN_HAP && !isSystemApp) {
        HILOGE("The caller is not a system app.");
        return false;
    }
    return true;
}
}  // namespace bluetooth
}  // namespace OHOS