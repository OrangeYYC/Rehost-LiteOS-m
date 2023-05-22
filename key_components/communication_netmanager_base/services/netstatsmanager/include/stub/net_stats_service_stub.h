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

#ifndef NET_STATS_SERVICE_STUB_H
#define NET_STATS_SERVICE_STUB_H

#include <map>

#include "i_net_stats_service.h"
#include "iremote_stub.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsServiceStub : public IRemoteStub<INetStatsService> {
public:
    NetStatsServiceStub();
    ~NetStatsServiceStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using NetStatsServiceFunc = int32_t (NetStatsServiceStub::*)(MessageParcel &, MessageParcel &);

    int32_t OnRegisterNetStatsCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnregisterNetStatsCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetIfaceRxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetIfaceTxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCellularRxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetCellularTxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetAllRxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetAllTxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetUidRxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetUidTxBytes(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetIfaceStatsDetail(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetUidStatsDetail(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateIfacesStats(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateStatsData(MessageParcel &data, MessageParcel &reply);
    int32_t OnResetFactory(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NetStatsServiceFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_SERVICE_STUB_H
