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

#ifndef I_NET_CONN_SERVICE_H
#define I_NET_CONN_SERVICE_H

#include <string>

#include "iremote_broker.h"

#include "i_net_conn_callback.h"
#include "i_net_supplier_callback.h"
#include "i_net_detection_callback.h"
#include "net_conn_constants.h"
#include "net_link_info.h"
#include "net_supplier_info.h"
#include "net_specifier.h"
#include "http_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class INetConnService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetConnService");
    enum {
        CMD_NM_START,
        CMD_NM_REGISTER_NET_SUPPLIER,
        CMD_NM_SYSTEM_READY,
        CMD_NM_REGISTER_NET_CONN_CALLBACK,
        CMD_NM_REGISTER_NET_CONN_CALLBACK_BY_SPECIFIER,
        CMD_NM_UNREGISTER_NET_CONN_CALLBACK,
        CMD_NM_REG_NET_SUPPLIER,
        CMD_NM_UNREG_NETWORK,
        CMD_NM_SET_NET_SUPPLIER_INFO,
        CMD_NM_SET_NET_LINK_INFO,
        CMD_NM_GETDEFAULTNETWORK,
        CMD_NM_HASDEFAULTNET,
        CMD_NM_NET_DETECTION,
        CMD_NM_GET_IFACE_NAMES,
        CMD_NM_GET_IFACENAME_BY_TYPE,
        CMD_NM_GET_ADDRESSES_BY_NAME,
        CMD_NM_GET_ADDRESS_BY_NAME,
        CMD_NM_GET_SPECIFIC_NET,
        CMD_NM_GET_ALL_NETS,
        CMD_NM_GET_SPECIFIC_UID_NET,
        CMD_NM_GET_CONNECTION_PROPERTIES,
        CMD_NM_GET_NET_CAPABILITIES,
        CMD_NM_BIND_SOCKET,
        CMD_NM_REGISTER_NET_DETECTION_RET_CALLBACK,
        CMD_NM_UNREGISTER_NET_DETECTION_RET_CALLBACK,
        CMD_NM_UPDATE_NET_STATE_FOR_TEST,
        CMD_NM_REGISTER_NET_SUPPLIER_CALLBACK,
        CMD_NM_SET_AIRPLANE_MODE,
        CMD_NM_IS_DEFAULT_NET_METERED,
        CMD_NM_SET_HTTP_PROXY,
        CMD_NM_GET_HTTP_PROXY,
        CMD_NM_GET_NET_ID_BY_IDENTIFIER,
        CMD_NM_SET_APP_NET,
        CMD_NM_END,
    };

public:
    virtual int32_t SystemReady() = 0;
    virtual int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                        const std::set<NetCap> &netCaps, uint32_t &supplierId) = 0;
    virtual int32_t UnregisterNetSupplier(uint32_t supplierId) = 0;
    virtual int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<INetSupplierCallback> &callback) = 0;
    virtual int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback) = 0;
    virtual int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier,
                                            const sptr<INetConnCallback> &callback, const uint32_t &timeoutMS) = 0;
    virtual int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback) = 0;
    virtual int32_t UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState) = 0;
    virtual int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) = 0;
    virtual int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) = 0;
    virtual int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) = 0;
    virtual int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) = 0;
    virtual int32_t RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) = 0;
    virtual int32_t UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) = 0;
    virtual int32_t NetDetection(int32_t netId) = 0;
    virtual int32_t GetDefaultNet(int32_t &netId) = 0;
    virtual int32_t HasDefaultNet(bool &flag) = 0;
    virtual int32_t GetAddressesByName(const std::string &host, int32_t netId, std::vector<INetAddr> &addrList) = 0;
    virtual int32_t GetAddressByName(const std::string &host, int32_t netId, INetAddr &addr) = 0;
    virtual int32_t GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList) = 0;
    virtual int32_t GetAllNets(std::list<int32_t> &netIdList) = 0;
    virtual int32_t GetSpecificUidNet(int32_t uid, int32_t &netId) = 0;
    virtual int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) = 0;
    virtual int32_t GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap) = 0;
    virtual int32_t BindSocket(int32_t socket_fd, int32_t netId) = 0;
    virtual int32_t SetAirplaneMode(bool state) = 0;
    virtual int32_t IsDefaultNetMetered(bool &isMetered) = 0;
    virtual int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy) = 0;
    virtual int32_t GetGlobalHttpProxy(HttpProxy &httpProxy) = 0;
    virtual int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList) = 0;
    virtual int32_t SetAppNet(int32_t netId) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_CONN_SERVICE_H
