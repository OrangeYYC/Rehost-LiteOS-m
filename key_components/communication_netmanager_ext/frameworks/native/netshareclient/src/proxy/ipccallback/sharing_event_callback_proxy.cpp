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

#include "sharing_event_callback_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
SharingEventCallbackProxy::SharingEventCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<ISharingEventCallback>(object)
{
}

void SharingEventCallbackProxy::OnSharingStateChanged(const bool &isRunning)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("OnSharingStateChanged get Remote() error.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ISharingEventCallback::GetDescriptor());

    if (!data.WriteBool(isRunning)) {
        NETMGR_EXT_LOG_E("OnSharingStateChanged WriteInt32 error.");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(ISharingEventCallback::Message::GLOBAL_SHARING_STATE_CHANGED), data, reply, option);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("OnSharingStateChanged SendRequest error=[%{public}d].", ret);
    }
}

void SharingEventCallbackProxy::OnInterfaceSharingStateChanged(const SharingIfaceType &type, const std::string &iface,
                                                               const SharingIfaceState &state)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("OnInterfaceSharingStateChanged get Remote() error.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ISharingEventCallback::GetDescriptor());

    if (!data.WriteInt32(static_cast<int32_t>(type))) {
        NETMGR_EXT_LOG_E("OnInterfaceSharingStateChanged type WriteInt32 error.");
        return;
    }
    if (!data.WriteString(iface)) {
        NETMGR_EXT_LOG_E("OnInterfaceSharingStateChanged WriteString error.");
        return;
    }
    if (!data.WriteInt32(static_cast<int32_t>(state))) {
        NETMGR_EXT_LOG_E("OnInterfaceSharingStateChanged state WriteInt32 error.");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(ISharingEventCallback::Message::INTERFACE_SHARING_STATE_CHANGED), data, reply, option);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("OnInterfaceSharingStateChanged SendRequest error=[%{public}d].", ret);
    }
}

void SharingEventCallbackProxy::OnSharingUpstreamChanged(const sptr<NetHandle> netHandle)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("OnSharingUpstreamChanged get Remote() error.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ISharingEventCallback::GetDescriptor());

    if (!data.WriteInt32(netHandle->GetNetId())) {
        NETMGR_EXT_LOG_E("OnSharingUpstreamChanged WriteInt32 error");
        return;
    }

    int32_t ret = remote->SendRequest(static_cast<int32_t>(ISharingEventCallback::Message::SHARING_UPSTREAM_CHANGED),
                                      data, reply, option);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("OnSharingUpstreamChanged SendRequest error=[%{public}d].", ret);
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
