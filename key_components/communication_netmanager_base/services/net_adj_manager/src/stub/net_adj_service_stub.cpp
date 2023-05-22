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

#include "net_adj_service_stub.h"

namespace OHOS {
namespace NetManagerStandard {
NetAdjServiceStub::NetAdjServiceStub()
{
    memberFuncMap_[CMD_NM_ADJ_SYSTEM_READY] = &NetAdjServiceStub::OnSystemReady;
    memberFuncMap_[CMD_NM_ADJ_ADD_IFACE] = &NetAdjServiceStub::OnAddNetAdjIface;
    memberFuncMap_[CMD_NM_ADJ_REMOVE_ADJ_IFACE] = &NetAdjServiceStub::OnRemoveNetAdjIface;
    memberFuncMap_[CMD_NM_ADJ_REGISTER_ADJ_CALLBACK] = &NetAdjServiceStub::OnRegisterAdjIfaceCallback;
    memberFuncMap_[CMD_NM_ADJ_UNREGISTER_ADJ_CALLBACK] = &NetAdjServiceStub::OnUnregisterAdjIfaceCallback;
    memberFuncMap_[CMD_NM_ADJ_UPDATE_ADJ_INFO] = &NetAdjServiceStub::OnUpdateNetAdjInfo;
}

NetAdjServiceStub::~NetAdjServiceStub()
{
    memberFuncMap_.clear();
}

int32_t NetAdjServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                           MessageOption &option)
{
    std::u16string descriptor = NetAdjServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_FLATTEN_OBJECT;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetAdjServiceStub::OnSystemReady(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int32_t NetAdjServiceStub::OnAddNetAdjIface(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int32_t NetAdjServiceStub::OnRemoveNetAdjIface(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int32_t NetAdjServiceStub::OnRegisterAdjIfaceCallback(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int32_t NetAdjServiceStub::OnUnregisterAdjIfaceCallback(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int32_t NetAdjServiceStub::OnUpdateNetAdjInfo(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}
} // namespace NetManagerStandard
} // namespace OHOS