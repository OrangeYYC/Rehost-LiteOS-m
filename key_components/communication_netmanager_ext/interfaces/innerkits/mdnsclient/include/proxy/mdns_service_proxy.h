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

#include "i_mdns_service.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsServiceProxy : public IRemoteProxy<IMDnsService> {
public:
    explicit MDnsServiceProxy(const sptr<IRemoteObject> &impl);
    virtual ~MDnsServiceProxy();

    int32_t RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb);
    int32_t UnRegisterService(const sptr<IRegistrationCallback> &cb);

    int32_t StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb);
    int32_t StopDiscoverService(const sptr<IDiscoveryCallback> &cb);

    int32_t ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb);

private:
    static inline BrokerDelegator<MDnsServiceProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // MDNS_SERVICE_PROXY_H