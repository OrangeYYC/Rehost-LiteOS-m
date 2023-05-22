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

#ifndef NETSHARE_RESULT_CALLBACK_PROXY_H
#define NETSHARE_RESULT_CALLBACK_PROXY_H

#include <string>

#include "i_netshare_result_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareResultCallbackProxy : public IRemoteProxy<INetShareResultCallback> {
public:
    explicit NetShareResultCallbackProxy(const sptr<IRemoteObject> &object);
    virtual ~NetShareResultCallbackProxy() = default;
    virtual void OnResult(const int32_t &result) override;

private:
    static inline BrokerDelegator<NetShareResultCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NETSHARE_RESULT_CALLBACK_PROXY_H
