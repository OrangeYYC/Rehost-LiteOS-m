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
#include "net_conn_callback_stub.h"
#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_NET_CAP_NUM = 32;

NetConnCallbackStub::NetConnCallbackStub()
{
    memberFuncMap_[NET_AVAILABLE] = &NetConnCallbackStub::OnNetAvailable;
    memberFuncMap_[NET_CAPABILITIES_CHANGE] = &NetConnCallbackStub::OnNetCapabilitiesChange;
    memberFuncMap_[NET_CONNECTION_PROPERTIES_CHANGE] = &NetConnCallbackStub::OnNetConnectionPropertiesChange;
    memberFuncMap_[NET_LOST] = &NetConnCallbackStub::OnNetLost;
    memberFuncMap_[NET_UNAVAILABLE] = &NetConnCallbackStub::OnNetUnavailable;
    memberFuncMap_[NET_BLOCK_STATUS_CHANGE] = &NetConnCallbackStub::OnNetBlockStatusChange;
}

NetConnCallbackStub::~NetConnCallbackStub() {}

int32_t NetConnCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    NETMGR_LOG_D("Stub call start, code:[%{public}d]", code);
    std::u16string myDescripter = NetConnCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
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

    NETMGR_LOG_D("Stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetConnCallbackStub::OnNetAvailable(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    sptr<NetHandle> netHandle = std::make_unique<NetHandle>(netId).release();
    int32_t result = NetAvailable(netHandle);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::OnNetCapabilitiesChange(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }

    int32_t netId = 0;
    sptr<NetAllCapabilities> netAllCap = std::make_unique<NetAllCapabilities>().release();
    if (!data.ReadInt32(netId) || !data.ReadUint32(netAllCap->linkUpBandwidthKbps_) ||
        !data.ReadUint32(netAllCap->linkDownBandwidthKbps_)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    uint32_t size = 0;
    uint32_t value = 0;
    if (!data.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; i++) {
        if (!data.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        netAllCap->netCaps_.insert(static_cast<NetCap>(value));
    }
    if (!data.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; i++) {
        if (!data.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        netAllCap->bearerTypes_.insert(static_cast<NetBearType>(value));
    }

    sptr<NetHandle> netHandle = std::make_unique<NetHandle>(netId).release();
    int32_t result = NetCapabilitiesChange(netHandle, netAllCap);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::OnNetConnectionPropertiesChange(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }

    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    sptr<NetLinkInfo> info = NetLinkInfo::Unmarshalling(data);
    sptr<NetHandle> netHandle = std::make_unique<NetHandle>(netId).release();
    int32_t result = NetConnectionPropertiesChange(netHandle, info);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::OnNetLost(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }

    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    sptr<NetHandle> netHandle = std::make_unique<NetHandle>(netId).release();
    int32_t result = NetLost(netHandle);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::OnNetUnavailable(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }

    int32_t result = NetUnavailable();
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::OnNetBlockStatusChange(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_W("sent raw data is less than 32k");
    }

    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    bool blocked;
    if (!data.ReadBool(blocked)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    sptr<NetHandle> netHandle = std::make_unique<NetHandle>(netId).release();
    int32_t result = NetBlockStatusChange(netHandle, blocked);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write parcel failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetAvailable(sptr<NetHandle> &netHandle)
{
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetCapabilitiesChange(sptr<NetHandle> &netHandle,
                                                   const sptr<NetAllCapabilities> &netAllCap)
{
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
{
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetLost(sptr<NetHandle> &netHandle)
{
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetUnavailable()
{
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStub::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
