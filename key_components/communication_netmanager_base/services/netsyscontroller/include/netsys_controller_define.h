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

#ifndef NETSYS_CONTROLLER_DEFINE_H
#define NETSYS_CONTROLLER_DEFINE_H

#include <string>
#include <functional>

namespace OHOS {
namespace NetManagerStandard {
struct NetsysNotifyCallback {
    std::function<void(const std::string &iface)> NetsysResponseInterfaceAdd;
    std::function<void(const std::string &iface)> NetsysResponseInterfaceRemoved;
};

enum NetsysContrlResultCode {
    ERR_NULLPTR = 0,
    ERR_NATIVESERVICE_NOTFIND = (-1),
    ERR_START_DHCPSERVICE_FAILED = (-2),
    ERR_STOP_DHCPSERVICE_FAILED = (-3),
    NETSYS_NETSYSSERVICE_NULL = 2105001,
    NETSYS_ERR_VPN = 2105005,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETSYS_CONTROLLER_DEFINE_H
