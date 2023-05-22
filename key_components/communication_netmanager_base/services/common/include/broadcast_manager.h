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

#ifndef BROADCAST_MANAGER_H
#define BROADCAST_MANAGER_H

#include <string>
#include <map>

#include "singleton.h"

namespace OHOS {
namespace NetManagerStandard {
struct BroadcastInfo {
    std::string     action;
    std::string     data;
    std::string     permission;
    int32_t         code;
    bool            ordered;
    BroadcastInfo() : code(0), ordered(true) {}
};

class BroadcastManager {
    DECLARE_DELAYED_SINGLETON(BroadcastManager)
public:
    bool SendBroadcast(const BroadcastInfo &info, const std::map<std::string, bool> &param);

    bool SendBroadcast(const BroadcastInfo &info, const std::map<std::string, int32_t> &param);

    bool SendBroadcast(const BroadcastInfo &info, const std::map<std::string, int64_t> &param);

    bool SendBroadcast(const BroadcastInfo &info, const std::map<std::string, std::string> &param);

private:
    template <typename T>
    bool SendBroadcastEx(const BroadcastInfo &info, const std::map<std::string, T> &param);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // BROADCAST_MANAGER_H
