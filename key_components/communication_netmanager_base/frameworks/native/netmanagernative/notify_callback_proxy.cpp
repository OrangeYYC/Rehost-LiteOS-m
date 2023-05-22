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
#include "notify_callback_proxy.h"

#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
namespace {
bool WriteInterfaceStateData(MessageParcel &data, const std::string &ifName)
{
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteString(ifName)) {
        return false;
    }
    return true;
}

bool WriteInterfaceAddressData(MessageParcel &data, const std::string &addr, const std::string &ifName, int flags,
                               int scope)
{
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteString(addr)) {
        return false;
    }

    if (!data.WriteString(ifName)) {
        return false;
    }

    if (!data.WriteInt32(flags)) {
        return false;
    }

    if (!data.WriteInt32(scope)) {
        return false;
    }
    return true;
}

bool WriteLinkStateData(MessageParcel &data, const std::string &ifName, bool up)
{
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteString(ifName)) {
        return false;
    }

    if (!data.WriteBool(up)) {
        return false;
    }
    return true;
}
}
NotifyCallbackProxy::NotifyCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INotifyCallback>(impl)
{}

NotifyCallbackProxy::~NotifyCallbackProxy() {}

int32_t NotifyCallbackProxy::OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int flags,
                                                       int scope)
{
    NETNATIVE_LOGI("Proxy OnInterfaceAddressUpdated");
    MessageParcel data;
    if (!WriteInterfaceAddressData(data, addr, ifName, flags, scope)) {
        NETNATIVE_LOGE("WriteInterfaceAddressData failed");
        return ERR_NONE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_ADDRESS_UPDATED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int flags,
                                                       int scope)
{
    NETNATIVE_LOGI("Proxy OnInterfaceAddressRemoved");
    MessageParcel data;
    if (!WriteInterfaceAddressData(data, addr, ifName, flags, scope)) {
        NETNATIVE_LOGE("WriteInterfaceAddressData failed");
        return ERR_NONE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_ADDRESS_REMOVED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnInterfaceAdded(const std::string &ifName)
{
    NETNATIVE_LOGI("Proxy OnInterfaceAdded");
    MessageParcel data;
    if (!WriteInterfaceStateData(data, ifName)) {
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_ADDED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnInterfaceRemoved(const std::string &ifName)
{
    NETNATIVE_LOGI("Proxy OnInterfaceRemoved");
    MessageParcel data;
    if (!WriteInterfaceStateData(data, ifName)) {
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_REMOVED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnInterfaceChanged(const std::string &ifName, bool up)
{
    NETNATIVE_LOGI("Proxy OnInterfaceChanged");
    MessageParcel data;
    if (!WriteLinkStateData(data, ifName, up)) {
        return ERR_NONE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_CHANGED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    NETNATIVE_LOGI("Proxy OnInterfaceLinkStateChanged");
    MessageParcel data;
    if (!WriteLinkStateData(data, ifName, up)) {
        return ERR_NONE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_INTERFACE_LINK_STATE_CHANGED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                                            const std::string &ifName)
{
    NETNATIVE_LOGI("Proxy OnRouteChanged");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteBool(updated)) {
        return false;
    }

    if (!data.WriteString(route)) {
        return false;
    }

    if (!data.WriteString(gateway)) {
        return false;
    }

    if (!data.WriteString(ifName)) {
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_ROUTE_CHANGED, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnDhcpSuccess(sptr<DhcpResultParcel> &dhcpResult)
{
    NETNATIVE_LOGI("Proxy OnDhcpSuccess");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }
    dhcpResult->Marshalling(data);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_DHCP_SUCCESS, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}

int32_t NotifyCallbackProxy::OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
{
    NETNATIVE_LOGI("Proxy OnBandwidthReachedLimit");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NotifyCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteString(limitName)) {
        return false;
    }
    if (!data.WriteString(iface)) {
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return ERR_NULL_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(ON_BANDWIDTH_REACHED_LIMIT, data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return reply.ReadInt32();
}
} // namespace NetsysNative
} // namespace OHOS
