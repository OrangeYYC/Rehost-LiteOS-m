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

#ifndef NETMANAGER_BASE_NETWORK_EXEC_H
#define NETMANAGER_BASE_NETWORK_EXEC_H

#include <netdb.h>

#include "gettype_context.h"
#include "subscribe_context.h"
#include "unsubscribe_context.h"

#include "napi/native_api.h"
#include "nocopyable.h"

namespace OHOS::NetManagerStandard {
class NetworkExec final {
public:
    DISALLOW_COPY_AND_MOVE(NetworkExec);

    NetworkExec() = delete;

    ~NetworkExec() = delete;

    static bool ExecGetType(GetTypeContext *context);

    static napi_value GetTypeCallback(GetTypeContext *context);

    static bool ExecSubscribe(SubscribeContext *context);

    static napi_value SubscribeCallback(SubscribeContext *context);

    static bool ExecUnsubscribe(UnsubscribeContext *context);

    static napi_value UnsubscribeCallback(UnsubscribeContext *context);
};
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_NETWORK_EXEC_H */
