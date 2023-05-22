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

#include "mdns_event_stub.h"

#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

int32_t RegistrationCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                  MessageOption &option)
{
    NETMGR_EXT_LOG_D("stub call start, code = [%{public}d]", code);

    if (GetDescriptor() != data.ReadInterfaceToken()) {
        NETMGR_EXT_LOG_E("descriptor checked fail");
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }

    sptr<MDnsServiceInfo> serviceInfo = MDnsServiceInfo::Unmarshalling(data);
    if (serviceInfo == nullptr) {
        NETMGR_EXT_LOG_E("MDnsServiceInfo::Unmarshalling failed");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    int32_t retCode = 0;
    if (!data.ReadInt32(retCode)) {
        NETMGR_EXT_LOG_E("ReadInt32 failed");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    IRegistrationCallback::Message msgCode = static_cast<IRegistrationCallback::Message>(code);
    if (msgCode == IRegistrationCallback::Message::REGISTERED) {
        HandleRegister(*serviceInfo, retCode);
    } else if (msgCode == IRegistrationCallback::Message::UNREGISTERED) {
        HandleUnRegister(*serviceInfo, retCode);
    } else if (msgCode == IRegistrationCallback::Message::RESULT) {
        HandleRegisterResult(*serviceInfo, retCode);
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}

int32_t DiscoveryCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                               MessageOption &option)
{
    NETMGR_EXT_LOG_D("stub call start, code = [%{public}d]", code);

    if (GetDescriptor() != data.ReadInterfaceToken()) {
        NETMGR_EXT_LOG_E("descriptor checked fail");
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }

    sptr<MDnsServiceInfo> serviceInfo = MDnsServiceInfo::Unmarshalling(data);
    if (serviceInfo == nullptr) {
        NETMGR_EXT_LOG_E("MDnsServiceInfo::Unmarshalling failed");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    int32_t retCode = 0;
    if (!data.ReadInt32(retCode)) {
        NETMGR_EXT_LOG_E("ReadInt32 failed");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    IDiscoveryCallback::Message msgCode = static_cast<IDiscoveryCallback::Message>(code);
    if (msgCode == IDiscoveryCallback::Message::STARTED) {
        HandleStartDiscover(*serviceInfo, retCode);
    } else if (msgCode == IDiscoveryCallback::Message::STOPPED) {
        HandleStopDiscover(*serviceInfo, retCode);
    } else if (msgCode == IDiscoveryCallback::Message::FOUND) {
        HandleServiceFound(*serviceInfo, retCode);
    } else if (msgCode == IDiscoveryCallback::Message::LOST) {
        HandleServiceLost(*serviceInfo, retCode);
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}

int32_t ResolveCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    NETMGR_EXT_LOG_D("stub call start, code = [%{public}d]", code);

    if (GetDescriptor() != data.ReadInterfaceToken()) {
        NETMGR_EXT_LOG_E("descriptor checked fail");
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }

    sptr<MDnsServiceInfo> serviceInfo = MDnsServiceInfo::Unmarshalling(data);
    if (serviceInfo == nullptr) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    int32_t retCode = 0;
    if (!data.ReadInt32(retCode)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    IResolveCallback::Message msgCode = static_cast<IResolveCallback::Message>(code);
    if (msgCode == IResolveCallback::Message::RESULT) {
        HandleResolveResult(*serviceInfo, retCode);
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}
} // namespace NetManagerStandard
} // namespace OHOS