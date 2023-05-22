/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_I_MDNS_EVENT_H
#define OHOS_I_MDNS_EVENT_H

#include <cstdint>

#include "iremote_broker.h"

#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {
class IRegistrationCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.IRegistrationCallback");
    virtual void HandleRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    virtual void HandleUnRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    virtual void HandleRegisterResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    enum class Message {
        REGISTERED,
        UNREGISTERED,
        RESULT,
    };
};

class IDiscoveryCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.IDiscoveryCallback");
    virtual void HandleStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    virtual void HandleStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    virtual void HandleServiceFound(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    virtual void HandleServiceLost(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    enum class Message {
        STARTED,
        STOPPED,
        FOUND,
        LOST,
    };
};

class IResolveCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.IResolveCallback");
    virtual void HandleResolveResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) = 0;
    enum class Message {
        RESULT,
    };
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // OHOS_I_MDNS_EVENT_H