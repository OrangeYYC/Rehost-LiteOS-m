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

#ifndef MDNS_SERVICE_PROXY_H
#define MDNS_SERVICE_PROXY_H

#include "iremote_proxy.h"

#include "i_mdns_event.h"

namespace OHOS {
namespace NetManagerStandard {
class RegistrationCallbackProxy : public IRemoteProxy<IRegistrationCallback> {
public:
    explicit RegistrationCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~RegistrationCallbackProxy() = default;

    void HandleRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleUnRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleRegisterResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;

private:
    static inline BrokerDelegator<RegistrationCallbackProxy> delegator_;
};

class DiscoveryCallbackProxy : public IRemoteProxy<IDiscoveryCallback> {
public:
    explicit DiscoveryCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~DiscoveryCallbackProxy() = default;

    void HandleStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleServiceFound(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleServiceLost(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;

private:
    static inline BrokerDelegator<DiscoveryCallbackProxy> delegator_;
};

class ResolveCallbackProxy : public IRemoteProxy<IResolveCallback> {
public:
    explicit ResolveCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~ResolveCallbackProxy() = default;

    void HandleResolveResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;

private:
    static inline BrokerDelegator<ResolveCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // MDNS_SERVICE_PROXY_H