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

#include "ethernet_service_stub.h"

#include "net_manager_constants.h"
#include "i_ethernet_service.h"
#include "interface_configuration.h"
#include "interface_type.h"
#include "ipc_object_stub.h"
#include "message_parcel.h"
#include "net_manager_constants.h"
#include "net_manager_ext_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr uint32_t MAX_SIZE = 16;
constexpr uint32_t MAX_IFACE_NAME_LEN = 13;
constexpr uint32_t MAX_MAC_ADDR_LEN = 17;
constexpr uint32_t MAX_IPV4_ADDR_LEN = 15;
constexpr uint32_t MAX_PRE_LEN = 128;
}

EthernetServiceStub::EthernetServiceStub()
{
    memberFuncMap_[CMD_SET_IF_CFG] = &EthernetServiceStub::OnSetIfaceConfig;
    memberFuncMap_[CMD_GET_IF_CFG] = &EthernetServiceStub::OnGetIfaceConfig;
    memberFuncMap_[CMD_IS_ACTIVATE] = &EthernetServiceStub::OnIsIfaceActive;
    memberFuncMap_[CMD_GET_ACTIVATE_INTERFACE] = &EthernetServiceStub::OnGetAllActiveIfaces;
    memberFuncMap_[CMD_RESET_FACTORY] = &EthernetServiceStub::OnResetFactory;
    memberFuncMap_[CMD_SET_INTERFACE_UP] = &EthernetServiceStub::OnSetInterfaceUp;
    memberFuncMap_[CMD_SET_INTERFACE_DOWN] = &EthernetServiceStub::OnSetInterfaceDown;
    memberFuncMap_[CMD_GET_INTERFACE_CONFIG] = &EthernetServiceStub::OnGetInterfaceConfig;
    memberFuncMap_[CMD_SET_INTERFACE_CONFIG] = &EthernetServiceStub::OnSetInterfaceConfig;
}

int32_t EthernetServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    NETMGR_EXT_LOG_D("stub call start, code = [%{public}d]", code);

    std::u16string myDescripter = EthernetServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
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
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t EthernetServiceStub::OnSetIfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    sptr<InterfaceConfiguration> ic = InterfaceConfiguration::Unmarshalling(data);
    if (ic == nullptr) {
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    return SetIfaceConfig(iface, ic);
}

int32_t EthernetServiceStub::OnGetIfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    sptr<InterfaceConfiguration> ifaceConfig = new (std::nothrow) InterfaceConfiguration();
    int32_t ret = GetIfaceConfig(iface, ifaceConfig);
    if (ret == NETMANAGER_EXT_SUCCESS && ifaceConfig != nullptr) {
        if (!reply.WriteInt32(GET_CFG_SUC)) {
            NETMGR_EXT_LOG_E("write failed");
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
        if (!ifaceConfig->Marshalling(reply)) {
            NETMGR_EXT_LOG_E("proxy Marshalling failed");
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t EthernetServiceStub::OnIsIfaceActive(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    int32_t activeStatus = 0;
    int32_t ret = IsIfaceActive(iface, activeStatus);
    if (ret == NETMANAGER_EXT_SUCCESS) {
        if (!reply.WriteUint32(activeStatus)) {
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t EthernetServiceStub::OnGetAllActiveIfaces(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> ifaces;
    int32_t ret = GetAllActiveIfaces(ifaces);
    NETMGR_EXT_LOG_E("ret %{public}d", ret);
    if (ret != NETMANAGER_EXT_SUCCESS || ifaces.size() == 0) {
        NETMGR_EXT_LOG_E("get all active ifaces failed");
        return ret;
    }
    if (ifaces.size() > MAX_SIZE) {
        NETMGR_EXT_LOG_E("ifaces size is too large");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    if (!reply.WriteUint32(ifaces.size())) {
        NETMGR_EXT_LOG_E("iface size write failed");
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    for (auto iface : ifaces) {
        if (!reply.WriteString(iface)) {
            NETMGR_EXT_LOG_E("iface write failed");
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetServiceStub::OnResetFactory(MessageParcel &data, MessageParcel &reply)
{
    return ResetFactory();
}

int32_t EthernetServiceStub::OnSetInterfaceUp(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    return SetInterfaceUp(iface);
}

int32_t EthernetServiceStub::OnSetInterfaceDown(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    return SetInterfaceDown(iface);
}

int32_t EthernetServiceStub::OnGetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    OHOS::nmd::InterfaceConfigurationParcel cfg;
    int32_t result = GetInterfaceConfig(iface, cfg);
    if (result != ERR_NONE) {
        NETMGR_EXT_LOG_E("GetInterfaceConfig is error");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    reply.WriteString(cfg.ifName);
    reply.WriteString(cfg.hwAddr);
    reply.WriteString(cfg.ipv4Addr);
    reply.WriteInt32(cfg.prefixLength);
    if (cfg.flags.size() > MAX_SIZE) {
        NETMGR_EXT_LOG_E("cfg flags size is too large");
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    reply.WriteInt32(static_cast<int32_t>(cfg.flags.size()));
    for (auto flag : cfg.flags) {
        reply.WriteString(flag);
    }
    reply.WriteInt32(result);
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetServiceStub::OnSetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    std::string iface = data.ReadString();

    OHOS::nmd::InterfaceConfigurationParcel cfg;
    cfg.ifName = data.ReadString();
    if (cfg.ifName.size() > MAX_IFACE_NAME_LEN || cfg.ifName.size() == 0) {
        NETMGR_EXT_LOG_E("ifName=[%{public}s] is too long", cfg.ifName.c_str());
        return NETMANAGER_EXT_ERR_INVALID_PARAMETER;
    }
    cfg.hwAddr = data.ReadString();
    if (cfg.hwAddr.size() > MAX_MAC_ADDR_LEN) {
        NETMGR_EXT_LOG_E("hwAddr=[%{public}s] is too long", cfg.hwAddr.c_str());
        return NETMANAGER_EXT_ERR_INVALID_PARAMETER;
    }
    cfg.ipv4Addr = data.ReadString();
    if (cfg.ipv4Addr.size() > MAX_IPV4_ADDR_LEN) {
        NETMGR_EXT_LOG_E("ipv4Addr=[%{public}s] is too long", cfg.ipv4Addr.c_str());
        return NETMANAGER_EXT_ERR_INVALID_PARAMETER;
    }
    cfg.prefixLength = data.ReadInt32();
    if (cfg.prefixLength > MAX_PRE_LEN) {
        NETMGR_EXT_LOG_E("prefixLength=[%{public}d] is too large", cfg.prefixLength);
        return NETMANAGER_EXT_ERR_INVALID_PARAMETER;
    }

    int32_t vecSize = data.ReadInt32();
    if (vecSize <= 0 || vecSize > MAX_SIZE) {
        NETMGR_EXT_LOG_E("flags size=[%{public}d] is 0 or too large", vecSize);
        return NETMANAGER_EXT_ERR_INVALID_PARAMETER;
    }
    for (int32_t idx = 0; idx < vecSize; idx++) {
        cfg.flags.push_back(data.ReadString());
    }

    int32_t result = SetInterfaceConfig(iface, cfg);
    if (result != ERR_NONE) {
        NETMGR_EXT_LOG_E("Set interface config failed");
        return NETMANAGER_EXT_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_EXT_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
