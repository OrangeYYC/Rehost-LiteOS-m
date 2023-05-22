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

#ifndef MDNS_MANDGER_H
#define MDNS_MANDGER_H

#include <cstdint>
#include <map>
#include <string>

#include "parcel.h"

#include "i_mdns_event.h"
#include "mdns_common.h"
#include "mdns_protocol_impl.h"
#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {

class MDnsManager {
public:
    MDnsManager();
    ~MDnsManager() = default;
    int32_t RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb);
    int32_t UnRegisterService(const sptr<IRegistrationCallback> &cb);

    int32_t StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb);
    int32_t StopDiscoverService(const sptr<IDiscoveryCallback> &cb);

    int32_t ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb);

    void GetDumpMessage(std::string &message);

private:
    void InitHandler();
    void ReceiveResult(const MDnsProtocolImpl::Result &result, int32_t error);
    MDnsServiceInfo ConvertResultToInfo(const MDnsProtocolImpl::Result &result);
    void ReceiveRegister(const MDnsProtocolImpl::Result &result, int32_t error);
    void ReceiveDiscover(const MDnsProtocolImpl::Result &result, int32_t error);
    void ReceiveInstanceResolve(const MDnsProtocolImpl::Result &result, int32_t error);

    struct CompareSmartPointer {
        bool operator()(const sptr<IRemoteBroker> &lhs, const sptr<IRemoteBroker> &rhs) const
        {
            return lhs->AsObject().GetRefPtr() < rhs->AsObject().GetRefPtr();
        }
    };

    MDnsProtocolImpl impl;
    std::vector<std::pair<sptr<IRegistrationCallback>, std::string>> registerMap_;
    std::vector<std::pair<sptr<IDiscoveryCallback>, std::string>> discoveryMap_;
    std::vector<std::pair<sptr<IResolveCallback>, std::string>> resolveMap_;
    std::mutex mutex_;
};

} // namespace NetManagerStandard
} // namespace OHOS

#endif