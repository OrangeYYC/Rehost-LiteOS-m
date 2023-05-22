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
#include "net_supplier_callback_stub.h"

#include "net_mgr_log_wrapper.h"
#include "net_manager_constants.h"

static constexpr uint32_t MAX_NET_CAP_NUM = 32;

namespace OHOS {
namespace NetManagerStandard {
NetSupplierCallbackStub::NetSupplierCallbackStub()
{
    memberFuncMap_[NET_SUPPLIER_REQUEST_NETWORK] = &NetSupplierCallbackStub::OnRequestNetwork;
    memberFuncMap_[NET_SUPPLIER_RELEASE_NETWORK] = &NetSupplierCallbackStub::OnReleaseNetwork;
}

NetSupplierCallbackStub::~NetSupplierCallbackStub() {}

void NetSupplierCallbackStub::RegisterSupplierCallbackImpl(const sptr<NetSupplierCallbackBase> &callback)
{
    callback_ = callback;
}

int32_t NetSupplierCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    NETMGR_LOG_I("Net supplier callback stub call start, code:[%{public}d]", code);
    std::u16string myDescripter = NetSupplierCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        NETMGR_LOG_I("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETMGR_LOG_I("Stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetSupplierCallbackStub::OnRequestNetwork(MessageParcel &data, MessageParcel &reply)
{
    std::string ident;
    std::set<NetCap> netCaps;

    data.ReadString(ident);
    uint32_t size = 0;
    uint32_t value = 0;
    data.ReadUint32(size);
    size = (size > MAX_NET_CAP_NUM) ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; i++) {
        data.ReadUint32(value);
        netCaps.insert(static_cast<NetCap>(value));
    }

    RequestNetwork(ident, netCaps);

    reply.WriteInt32(0);
    return NETMANAGER_SUCCESS;
}

int32_t NetSupplierCallbackStub::OnReleaseNetwork(MessageParcel &data, MessageParcel &reply)
{
    std::string ident;
    std::set<NetCap> netCaps;

    data.ReadString(ident);
    uint32_t size = 0;
    uint32_t value = 0;
    data.ReadUint32(size);
    size = (size > MAX_NET_CAP_NUM) ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; i++) {
        data.ReadUint32(value);
        netCaps.insert(static_cast<NetCap>(value));
    }

    ReleaseNetwork(ident, netCaps);

    reply.WriteInt32(0);
    return NETMANAGER_SUCCESS;
}

int32_t NetSupplierCallbackStub::RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps)
{
    if (callback_ != nullptr) {
        callback_->RequestNetwork(ident, netCaps);
    }
    return 0;
}

int32_t NetSupplierCallbackStub::ReleaseNetwork(const std::string &ident, const std::set<NetCap> &netCaps)
{
    if (callback_ != nullptr) {
        callback_->ReleaseNetwork(ident, netCaps);
    }
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS
