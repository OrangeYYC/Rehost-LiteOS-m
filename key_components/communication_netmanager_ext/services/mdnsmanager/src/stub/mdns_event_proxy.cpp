/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS"BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mdns_event_proxy.h"

#include <codecvt>

#include "iremote_object.h"
#include "net_manager_ext_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
RegistrationCallbackProxy::RegistrationCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IRegistrationCallback>(impl)
{
}

void RegistrationCallbackProxy::HandleRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IRegistrationCallback::Message::REGISTERED), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

void RegistrationCallbackProxy::HandleUnRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    NETMGR_EXT_LOG_I("SendRequest");
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IRegistrationCallback::Message::UNREGISTERED), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

void RegistrationCallbackProxy::HandleRegisterResult(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IRegistrationCallback::Message::RESULT), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

DiscoveryCallbackProxy::DiscoveryCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IDiscoveryCallback>(impl)
{
}

void DiscoveryCallbackProxy::HandleStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }

    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IDiscoveryCallback::Message::STARTED), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

void DiscoveryCallbackProxy::HandleStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }

    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IDiscoveryCallback::Message::STOPPED), data, reply, option);
    option.SetFlags(MessageOption::TF_ASYNC);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

void DiscoveryCallbackProxy::HandleServiceFound(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IDiscoveryCallback::Message::FOUND), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

void DiscoveryCallbackProxy::HandleServiceLost(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IDiscoveryCallback::Message::LOST), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}

ResolveCallbackProxy::ResolveCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IResolveCallback>(impl) {}

void ResolveCallbackProxy::HandleResolveResult(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling MDnsServiceInfo failed");
        return;
    }
    if (!data.WriteInt32(retCode)) {
        NETMGR_EXT_LOG_E("WriteInt32 failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IResolveCallback::Message::RESULT), data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
    }
}
} // namespace NetManagerStandard
} // namespace OHOS