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

#ifndef NET_CONN_SERVICE_H
#define NET_CONN_SERVICE_H

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "singleton.h"
#include "system_ability.h"

#include "network.h"
#include "net_activate.h"
#include "net_conn_event_handler.h"
#include "net_conn_service_iface.h"
#include "net_conn_service_stub.h"
#include "net_score.h"
#include "net_supplier.h"
#include "http_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t MAX_REQUEST_NUM = 2000;
class NetConnService : public SystemAbility,
                       public INetActivateCallback,
                       public NetConnServiceStub,
                       public std::enable_shared_from_this<NetConnService> {
    DECLARE_DELAYED_SINGLETON(NetConnService)
    DECLARE_SYSTEM_ABILITY(NetConnService)

    using NET_SUPPLIER_MAP = std::map<uint32_t, sptr<NetSupplier>>;
    using NET_NETWORK_MAP = std::map<int32_t, std::shared_ptr<Network>>;
    using NET_ACTIVATE_MAP = std::map<uint32_t, sptr<NetActivate>>;

public:
    void OnStart() override;
    void OnStop() override;
    /**
     * The interface in NetConnService can be called when the system is ready
     *
     * @return Returns 0, the system is ready, otherwise the system is not ready
     */
    int32_t SystemReady() override;

    /**
     * The interface is register the network
     *
     * @param bearerType Bearer Network Type
     * @param ident Unique identification of mobile phone card
     * @param netCaps Network capabilities registered by the network supplier
     * @param supplierId out param, return supplier id
     *
     * @return function result
     */
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId) override;

    /**
     * The interface is unregister the network
     *
     * @param supplierId The id of the network supplier
     *
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t UnregisterNetSupplier(uint32_t supplierId) override;

    /**
     * Register supplier callback
     *
     * @param supplierId The id of the network supplier
     * @param callback INetSupplierCallback callback interface
     *
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<INetSupplierCallback> &callback) override;

    /**
     * Register net connection callback
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     *
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     */
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback) override;

    /**
     * Register net connection callback by NetSpecifier
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     * @param timeoutMS net connection time out
     *
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     */
    int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                    const uint32_t &timeoutMS) override;

    /**
     * Unregister net connection callback
     *
     * @return Returns 0, successfully unregister net connection callback, otherwise it will fail
     */
    int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback) override;

    int32_t UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState) override;
    /**
     * The interface is update network connection status information
     *
     * @param supplierId The id of the network supplier
     * @param netSupplierInfo network connection status information
     *
     * @return Returns 0, successfully update the network connection status information, otherwise it will fail
     */
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) override;

    /**
     * The interface is update network link attribute information
     *
     * @param supplierId The id of the network supplier
     * @param netLinkInfo network link attribute information
     *
     * @return Returns 0, successfully update the network link attribute information, otherwise it will fail
     */
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) override;

    /**
     * The interface names which NetBearType is equal than bearerType
     *
     * @param bearerType Network bearer type
     * @param ifaceNames save the obtained ifaceNames
     * @return Returns 0, successfully get the network link attribute iface name, otherwise it will fail
     */
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) override;

    /**
     * The interface is get the iface name for network
     *
     * @param bearerType Network bearer type
     * @param ident Unique identification of mobile phone card
     * @param ifaceName save the obtained ifaceName
     * @return Returns 0, successfully get the network link attribute iface name, otherwise it will fail
     */
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) override;

    /**
     * register network detection return result method
     *
     * @param netId  Network ID
     * @param callback The callback of INetDetectionCallback interface
     * @return int32_t  Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) override;

    /**
     * unregister network detection return result method
     *
     * @param netId Network ID
     * @param callback  The callback of INetDetectionCallback interface
     * @return int32_t  Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) override;

    /**
     * The interface of network detection called by the application
     *
     * @param netId network ID
     * @return int32_t Whether the network probe is successful
     */
    int32_t NetDetection(int32_t netId) override;
    int32_t GetDefaultNet(int32_t &netId) override;
    int32_t HasDefaultNet(bool &flag) override;
    int32_t GetAddressesByName(const std::string &host, int32_t netId, std::vector<INetAddr> &addrList) override;
    int32_t GetAddressByName(const std::string &host, int32_t netId, INetAddr &addr) override;
    int32_t GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList) override;
    int32_t GetAllNets(std::list<int32_t> &netIdList) override;
    int32_t GetSpecificUidNet(int32_t uid, int32_t &netId) override;
    int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) override;
    int32_t GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap) override;
    int32_t BindSocket(int32_t socket_fd, int32_t netId) override;
    void HandleDetectionResult(uint32_t supplierId, bool ifValid);
    int32_t RestrictBackgroundChanged(bool isRestrictBackground);
    /**
     * Set airplane mode
     *
     * @param state airplane state
     * @return Returns 0, successfully set airplane mode, otherwise it will fail
     */
    int32_t SetAirplaneMode(bool state) override;
    /**
     * Dump
     *
     * @param fd file description
     * @param args unused
     * @return Returns 0, successfully get dump info, otherwise it will fail
     */
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    /**
     * Is default network metered
     *
     * @param save the metered state
     * @return Returns 0, Successfully get whether the default network is metered, otherwise it will fail
     */
    int32_t IsDefaultNetMetered(bool &isMetered) override;

    /**
     * Set http proxy server
     *
     * @param httpProxy the http proxy server
     * @return NETMANAGER_SUCCESS if OK, NET_CONN_ERR_HTTP_PROXY_INVALID if httpProxy is null string
     */
    int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy) override;

    /**
     * Get http proxy server
     *
     * @param httpProxy output param, the http proxy server
     * @return NETMANAGER_SUCCESS if OK, NET_CONN_ERR_NO_HTTP_PROXY if httpProxy is null string
     */
    int32_t GetGlobalHttpProxy(HttpProxy &httpProxy) override;

    /**
     * Get net id by identifier
     *
     * @param ident Net identifier
     * @param netIdList output param, the net id list
     * @return NETMANAGER_SUCCESS if OK, ERR_NO_NET_IDENT if ident is null string
     */
    int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList) override;

    /**
     * Activate network timeout
     *
     * @param reqId Net request id
     */
    void OnNetActivateTimeOut(uint32_t reqId) override;

    int32_t SetAppNet(int32_t netId) override;

private:
    bool Init();
    std::list<sptr<NetSupplier>> GetNetSupplierFromList(NetBearType bearerType, const std::string &ident = "");
    sptr<NetSupplier> GetNetSupplierFromList(NetBearType bearerType, const std::string &ident,
                                             const std::set<NetCap> &netCaps);
    int32_t ActivateNetwork(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                            const uint32_t &timeoutMS);
    void CallbackForSupplier(sptr<NetSupplier> &supplier, CallbackType type);
    void CallbackForAvailable(sptr<NetSupplier> &supplier, const sptr<INetConnCallback> &callback);
    uint32_t FindBestNetworkForRequest(sptr<NetSupplier> &supplier, sptr<NetActivate> &netActivateNetwork);
    void SendRequestToAllNetwork(sptr<NetActivate> request);
    void SendBestScoreAllNetwork(uint32_t reqId, int32_t bestScore, uint32_t supplierId);
    void SendAllRequestToNetwork(sptr<NetSupplier> supplier);
    void FindBestNetworkForAllRequest();
    void MakeDefaultNetWork(sptr<NetSupplier> &oldService, sptr<NetSupplier> &newService);
    void NotFindBestSupplier(uint32_t reqId, const sptr<NetActivate> &active, const sptr<NetSupplier> &supplier,
                             const sptr<INetConnCallback> &callback);
    void CreateDefaultRequest();
    int32_t RegUnRegNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback, bool isReg);
    int32_t GenerateNetId();
    bool FindSameCallback(const sptr<INetConnCallback> &callback, uint32_t &reqId);
    void GetDumpMessage(std::string &message);
    sptr<NetSupplier> FindNetSupplier(uint32_t supplierId);
    int32_t RegisterNetSupplierAsync(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                     uint32_t &supplierId);
    int32_t UnregisterNetSupplierAsync(uint32_t supplierId);
    int32_t RegisterNetSupplierCallbackAsync(uint32_t supplierId, const sptr<INetSupplierCallback> &callback);
    int32_t RegisterNetConnCallbackAsync(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                         const uint32_t &timeoutMS);
    int32_t UnregisterNetConnCallbackAsync(const sptr<INetConnCallback> &callback);
    int32_t RegUnRegNetDetectionCallbackAsync(int32_t netId, const sptr<INetDetectionCallback> &callback, bool isReg);
    int32_t UpdateNetStateForTestAsync(const sptr<NetSpecifier> &netSpecifier, int32_t netState);
    int32_t UpdateNetSupplierInfoAsync(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo);
    int32_t UpdateNetLinkInfoAsync(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo);
    int32_t NetDetectionAsync(int32_t netId);
    int32_t RestrictBackgroundChangedAsync(bool restrictBackground);
    int32_t SetGlobalHttpProxyAsync(const HttpProxy &httpProxy);
    void SendGlobalHttpProxyChangeBroadcast();
    void RequestAllNetworkExceptDefault();

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    bool registerToService_;
    ServiceRunningState state_;
    sptr<NetSpecifier> defaultNetSpecifier_ = nullptr;
    sptr<NetActivate> defaultNetActivate_ = nullptr;
    sptr<NetSupplier> defaultNetSupplier_ = nullptr;
    NET_SUPPLIER_MAP netSuppliers_;
    NET_ACTIVATE_MAP netActivates_;
    NET_NETWORK_MAP networks_;
    std::unique_ptr<NetScore> netScore_ = nullptr;
    sptr<NetConnServiceIface> serviceIface_ = nullptr;
    std::atomic<int32_t> netIdLastValue_ = MIN_NET_ID - 1;
    HttpProxy httpProxy_;
    std::mutex netManagerMutex_;
    std::shared_ptr<AppExecFwk::EventRunner> netConnEventRunner_ = nullptr;
    std::shared_ptr<NetConnEventHandler> netConnEventHandler_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_SERVICE_H
