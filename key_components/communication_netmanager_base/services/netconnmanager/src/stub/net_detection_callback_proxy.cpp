/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "net_detection_callback_proxy.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetDetectionCallbackProxy::NetDetectionCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetDetectionCallback>(impl)
{}

NetDetectionCallbackProxy::~NetDetectionCallbackProxy() {}

int32_t NetDetectionCallbackProxy::OnNetDetectionResultChanged(NetDetectionResultCode detectionResult,
    const std::string &urlRedirect)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(urlRedirect)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteInt32(static_cast<int32_t>(detectionResult))) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(NET_DETECTION_RESULT, dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }

    return replyParcel.ReadInt32();
}

bool NetDetectionCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetDetectionCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
