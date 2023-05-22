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

#ifndef NET_ACTIVATE_H
#define NET_ACTIVATE_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "i_net_conn_callback.h"
#include "net_specifier.h"
#include "net_supplier.h"

class NetSupplier;

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t DEFAULT_REQUEST_ID = 0;
constexpr uint32_t MIN_REQUEST_ID = DEFAULT_REQUEST_ID + 1;
constexpr uint32_t MAX_REQUEST_ID = 0x7FFFFFFF;
class INetActivateCallback {
public:
    virtual ~INetActivateCallback() = default;

public:
    virtual void OnNetActivateTimeOut(uint32_t reqId) = 0;
};

class NetActivate : public virtual RefBase {
public:
    using TimeOutHandler = std::function<int32_t(uint32_t &reqId)>;

public:
    NetActivate(const sptr<NetSpecifier> &specifier, const sptr<INetConnCallback> &callback,
                std::weak_ptr<INetActivateCallback> timeoutCallback, const uint32_t &timeoutMS);
    ~NetActivate();
    bool MatchRequestAndNetwork(sptr<NetSupplier> supplier);
    void SetRequestId(uint32_t reqId);
    uint32_t GetRequestId() const;
    sptr<NetSupplier> GetServiceSupply() const;
    void SetServiceSupply(sptr<NetSupplier> netServiceSupplied);
    sptr<INetConnCallback> GetNetCallback();
    sptr<NetSpecifier> GetNetSpecifier();

private:
    bool CompareByNetworkIdent(const std::string &ident);
    bool CompareByNetworkCapabilities(const NetCaps &netCaps);
    bool CompareByNetworkNetType(NetBearType bearerType);
    bool CompareByNetworkBand(uint32_t netLinkUpBand, uint32_t netLinkDownBand);
    bool HaveCapability(NetCap netCap) const;
    bool HaveTypes(const std::set<NetBearType> &bearerTypes) const;
    void TimeOutNetAvailable();

private:
    uint32_t requestId_ = 1;
    sptr<NetSpecifier> netSpecifier_ = nullptr;
    sptr<INetConnCallback> netConnCallback_ = nullptr;
    sptr<NetSupplier> netServiceSupplied_ = nullptr;
    uint32_t timeoutMS_ = 0;
    std::weak_ptr<INetActivateCallback> timeoutCallback_;
    std::string activateName_ = "";
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ACTIVATE_H