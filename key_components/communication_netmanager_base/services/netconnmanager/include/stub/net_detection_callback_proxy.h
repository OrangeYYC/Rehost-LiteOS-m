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

#ifndef NET_DETECTION_CALLBACK_PROXY_H
#define NET_DETECTION_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "i_net_detection_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetDetectionCallbackProxy : public IRemoteProxy<INetDetectionCallback> {
public:
    explicit NetDetectionCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetDetectionCallbackProxy();

public:
    int32_t OnNetDetectionResultChanged(
        NetDetectionResultCode detectionResult, const std::string &urlRedirect) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<NetDetectionCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DETECTION_CALLBACK_PROXY_H
