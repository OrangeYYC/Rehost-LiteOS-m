/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "net_settings.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t DEFAULT_SYSTEM_UID = 456;
NetSettings::NetSettings()
{
    AddSystemUid(DEFAULT_SYSTEM_UID);
}

NetSettings::~NetSettings() {}

NetSettings &NetSettings::GetInstance()
{
    static NetSettings gNetSettings;
    return gNetSettings;
}

bool NetSettings::IsUidForeground(uint32_t uid)
{
    return foregroundUid_ == uid;
}

void NetSettings::SetForegroundUid(uint32_t uid)
{
    foregroundUid_ = uid;
}

bool NetSettings::IsSystem(uint32_t uid)
{
    return std::find(systemUids_.begin(), systemUids_.end(), uid) != systemUids_.end();
}

void NetSettings::AddSystemUid(uint32_t uid)
{
    systemUids_.push_back(uid);
}

void NetSettings::RemoveSystemUid(uint32_t uid)
{
    if (uid == 0) {
        systemUids_.clear();
        return;
    }

    for (auto it = systemUids_.begin(); it != systemUids_.end(); it++) {
        if (*it == uid) {
            systemUids_.erase(it);
            return;
        }
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
