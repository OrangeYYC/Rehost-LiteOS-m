/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "net_detection_callback_stub.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetDetectionCallbackStub::NetDetectionCallbackStub()
{
    memberFuncMap_[NET_DETECTION_RESULT] = &NetDetectionCallbackStub::OnNetDetectionResult;
}

NetDetectionCallbackStub::~NetDetectionCallbackStub() {}

int32_t NetDetectionCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    NETMGR_LOG_D("Stub call start, code:[%{public}d]", code);
    std::u16string myDescripter = NetDetectionCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        NETMGR_LOG_E("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
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

int32_t NetDetectionCallbackStub::OnNetDetectionResult(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_E("Execute ContainFileDescriptors failed");
    }
    std::string urlRedirect;
    if (!data.ReadString(urlRedirect)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t netDetectionResult = 0;
    if (!data.ReadInt32(netDetectionResult)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = OnNetDetectionResultChanged(static_cast<NetDetectionResultCode>(netDetectionResult), urlRedirect);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write parcel failed");
        return ret;
    }

    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
