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

#ifndef MDNS_EVENT_STUB_H
#define MDNS_EVENT_STUB_H

#include <cstdint>
#include <map>

#include "iremote_stub.h"

#include "i_mdns_event.h"

namespace OHOS {
namespace NetManagerStandard {
class RegistrationCallbackStub : public IRemoteStub<IRegistrationCallback> {
public:
    RegistrationCallbackStub() = default;
    virtual ~RegistrationCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class DiscoveryCallbackStub : public IRemoteStub<IDiscoveryCallback> {
public:
    DiscoveryCallbackStub() = default;
    virtual ~DiscoveryCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class ResolveCallbackStub : public IRemoteStub<IResolveCallback> {
public:
    ResolveCallbackStub() = default;
    virtual ~ResolveCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // MDNS_EVENT_STUB_H
