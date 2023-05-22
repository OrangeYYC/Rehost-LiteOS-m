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

#include "sharing_event_callback_stub.h"

#include "string_ex.h"
#include "net_manager_constants.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t SharingEventCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                  MessageOption &option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != ISharingEventCallback::GetDescriptor()) {
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }
    ISharingEventCallback::Message msgCode = static_cast<ISharingEventCallback::Message>(code);
    switch (msgCode) {
        case ISharingEventCallback::Message::GLOBAL_SHARING_STATE_CHANGED: {
            bool isRunning = data.ReadBool();
            OnSharingStateChanged(isRunning);
            break;
        }
        case ISharingEventCallback::Message::INTERFACE_SHARING_STATE_CHANGED: {
            int32_t tmpInt = data.ReadInt32();
            SharingIfaceType type = static_cast<SharingIfaceType>(tmpInt);
            std::string iface = data.ReadString();
            tmpInt = data.ReadInt32();
            SharingIfaceState state = static_cast<SharingIfaceState>(tmpInt);
            OnInterfaceSharingStateChanged(type, iface, state);
            break;
        }
        case ISharingEventCallback::Message::SHARING_UPSTREAM_CHANGED: {
            int32_t netId = 0;
            if (!data.ReadInt32(netId)) {
                return IPC_PROXY_ERR;
            }
            sptr<NetHandle> netHandle = new NetHandle(netId);
            OnSharingUpstreamChanged(netHandle);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NETMANAGER_EXT_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
