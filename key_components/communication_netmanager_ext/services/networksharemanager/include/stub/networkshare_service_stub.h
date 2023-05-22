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

#ifndef NETWORKSHARE_SERVICE_STUB_H
#define NETWORKSHARE_SERVICE_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_networkshare_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareServiceStub : public IRemoteStub<INetworkShareService> {
    using NetworkShareServiceFunc = int32_t (NetworkShareServiceStub::*)(MessageParcel &, MessageParcel &);

public:
    NetworkShareServiceStub();
    ~NetworkShareServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t ReplyIsNetworkSharingSupported(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyIsSharing(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyStartNetworkSharing(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyStopNetworkSharing(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetSharableRegexs(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetSharingState(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetNetSharingIfaces(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyRegisterSharingEvent(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyUnregisterSharingEvent(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetStatsRxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetStatsTxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t ReplyGetStatsTotalBytes(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NetworkShareServiceFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_SERVICE_STUB_H
