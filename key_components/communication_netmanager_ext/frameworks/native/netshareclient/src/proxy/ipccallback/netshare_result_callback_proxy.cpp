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

#include "netshare_result_callback_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetShareResultCallbackProxy::NetShareResultCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<INetShareResultCallback>(object)
{
}

void NetShareResultCallbackProxy::OnResult(const int32_t &result)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("NetShareResultCallbackProxy get Remote() error.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(INetShareResultCallback::GetDescriptor());

    if (!data.WriteInt32(result)) {
        NETMGR_EXT_LOG_E("NetShareResultCallbackProxy WriteInt32 error.");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<int32_t>(INetShareResultCallback::Message::RESULT), data, reply, option);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("NetShareResultCallbackProxy SendRequest error=[%{public}d].", ret);
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
