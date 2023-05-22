/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "mdns_service_stub.h"

#include "net_manager_ext_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
MDnsServiceStub::MDnsServiceStub()
{
    memberFuncMap_[CMD_DISCOVER] = &MDnsServiceStub::OnStartDiscoverService;
    memberFuncMap_[CMD_STOP_DISCOVER] = &MDnsServiceStub::OnStopDiscoverService;
    memberFuncMap_[CMD_REGISTER] = &MDnsServiceStub::OnRegisterService;
    memberFuncMap_[CMD_STOP_REGISTER] = &MDnsServiceStub::OnUnRegisterService;
    memberFuncMap_[CMD_RESOLVE] = &MDnsServiceStub::OnResolveService;
}

int32_t MDnsServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                         MessageOption &option)
{
    NETMGR_EXT_LOG_D("stub call start, code = [%{public}d]", code);

    if (GetDescriptor() != data.ReadInterfaceToken()) {
        NETMGR_EXT_LOG_E("descriptor checked fail");
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETMGR_EXT_LOG_D("stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t MDnsServiceStub::OnRegisterService(MessageParcel &data, MessageParcel &reply)
{
    sptr<MDnsServiceInfo> serviceInfo = MDnsServiceInfo::Unmarshalling(data);
    if (!serviceInfo) {
        NETMGR_EXT_LOG_E("serviceInfo is nullptr");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    NETMGR_EXT_LOG_D("MDnsServiceProxy [%{public}s][%{public}s][%{public}d]", serviceInfo->name.c_str(),
                     serviceInfo->type.c_str(), serviceInfo->port);

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote is nullptr.");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IRegistrationCallback> callback = iface_cast<IRegistrationCallback>(remote);
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("iface_cast callback is nullptr.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t err = RegisterService(*serviceInfo, callback);
    NETMGR_EXT_LOG_D("MDnsService::RegisterService return:[%{public}d]", err);
    if (!reply.WriteInt32(err)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return ERR_NONE;
}

int32_t MDnsServiceStub::OnUnRegisterService(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote ptr is nullptr.");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IRegistrationCallback> callback = iface_cast<IRegistrationCallback>(remote);
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("iface_cast callback is nullptr.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t err = UnRegisterService(callback);
    NETMGR_EXT_LOG_D("MDnsService::UnRegisterService return:[%{public}d]", err);
    if (!reply.WriteInt32(err)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return ERR_NONE;
}

int32_t MDnsServiceStub::OnStartDiscoverService(MessageParcel &data, MessageParcel &reply)
{
    std::string type;
    if (!data.ReadString(type)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote ptr is nullptr.");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IDiscoveryCallback> callback = iface_cast<IDiscoveryCallback>(remote);
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("iface_cast callback is nullptr.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t err = StartDiscoverService(type, callback);
    NETMGR_EXT_LOG_D("MDnsService::StartDiscoverService :[%{public}d]", err);
    if (!reply.WriteInt32(err)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return ERR_NONE;
}

int32_t MDnsServiceStub::OnStopDiscoverService(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote ptr is nullptr.");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IDiscoveryCallback> callback = iface_cast<IDiscoveryCallback>(remote);
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("iface_cast callback is nullptr.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t err = StopDiscoverService(callback);
    NETMGR_EXT_LOG_D("MDnsService::StopDiscoverService :[%{public}d]", err);
    if (!reply.WriteInt32(err)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return ERR_NONE;
}

int32_t MDnsServiceStub::OnResolveService(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_EXT_LOG_D("MDnsServiceStub::OnResolveService In");
    sptr<MDnsServiceInfo> serviceInfo = MDnsServiceInfo::Unmarshalling(data);
    if (!serviceInfo) {
        NETMGR_EXT_LOG_E("serviceInfo is nullptr");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote ptr is nullptr.");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }

    sptr<IResolveCallback> callback = iface_cast<IResolveCallback>(remote);
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("iface_cast callback is nullptr.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t err = ResolveService(*serviceInfo, callback);
    NETMGR_EXT_LOG_D("MDnsService::ResolveService :[%{public}d]", err);
    if (!reply.WriteInt32(err)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return ERR_NONE;
}
} // namespace NetManagerStandard
} // namespace OHOS