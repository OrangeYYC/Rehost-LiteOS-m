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

#ifndef SHARE_OBSERVER_WRAPPER_H
#define SHARE_OBSERVER_WRAPPER_H

#include <initializer_list>
#include <string_view>

#include <napi/native_api.h>

#include "event_manager.h"
#include "netshare_callback_observer.h"
#include "refbase.h"
#include "singleton.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareObserverWrapper final {
    DECLARE_DELAYED_SINGLETON(NetShareObserverWrapper);

public:
    napi_value On(napi_env env, napi_callback_info info, const std::initializer_list<std::string_view> &events,
                  bool asyncCallback);
    napi_value Off(napi_env env, napi_callback_info info, const std::initializer_list<std::string_view> &events,
                   bool asyncCallback);
    EventManager *GetEventManager() const;

private:
    sptr<NetShareCallbackObserver> observer_ = nullptr;
    EventManager *manager_ = nullptr;
    bool registed_ = false;

private:
    bool Register();
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // SHARE_OBSERVER_WRAPPER_H
