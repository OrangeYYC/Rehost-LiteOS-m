/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "bluetooth_errorcode.h"
#include "bluetooth_pan_server.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "interface_profile.h"
#include "interface_profile_pan.h"
#include "i_bluetooth_host_observer.h"
#include "permission_utils.h"
#include "remote_observer_list.h"
#include "hilog/log.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPanCallback : public bluetooth::IPanObserver {
public:
    BluetoothPanCallback() = default;
    ~BluetoothPanCallback() override = default;

    void OnConnectionStateChanged(const RawAddress &device, int state) override
    {
        HILOGI("addr:%{public}s, state:%{public}d", GET_ENCRYPT_ADDR(device), state);
        observers_->ForEach([device, state](sptr<IBluetoothPanObserver> observer) {
            observer->OnConnectionStateChanged(device, state);
        });
    }

    void SetObserver(RemoteObserverList<IBluetoothPanObserver> *observers)
    {
        observers_ = observers;
    }

private:
    RemoteObserverList<IBluetoothPanObserver> *observers_;
};

struct BluetoothPanServer::impl {
    impl();
    ~impl();

    /// sys state observer
    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothPanObserver> observers_;
    std::unique_ptr<BluetoothPanCallback> observerImp_ = std::make_unique<BluetoothPanCallback>();
    IProfilePan *panService_ = nullptr;
    std::vector<sptr<IBluetoothPanObserver>> advCallBack_;

    IProfilePan *GetServicePtr()
    {
        if (IProfileManager::GetInstance() == nullptr) {
            return nullptr;
        }
        return static_cast<IProfilePan *>(
            IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_PAN));
    }
};

class BluetoothPanServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    explicit SystemStateObserver(BluetoothPanServer::impl *pimpl) : pimpl_(pimpl) {}
    void OnSystemStateChange(const BTSystemState state) override
    {
        switch (state) {
            case BTSystemState::ON:
                pimpl_->panService_ = pimpl_->GetServicePtr();
                if (pimpl_->panService_ != nullptr) {
                    pimpl_->panService_->RegisterObserver(*pimpl_->observerImp_.get());
                }
                break;
            case BTSystemState::OFF:
                pimpl_->panService_ = nullptr;
                break;
            default:
                break;
        }
    };

private:
    BluetoothPanServer::impl *pimpl_ = nullptr;
};

BluetoothPanServer::impl::impl()
{
    HILOGI("starts");
}

BluetoothPanServer::impl::~impl()
{
    HILOGI("starts");
}

BluetoothPanServer::BluetoothPanServer()
{
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    pimpl->panService_ = pimpl->GetServicePtr();
    if (pimpl->panService_ != nullptr) {
        pimpl->panService_->RegisterObserver(*pimpl->observerImp_.get());
    }
}

BluetoothPanServer::~BluetoothPanServer()
{
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
    if (pimpl->panService_ != nullptr) {
        pimpl->panService_->DeregisterObserver(*pimpl->observerImp_.get());
    }
}

ErrCode BluetoothPanServer::RegisterObserver(const sptr<IBluetoothPanObserver> observer)
{
    HILOGI("enter");

    if (observer == nullptr) {
        HILOGE("observer is null");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr) {
        HILOGE("pimpl is null");
        return ERR_NO_INIT;
    }

    pimpl->observers_.Register(observer);
    pimpl->advCallBack_.push_back(observer);
    return ERR_OK;
}

ErrCode BluetoothPanServer::DeregisterObserver(const sptr<IBluetoothPanObserver> observer)
{
    HILOGI("enter");
    if (observer == nullptr) {
        HILOGE("observer is null");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr) {
        HILOGE("pimpl is null");
        return ERR_NO_INIT;
    }
    for (auto iter = pimpl->advCallBack_.begin(); iter != pimpl->advCallBack_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            if (pimpl != nullptr) {
                pimpl->observers_.Deregister(*iter);
                pimpl->advCallBack_.erase(iter);
                break;
            }
        }
    }
    pimpl->panService_->DeregisterObserver(*pimpl->observerImp_.get());
    return ERR_OK;
}

int32_t BluetoothPanServer::GetDevicesByStates(const std::vector<int32_t> &states,
    std::vector<BluetoothRawAddress>& result)
{
    HILOGI("enter");
    if (pimpl == nullptr || pimpl->panService_ == nullptr) {
        HILOGI("not init.");
        return BT_ERR_INTERNAL_ERROR;
    }

    std::vector<bluetooth::RawAddress> serviceDeviceList = pimpl->panService_->GetDevicesByStates(states);
    for (auto &device : serviceDeviceList) {
        BluetoothRawAddress bluetoothDevice(device.GetAddress());
        result.push_back(bluetoothDevice);
    }
    return BT_SUCCESS;
}

int32_t BluetoothPanServer::GetDeviceState(const BluetoothRawAddress &device, int32_t &state)
{
    if (pimpl == nullptr || pimpl->panService_ == nullptr) {
        HILOGI("not init.");
        return BT_ERR_INTERNAL_ERROR;
    }
    state = pimpl->panService_->GetDeviceState(device);
    HILOGI("addr:%{public}s, res:%{public}d", GET_ENCRYPT_ADDR(device), state);
    return BT_SUCCESS;
}

int32_t BluetoothPanServer::Disconnect(const BluetoothRawAddress &device)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_ADDR(device));
    if (!PermissionUtils::CheckSystemHapApp()) {
        HILOGE("check system api failed.");
        return BT_ERR_SYSTEM_PERMISSION_FAILED;
    }
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return BT_ERR_PERMISSION_FAILED;
    }
    if (pimpl == nullptr || pimpl->panService_ == nullptr) {
        HILOGI("not init.");
        return BT_ERR_INTERNAL_ERROR;
    }
    return pimpl->panService_->Disconnect(device);
}

int32_t BluetoothPanServer::SetTethering(const bool enable)
{
    HILOGI("enable:%{public}d", enable);
    if (!PermissionUtils::CheckSystemHapApp()) {
        HILOGE("check system api failed.");
        return BT_ERR_SYSTEM_PERMISSION_FAILED;
    }
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return BT_ERR_PERMISSION_FAILED;
    }
    if (pimpl == nullptr || pimpl->panService_ == nullptr) {
        HILOGI("not init.");
        return BT_ERR_INTERNAL_ERROR;
    }
    return pimpl->panService_->SetTethering(enable);
}

int32_t BluetoothPanServer::IsTetheringOn(bool& result)
{
    if (!PermissionUtils::CheckSystemHapApp()) {
        HILOGE("check system api failed.");
        return BT_ERR_SYSTEM_PERMISSION_FAILED;
    }
    if (pimpl == nullptr || pimpl->panService_ == nullptr) {
        HILOGI("not init.");
        return BT_ERR_INTERNAL_ERROR;
    }
    result = pimpl->panService_->IsTetheringOn();
    HILOGI("IsTetheringOn:%{public}d", result);
    return BT_SUCCESS;
}
}  // namespace Bluetooth
}  // namespace OHOS
