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

#ifndef NET_CONN_MANAGER_H
#define NET_CONN_MANAGER_H

#include <string>
#include <map>

#include "parcel.h"
#include "singleton.h"

#include "i_net_conn_service.h"
#include "i_net_supplier_callback.h"
#include "net_supplier_callback_base.h"
#include "net_link_info.h"
#include "net_specifier.h"
#include "net_handle.h"
#include "http_proxy.h"

namespace OHOS {
namespace nmd {
class FwmarkClient;
}
namespace NetManagerStandard {
class NetConnClient {
    DECLARE_DELAYED_SINGLETON(NetConnClient)

public:
    int32_t SystemReady();
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId);
    int32_t UnregisterNetSupplier(uint32_t supplierId);
    int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<NetSupplierCallbackBase> &callback);
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo);
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo);
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback);
    int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                    const uint32_t &timeoutMS);
    int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback);
    int32_t GetDefaultNet(NetHandle &netHandle);
    int32_t HasDefaultNet(bool &flag);
    int32_t GetAllNets(std::list<sptr<NetHandle>> &netList);
    int32_t GetConnectionProperties(const NetHandle &netHandle, NetLinkInfo &info);
    int32_t GetNetCapabilities(const NetHandle &netHandle, NetAllCapabilities &netAllCap);
    int32_t GetAddressesByName(const std::string &host, int32_t netId, std::vector<INetAddr> &addrList);
    int32_t GetAddressByName(const std::string &host, int32_t netId, INetAddr &addr);
    int32_t BindSocket(int32_t socket_fd, int32_t netId);
    int32_t NetDetection(const NetHandle &netHandle);
    int32_t SetAirplaneMode(bool state);
    int32_t IsDefaultNetMetered(bool &isMetered);
    int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy);
    int32_t GetGlobalHttpProxy(HttpProxy &httpProxy);
    int32_t SetAppNet(int32_t netId);
    int32_t GetAppNet(int32_t &netId);
    int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList);

private:
    class NetConnDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetConnDeathRecipient(NetConnClient &client) : client_(client) {}
        ~NetConnDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetConnClient &client_;
    };

private:
    sptr<INetConnService> GetProxy();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    std::mutex mutex_;
    sptr<INetConnService> NetConnService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    std::map<uint32_t, sptr<INetSupplierCallback>> netSupplierCallback_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_CONN_MANAGER_H
