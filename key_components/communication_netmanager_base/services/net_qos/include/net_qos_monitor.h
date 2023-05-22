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

#ifndef COMMUNICATION_NET_QOS_MONITOR_H
#define COMMUNICATION_NET_QOS_MONITOR_H

#include <map>

#include "net_qos_info.h"
#include "net_handle.h"

namespace OHOS {
namespace NetManagerStandard {
class NetQosMonitor {
public:
    NetQosMonitor();
    sptr<NetQosInfo> GetQosInfo(const sptr<NetHandle> &netHandle);

private:
    void StartQosMonitor(const sptr<NetHandle> &netHandle);
    void StopQosMonitor(const sptr<NetHandle> &netHandle);
    std::map<NetHandle, NetQosInfo> netQosInfoMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NET_QOS_MONITOR_H
