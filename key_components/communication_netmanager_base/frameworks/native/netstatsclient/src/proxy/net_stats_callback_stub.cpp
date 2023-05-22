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

#include "net_stats_callback_stub.h"

#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsCallbackStub::NetStatsCallbackStub()
{
    memberFuncMap_[NET_STATS_IFACE_CHANGED] = &NetStatsCallbackStub::OnNetIfaceStatsChanged;
    memberFuncMap_[NET_STATS_UID_CHANGED] = &NetStatsCallbackStub::OnNetUidStatsChanged;
}

NetStatsCallbackStub::~NetStatsCallbackStub() = default;

int32_t NetStatsCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                              MessageOption &option)
{
    std::u16string myDescripters = NetStatsCallbackStub::GetDescriptor();
    std::u16string remoteDescripters = data.ReadInterfaceToken();
    if (myDescripters != remoteDescripters) {
        NETMGR_LOG_E("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetStatsCallbackStub::OnNetIfaceStatsChanged(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = NetIfaceStatsChanged(iface);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsCallbackStub::OnNetUidStatsChanged(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("ReadUint32 uid failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = NetUidStatsChanged(iface, uid);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
