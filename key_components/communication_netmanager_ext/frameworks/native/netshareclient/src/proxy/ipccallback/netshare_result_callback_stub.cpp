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

#include "netshare_result_callback_stub.h"

#include "string_ex.h"
#include "net_manager_constants.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t NetShareResultCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                    MessageOption &option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != INetShareResultCallback::GetDescriptor()) {
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }
    INetShareResultCallback::Message msgCode = static_cast<INetShareResultCallback::Message>(code);
    if (msgCode == INetShareResultCallback::Message::RESULT) {
        int32_t status = data.ReadInt32();
        OnResult(status);
        return NETMANAGER_EXT_SUCCESS;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace NetManagerStandard
} // namespace OHOS
