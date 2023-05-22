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

#include "wrapper_distributor.h"

#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard::CommonUtils;
namespace {
bool IsValidMessage(const std::shared_ptr<NetsysEventMessage> &message)
{
    return message->GetAction() != NetsysEventMessage::Action::UNKNOWN &&
           message->GetSubSys() != NetsysEventMessage::SubSys::UNKNOWN;
}
} // namespace
WrapperDistributor::WrapperDistributor(int32_t socket, const int32_t format)
{
    NETNATIVE_LOG_D("WrapperDistributor::WrapperDistributor: Socket: %{public}d, Format: %{public}d", socket, format);
    receiver_ = std::make_unique<DataReceiver>(socket, format);
    receiver_->RegisterCallback(
        [this](const std::shared_ptr<NetsysEventMessage> message) { HandleDecodeSuccess(message); });
}

int32_t WrapperDistributor::Start()
{
    return receiver_->Start();
}

int32_t WrapperDistributor::Stop()
{
    return receiver_->Stop();
}

int32_t WrapperDistributor::RegisterNetlinkCallbacks(
    std::shared_ptr<std::vector<sptr<NetsysNative::INotifyCallback>>> netlinkCallbacks)
{
    if (netlinkCallbacks == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks is nullptr");
        return NetlinkResult::ERR_NULL_PTR;
    }
    netlinkCallbacks_ = netlinkCallbacks;
    return NetlinkResult::OK;
}

void WrapperDistributor::HandleDecodeSuccess(const std::shared_ptr<NetsysEventMessage> &message)
{
    if (message == nullptr) {
        NETNATIVE_LOGE("NetlinkProcessor: OnEvent: message is nullptr");
        return;
    }
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    if (!IsValidMessage(message)) {
        return;
    }
    HandleStateChanged(message);
}

void WrapperDistributor::HandleStateChanged(const std::shared_ptr<NetsysEventMessage> &message)
{
    message->DumpMessage();
    const NetsysEventMessage::SubSys subSys = message->GetSubSys();
    switch (subSys) {
        case NetsysEventMessage::SubSys::NET:
            HandleSubSysNet(message);
            break;
        case NetsysEventMessage::SubSys::QLOG:
            HandleSubSysQlog(message);
            break;
        default:
            break;
    }
}

void WrapperDistributor::HandleSubSysNet(const std::shared_ptr<NetsysEventMessage> &message)
{
    NetsysEventMessage::Action action = message->GetAction();
    const std::string &iface = message->GetMessage(NetsysEventMessage::Type::INTERFACE);

    switch (action) {
        case NetsysEventMessage::Action::ADD:
            NotifyInterfaceAdd(iface);
            break;
        case NetsysEventMessage::Action::REMOVE:
            NotifyInterfaceRemove(iface);
            break;
        case NetsysEventMessage::Action::CHANGE:
            NotifyInterfaceChange(iface, true);
            break;
        case NetsysEventMessage::Action::LINKUP:
            NotifyInterfaceLinkStateChange(iface, true);
            break;
        case NetsysEventMessage::Action::LINKDOWN:
            NotifyInterfaceLinkStateChange(iface, false);
            break;
        case NetsysEventMessage::Action::ADDRESSUPDATE:
        case NetsysEventMessage::Action::ADDRESSREMOVED:
            HandleAddressChange(message);
            break;
        case NetsysEventMessage::Action::ROUTEUPDATED:
        case NetsysEventMessage::Action::ROUTEREMOVED:
            HandleRouteChange(message);
            break;
        default:
            break;
    }
}

void WrapperDistributor::HandleAddressChange(const std::shared_ptr<NetsysEventMessage> &message)
{
    NetsysEventMessage::Action action = message->GetAction();
    const std::string &iface = message->GetMessage(NetsysEventMessage::Type::INTERFACE);
    const std::string &address = message->GetMessage(NetsysEventMessage::Type::ADDRESS);
    const std::string &flags = message->GetMessage(NetsysEventMessage::Type::FLAGS);
    const std::string &scope = message->GetMessage(NetsysEventMessage::Type::SCOPE);
    const bool addrUpdated = (action == NetsysEventMessage::Action::ADDRESSUPDATE);

    if (!iface.empty() && iface[0] && !address.empty() && !flags.empty() && !scope.empty()) {
        if (addrUpdated) {
            NotifyInterfaceAddressUpdate(address, iface, ConvertToInt64(flags), ConvertToInt64(scope));
        } else {
            NotifyInterfaceAddressRemove(address, iface, ConvertToInt64(flags), ConvertToInt64(scope));
        }
    }
}

void WrapperDistributor::HandleRouteChange(const std::shared_ptr<NetsysEventMessage> &message)
{
    NetsysEventMessage::Action action = message->GetAction();
    const std::string &route = message->GetMessage(NetsysEventMessage::Type::ROUTE);
    const std::string &gateway = message->GetMessage(NetsysEventMessage::Type::GATEWAY);
    const std::string &iface = message->GetMessage(NetsysEventMessage::Type::INTERFACE);
    if (!route.empty() && (!gateway.empty() || !iface.empty())) {
        NotifyRouteChange((action == NetsysEventMessage::Action::ROUTEUPDATED), route, gateway, iface);
    }
}

void WrapperDistributor::HandleSubSysQlog(const std::shared_ptr<NetsysEventMessage> &message)
{
    const std::string &alertName = message->GetMessage(NetsysEventMessage::Type::ALERT_NAME);
    const std::string &iface = message->GetMessage(NetsysEventMessage::Type::INTERFACE);
    if (iface.empty()) {
        NETNATIVE_LOGW("No interface name in event message");
        return;
    }
    NotifyQuotaLimitReache(alertName, iface);
}

void WrapperDistributor::NotifyInterfaceAdd(const std::string &ifName)
{
    NETNATIVE_LOG_D("interface added: %{public}s", ifName.c_str());
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceAdded(ifName);
    }
}

void WrapperDistributor::NotifyInterfaceRemove(const std::string &ifName)
{
    NETNATIVE_LOG_D("interface removed: %{public}s", ifName.c_str());
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceRemoved(ifName);
    }
}

void WrapperDistributor::NotifyInterfaceChange(const std::string &ifName, bool up)
{
    NETNATIVE_LOG_D("interface Change: %{public}s, %{public}d", ifName.c_str(), up);
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceChanged(ifName, up);
    }
}

void WrapperDistributor::NotifyInterfaceLinkStateChange(const std::string &ifName, bool up)
{
    NETNATIVE_LOG_D("interface link state Change: %{public}s, %{public}d", ifName.c_str(), up);
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceLinkStateChanged(ifName, up);
    }
}

void WrapperDistributor::NotifyQuotaLimitReache(const std::string &labelName, const std::string &ifName)
{
    NETNATIVE_LOG_D("NotifyQuotaLimitReache: %{public}s, %{public}s", labelName.c_str(), ifName.c_str());
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnBandwidthReachedLimit(labelName, ifName);
    }
}

void WrapperDistributor::NotifyInterfaceAddressUpdate(const std::string &addr, const std::string &ifName,
                                                      int32_t flags, int32_t scope)
{
    NETNATIVE_LOG_D("OnInterfaceAddressUpdated: %{public}s, %{public}s, %{public}d, %{public}d",
                    ToAnonymousIp(addr).c_str(), ifName.c_str(), flags, scope);
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceAddressUpdated(addr, ifName, flags, scope);
    }
}

void WrapperDistributor::NotifyInterfaceAddressRemove(const std::string &addr, const std::string &ifName,
                                                      int32_t flags, int32_t scope)
{
    NETNATIVE_LOG_D("NotifyInterfaceAddressRemove: %{public}s, %{public}s, %{public}d, %{public}d",
                    ToAnonymousIp(addr).c_str(), ifName.c_str(), flags, scope);
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnInterfaceAddressRemoved(addr, ifName, flags, scope);
    }
}

void WrapperDistributor::NotifyRouteChange(bool updated, const std::string &route, const std::string &gateway,
                                           const std::string &ifName)
{
    NETNATIVE_LOG_D("NotifyRouteChange: %{public}s, %{public}s, %{public}s, %{public}s",
                    updated ? "updated" : "removed", ToAnonymousIp(route).c_str(), ToAnonymousIp(gateway).c_str(),
                    ifName.c_str());
    if (netlinkCallbacks_ == nullptr) {
        NETNATIVE_LOGE("netlinkCallbacks_ is nullptr");
        return;
    }
    for (auto &callback : *netlinkCallbacks_) {
        callback->OnRouteChanged(updated, route, gateway, ifName);
    }
}
} // namespace nmd
} // namespace OHOS
