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

#ifndef MDNS_SERVICE_STUB_H
#define MDNS_SERVICE_STUB_H

#include <cstdint>
#include <map>

#include "iremote_stub.h"

#include "i_mdns_service.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsServiceStub : public IRemoteStub<IMDnsService> {
    using MDnsServiceFunc = int32_t (MDnsServiceStub::*)(MessageParcel &, MessageParcel &);

public:
    MDnsServiceStub();
    ~MDnsServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t OnRegisterService(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnRegisterService(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartDiscoverService(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopDiscoverService(MessageParcel &data, MessageParcel &reply);
    int32_t OnResolveService(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, MDnsServiceFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // MDNS_SERVICE_STUB_H
