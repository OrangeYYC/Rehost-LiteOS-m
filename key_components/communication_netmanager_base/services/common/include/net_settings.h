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

#ifndef NET_SETTINGS_H
#define NET_SETTINGS_H

#include <string>
#include <vector>

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t DEFAULT_FOREGROUND_UID = 123;

class NetSettings {
public:
    NetSettings();
    ~NetSettings();
    static NetSettings &GetInstance();

public:
    bool IsUidForeground(uint32_t uid);
    void SetForegroundUid(uint32_t uid);

    bool IsSystem(uint32_t uid);
    void AddSystemUid(uint32_t uid);
    void RemoveSystemUid(uint32_t uid);

private:
    uint32_t foregroundUid_ = DEFAULT_FOREGROUND_UID;
    std::vector<uint32_t> systemUids_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_SETTINGS_H