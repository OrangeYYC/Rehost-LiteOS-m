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

#ifndef NETMANAGER_NET_ADJ_SERVICE_STUB_H
#define NETMANAGER_NET_ADJ_SERVICE_STUB_H

#include <map>

#include "iremote_stub.h"

#include "i_net_adj_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetAdjServiceStub : public IRemoteStub<INetAdjService> {
public:
    NetAdjServiceStub();
    ~NetAdjServiceStub() override;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using NetAdjServiceFunc = int32_t (NetAdjServiceStub::*)(MessageParcel &, MessageParcel &);
    int32_t OnSystemReady(MessageParcel &data, MessageParcel &reply);
    int32_t OnAddNetAdjIface(MessageParcel &data, MessageParcel &reply);
    int32_t OnRemoveNetAdjIface(MessageParcel &data, MessageParcel &reply);
    int32_t OnRegisterAdjIfaceCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnregisterAdjIfaceCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateNetAdjInfo(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, NetAdjServiceFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_NET_ADJ_SERVICE_STUB_H
