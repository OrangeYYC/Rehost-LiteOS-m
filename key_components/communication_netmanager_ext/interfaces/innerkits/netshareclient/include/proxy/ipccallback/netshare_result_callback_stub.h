/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NETSHARE_RESULT_CALLBACK_STUB_H
#define NETSHARE_RESULT_CALLBACK_STUB_H

#include <string>

#include "i_netshare_result_callback.h"
#include "iremote_stub.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareResultCallbackStub : public IRemoteStub<INetShareResultCallback> {
public:
    NetShareResultCallbackStub() = default;
    virtual ~NetShareResultCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(NetShareResultCallbackStub);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETSHARE_RESULT_CALLBACK_STUB_H
