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

#include "net_mgr_log_wrapper.h"
#include "net_supplier_callback_proxy.h"
#include "net_manager_constants.h"
namespace OHOS {
namespace NetManagerStandard {
NetSupplierCallbackProxy::NetSupplierCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetSupplierCallback>(impl)
{}

NetSupplierCallbackProxy::~NetSupplierCallbackProxy() {}

int32_t NetSupplierCallbackProxy::RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteString(ident);
    uint32_t size = static_cast<uint32_t>(netCaps.size());
    data.WriteUint32(size);
    for (auto netCap : netCaps) {
        data.WriteUint32(static_cast<uint32_t>(netCap));
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NET_SUPPLIER_REQUEST_NETWORK, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NetSupplierCallbackProxy::ReleaseNetwork(const std::string &ident, const std::set<NetCap> &netCaps)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    data.WriteString(ident);
    uint32_t size = static_cast<uint32_t>(netCaps.size());
    data.WriteUint32(size);
    for (auto netCap : netCaps) {
        data.WriteInt32(static_cast<uint32_t>(netCap));
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NET_SUPPLIER_RELEASE_NETWORK, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

bool NetSupplierCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetSupplierCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
