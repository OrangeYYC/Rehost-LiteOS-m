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

#ifndef I_NET_STATS_CALLBACK_H
#define I_NET_STATS_CALLBACK_H

#include <string>

#include "iremote_broker.h"

namespace OHOS {
namespace NetManagerStandard {
class INetStatsCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetStatsCallback");
    virtual ~INetStatsCallback() = default;
    enum {
        NET_STATS_IFACE_CHANGED = 0,
        NET_STATS_UID_CHANGED = 1,
    };

    virtual int32_t NetIfaceStatsChanged(const std::string &iface) = 0;
    virtual int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_STATS_CALLBACK_H
