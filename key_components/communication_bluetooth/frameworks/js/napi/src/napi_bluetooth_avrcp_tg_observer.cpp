/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include <uv.h>
#include "bluetooth_utils.h"
#include "napi_bluetooth_avrcp_tg_observer.h"
#include "napi_bluetooth_event.h"

namespace OHOS {
namespace Bluetooth {

std::shared_mutex NapiAvrcpTargetObserver::g_avrcpTgCallbackInfosMutex;
void NapiAvrcpTargetObserver::OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
{
    HILOGI("enter, remote device address: %{public}s, state: %{public}d", GET_ENCRYPT_ADDR(device), state);
    std::unique_lock<std::shared_mutex> guard(g_avrcpTgCallbackInfosMutex);

    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>>::iterator it =
        callbackInfos_.find(STR_BT_AVRCP_TG_CONNECTION_STATE_CHANGE);
    if (it == callbackInfos_.end() || it->second == nullptr) {
        HILOGW("This callback is not registered by ability.");
        return;
    }
    HILOGI("%{public}s is registered by ability", STR_BT_AVRCP_TG_CONNECTION_STATE_CHANGE.c_str());
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = it->second;

    callbackInfo->state_ = state;
    callbackInfo->deviceId_ = device.GetDeviceAddr();
    NapiEvent::CheckAndNotify(callbackInfo, state);
}

} // namespace Bluetooth
} // namespace OHOS