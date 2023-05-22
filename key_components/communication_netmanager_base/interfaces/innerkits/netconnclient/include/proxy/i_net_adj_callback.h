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
#ifndef COMMUNICATION_I_NET_ADJ_CALLBACK_H
#define COMMUNICATION_I_NET_ADJ_CALLBACK_H

#include "iremote_broker.h"

#include "net_adj_info.h"

namespace OHOS {
namespace NetManagerStandard {
class INetAdjCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetAdjCallback");
    ~INetAdjCallback() override = default;

    enum {
        ADJ_ADD,
        ADJ_REMOVE,
    };

    virtual int32_t NetAdjAdd(const sptr<NetAdjInfo> &adjInfo) = 0;
    virtual int32_t NetAdjRemove(const sptr<NetAdjInfo> &adjInfo) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATION_I_NET_ADJ_CALLBACK_H