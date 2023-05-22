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
#ifndef NOTIFY_CALLBACK_STUB_H
#define NOTIFY_CALLBACK_STUB_H
#include <map>

#include "iremote_stub.h"

#include "i_notify_callback.h"

namespace OHOS {
namespace NetsysNative {
class NotifyCallbackStub : public IRemoteStub<INotifyCallback> {
public:
    NotifyCallbackStub();
    virtual ~NotifyCallbackStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using NotifyCallbackFunc = int32_t (NotifyCallbackStub::*)(MessageParcel &, MessageParcel &);

private:
    int32_t CmdOnInterfaceAddressUpdated(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnInterfaceAddressRemoved(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnInterfaceAdded(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnInterfaceRemoved(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnInterfaceChanged(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnInterfaceLinkStateChanged(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnRouteChanged(MessageParcel &data, MessageParcel &reply);
    int32_t CmdDhcpSuccess(MessageParcel &data, MessageParcel &reply);
    int32_t CmdOnBandwidthReachedLimit(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NotifyCallbackFunc> memberFuncMap_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NOTIFY_CALLBACK_STUB_H
