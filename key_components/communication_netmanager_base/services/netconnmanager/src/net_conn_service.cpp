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

#include <sys/time.h>

#include "common_event_support.h"
#include "system_ability_definition.h"

#include "broadcast_manager.h"
#include "event_report.h"
#include "net_activate.h"
#include "net_conn_service.h"
#include "net_conn_types.h"
#include "net_manager_center.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_supplier.h"
#include "netmanager_base_permission.h"
#include "netsys_controller.h"
#include "net_http_proxy_tracker.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
// hisysevent error messgae
constexpr const char *ERROR_MSG_NULL_SUPPLIER_INFO = "Net supplier info is nullptr";
constexpr const char *ERROR_MSG_NULL_NET_LINK_INFO = "Net link info is nullptr";
constexpr const char *ERROR_MSG_NULL_NET_SPECIFIER = "The parameter of netSpecifier or callback is null";
constexpr const char *ERROR_MSG_CAN_NOT_FIND_SUPPLIER = "Can not find supplier by id:";
constexpr const char *ERROR_MSG_UPDATE_NETLINK_INFO_FAILED = "Update net link info failed";
constexpr const char *NET_CONN_MANAGER_WORK_THREAD = "NET_CONN_MANAGER_WORK_THREAD";
} // namespace

const bool REGISTER_LOCAL_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<NetConnService>::GetInstance().get());

NetConnService::NetConnService()
    : SystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, true), registerToService_(false), state_(STATE_STOPPED)
{
    CreateDefaultRequest();
}

NetConnService::~NetConnService() {}

void NetConnService::OnStart()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    NETMGR_LOG_D("NetConnService::OnStart begin");
    if (state_ == STATE_RUNNING) {
        NETMGR_LOG_D("the state is already running");
        return;
    }
    if (!Init()) {
        NETMGR_LOG_E("init failed");
        return;
    }
    state_ = STATE_RUNNING;
    gettimeofday(&tv, nullptr);
    NETMGR_LOG_D("NetConnService::OnStart end");
}

void NetConnService::CreateDefaultRequest()
{
    if (!defaultNetActivate_) {
        defaultNetSpecifier_ = (std::make_unique<NetSpecifier>()).release();
        defaultNetSpecifier_->SetCapability(NET_CAPABILITY_INTERNET);
        std::weak_ptr<INetActivateCallback> timeoutCb;
        defaultNetActivate_ = new (std::nothrow) NetActivate(defaultNetSpecifier_, nullptr, timeoutCb, 0);
        defaultNetActivate_->SetRequestId(DEFAULT_REQUEST_ID);
        netActivates_[DEFAULT_REQUEST_ID] = defaultNetActivate_;
    }
}

void NetConnService::OnStop()
{
    NETMGR_LOG_D("NetConnService::OnStop begin");
    if (netConnEventRunner_) {
        netConnEventRunner_->Stop();
        netConnEventRunner_.reset();
    }
    if (netConnEventHandler_) {
        netConnEventHandler_.reset();
    }
    state_ = STATE_STOPPED;
    registerToService_ = false;
    NETMGR_LOG_D("NetConnService::OnStop end");
}

bool NetConnService::Init()
{
    if (!REGISTER_LOCAL_RESULT) {
        NETMGR_LOG_E("Register to local sa manager failed");
        registerToService_ = false;
        return false;
    }
    if (!registerToService_) {
        if (!Publish(DelayedSingleton<NetConnService>::GetInstance().get())) {
            NETMGR_LOG_E("Register to sa manager failed");
            return false;
        }
        registerToService_ = true;
    }
    netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    if (netConnEventRunner_ == nullptr) {
        NETMGR_LOG_E("Create event runner failed.");
        return false;
    }
    netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnEventRunner_);
    serviceIface_ = std::make_unique<NetConnServiceIface>().release();
    NetManagerCenter::GetInstance().RegisterConnService(serviceIface_);
    netScore_ = std::make_unique<NetScore>();
    if (netScore_ == nullptr) {
        NETMGR_LOG_E("Make NetScore failed");
        return false;
    }
    NetHttpProxyTracker httpProxyTracker;
    if (!httpProxyTracker.ReadFromSystemParameter(httpProxy_)) {
        NETMGR_LOG_E("NetConnService Init: read http proxy failed");
    }
    SendGlobalHttpProxyChangeBroadcast();
    return true;
}

int32_t NetConnService::SystemReady()
{
    NETMGR_LOG_D("System ready.");
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                            const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, bearerType, &ident, &netCaps, &supplierId, &result]() {
            result = this->RegisterNetSupplierAsync(bearerType, ident, netCaps, supplierId);
        });
    }
    return result;
}

int32_t NetConnService::RegisterNetSupplierCallback(uint32_t supplierId, const sptr<INetSupplierCallback> &callback)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, supplierId, &callback, &result]() {
            result = this->RegisterNetSupplierCallbackAsync(supplierId, callback);
        });
    }
    return result;
}

int32_t NetConnService::RegisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    NETMGR_LOG_D("RegisterNetConnCallback service in.");
    return RegisterNetConnCallback(defaultNetSpecifier_, callback, 0);
}

int32_t NetConnService::RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier,
                                                const sptr<INetConnCallback> &callback, const uint32_t &timeoutMS)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, &netSpecifier, &callback, timeoutMS, &result]() {
            result = this->RegisterNetConnCallbackAsync(netSpecifier, callback, timeoutMS);
        });
    }
    return result;
}

int32_t NetConnService::RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    NETMGR_LOG_D("Enter NetConnService::RegisterNetDetectionCallback");
    return RegUnRegNetDetectionCallback(netId, callback, true);
}

int32_t NetConnService::UnregisterNetSupplier(uint32_t supplierId)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask(
            [this, supplierId, &result]() { result = this->UnregisterNetSupplierAsync(supplierId); });
    }
    return result;
}

int32_t NetConnService::UnregisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask(
            [this, &callback, &result]() { result = this->UnregisterNetConnCallbackAsync(callback); });
    }
    return result;
}

int32_t NetConnService::UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    NETMGR_LOG_D("Enter NetConnService::UnRegisterNetDetectionCallback");
    return RegUnRegNetDetectionCallback(netId, callback, false);
}

int32_t NetConnService::RegUnRegNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback,
                                                     bool isReg)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, netId, &callback, isReg, &result]() {
            result = this->RegUnRegNetDetectionCallbackAsync(netId, callback, isReg);
        });
    }
    return result;
}

int32_t NetConnService::UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, &netSpecifier, netState, &result]() {
            result = this->UpdateNetStateForTestAsync(netSpecifier, netState);
        });
    }
    return result;
}

int32_t NetConnService::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, supplierId, &netSupplierInfo, &result]() {
            result = this->UpdateNetSupplierInfoAsync(supplierId, netSupplierInfo);
        });
    }
    return result;
}

int32_t NetConnService::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, supplierId, &netLinkInfo, &result]() {
            result = this->UpdateNetLinkInfoAsync(supplierId, netLinkInfo);
        });
    }
    return result;
}

int32_t NetConnService::NetDetection(int32_t netId)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, netId, &result]() { result = this->NetDetectionAsync(netId); });
    }
    return result;
}

int32_t NetConnService::RestrictBackgroundChanged(bool restrictBackground)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([this, restrictBackground, &result]() {
            result = this->RestrictBackgroundChangedAsync(restrictBackground);
        });
    }
    return result;
}

int32_t NetConnService::RegisterNetSupplierAsync(NetBearType bearerType, const std::string &ident,
                                                 const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    NETMGR_LOG_D("register supplier, netType[%{public}u], ident[%{public}s]", static_cast<uint32_t>(bearerType),
                 ident.c_str());
    // If there is no supplier in the list, create a supplier
    if (bearerType < BEARER_CELLULAR || bearerType >= BEARER_DEFAULT) {
        NETMGR_LOG_E("netType parameter invalid");
        return NET_CONN_ERR_NET_TYPE_NOT_FOUND;
    }
    sptr<NetSupplier> supplier = GetNetSupplierFromList(bearerType, ident, netCaps);
    if (supplier != nullptr) {
        NETMGR_LOG_D("supplier already exists.");
        supplierId = supplier->GetSupplierId();
        return NETMANAGER_SUCCESS;
    }
    // If there is no supplier in the list, create a supplier
    supplier = (std::make_unique<NetSupplier>(bearerType, ident, netCaps)).release();
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier is nullptr");
        return NET_CONN_ERR_NO_SUPPLIER;
    }
    supplierId = supplier->GetSupplierId();
    if (!netScore_->GetServiceScore(supplier)) {
        NETMGR_LOG_E("GetServiceScore fail.");
    }
    // create network
    int32_t netId = GenerateNetId();
    NETMGR_LOG_D("GenerateNetId is: [%{public}d]", netId);
    if (netId == INVALID_NET_ID) {
        NETMGR_LOG_E("GenerateNetId fail");
        return NET_CONN_ERR_INVALID_NETWORK;
    }
    std::shared_ptr<Network> network = std::make_shared<Network>(
        netId, supplierId,
        std::bind(&NetConnService::HandleDetectionResult, this, std::placeholders::_1, std::placeholders::_2),
        bearerType, netConnEventHandler_);
    NETMGR_LOG_D("Register supplier,supplierId[%{public}d] netId[%{public}d], ", supplierId, netId);
    supplier->SetNetwork(network);
    supplier->SetNetValid(true);
    // save supplier
    std::unique_lock<std::mutex> locker(netManagerMutex_);
    netSuppliers_[supplierId] = supplier;
    networks_[netId] = network;
    locker.unlock();

    struct EventInfo eventInfo = {.netId = netId, .bearerType = bearerType, .ident = ident, .supplierId = supplierId};
    EventReport::SendSupplierBehaviorEvent(eventInfo);
    NETMGR_LOG_D("RegisterNetSupplier service out. netSuppliers_ size[%{public}zd]", netSuppliers_.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::RegisterNetSupplierCallbackAsync(uint32_t supplierId,
                                                         const sptr<INetSupplierCallback> &callback)
{
    NETMGR_LOG_D("Register net supplier callback async");
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    auto supplier = FindNetSupplier(supplierId);
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier doesn't exist.");
        return NET_CONN_ERR_NO_SUPPLIER;
    }
    supplier->RegisterSupplierCallback(callback);
    SendAllRequestToNetwork(supplier);
    NETMGR_LOG_D("RegisterNetSupplierCallback service out.");
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::RegisterNetConnCallbackAsync(const sptr<NetSpecifier> &netSpecifier,
                                                     const sptr<INetConnCallback> &callback, const uint32_t &timeoutMS)
{
    NETMGR_LOG_D("Register net connect callback async");
    if (netSpecifier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is null");
        struct EventInfo eventInfo = {.errorType = static_cast<int32_t>(FAULT_INVALID_PARAMETER),
                                      .errorMsg = ERROR_MSG_NULL_NET_SPECIFIER};
        EventReport::SendRequestFaultEvent(eventInfo);
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    if (netActivates_.size() >= MAX_REQUEST_NUM) {
        NETMGR_LOG_E("Over the max request number");
        return NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM;
    }
    uint32_t reqId = 0;
    if (FindSameCallback(callback, reqId)) {
        NETMGR_LOG_E("RegisterNetConnCallback find same callback");
        return NET_CONN_ERR_CALLBACK_NOT_FOUND;
    }
    return ActivateNetwork(netSpecifier, callback, timeoutMS);
}

int32_t NetConnService::UnregisterNetSupplierAsync(uint32_t supplierId)
{
    NETMGR_LOG_D("UnregisterNetSupplier supplierId[%{public}d]", supplierId);
    // Remove supplier from the list based on supplierId
    auto supplier = FindNetSupplier(supplierId);
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier doesn't exist.");
        return NET_CONN_ERR_NO_SUPPLIER;
    }
    NETMGR_LOG_D("unregister supplier[%{public}d, %{public}s], defaultNetSupplier[%{public}d], %{public}s",
                 supplier->GetSupplierId(), supplier->GetNetSupplierIdent().c_str(),
                 defaultNetSupplier_ ? defaultNetSupplier_->GetSupplierId() : 0,
                 defaultNetSupplier_ ? defaultNetSupplier_->GetNetSupplierIdent().c_str() : "null");

    struct EventInfo eventInfo = {.bearerType = supplier->GetNetSupplierType(),
                                  .ident = supplier->GetNetSupplierIdent(),
                                  .supplierId = supplier->GetSupplierId()};
    EventReport::SendSupplierBehaviorEvent(eventInfo);

    int32_t netId = supplier->GetNetId();
    NET_NETWORK_MAP::iterator iterNetwork = networks_.find(netId);
    if (iterNetwork != networks_.end()) {
        std::unique_lock<std::mutex> locker(netManagerMutex_);
        networks_.erase(iterNetwork);
        locker.unlock();
    }
    if (defaultNetSupplier_ == supplier) {
        NETMGR_LOG_D("set defaultNetSupplier_ to null.");
        sptr<NetSupplier> newSupplier = nullptr;
        MakeDefaultNetWork(defaultNetSupplier_, newSupplier);
    }
    NetSupplierInfo info;
    supplier->UpdateNetSupplierInfo(info);
    std::unique_lock<std::mutex> locker(netManagerMutex_);
    netSuppliers_.erase(supplierId);
    locker.unlock();
    FindBestNetworkForAllRequest();
    NETMGR_LOG_D("UnregisterNetSupplier supplierId[%{public}d] out", supplierId);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::UnregisterNetConnCallbackAsync(const sptr<INetConnCallback> &callback)
{
    NETMGR_LOG_D("UnregisterNetConnCallback Enter");
    if (callback == nullptr) {
        NETMGR_LOG_E("callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    uint32_t reqId = 0;
    if (!FindSameCallback(callback, reqId)) {
        NETMGR_LOG_D("UnregisterNetConnCallback can not find same callback");
        return NET_CONN_ERR_SAME_CALLBACK;
    }
    NET_ACTIVATE_MAP::iterator iterActive;
    for (iterActive = netActivates_.begin(); iterActive != netActivates_.end();) {
        if (!iterActive->second) {
            ++iterActive;
            continue;
        }
        sptr<INetConnCallback> saveCallback = iterActive->second->GetNetCallback();
        if (saveCallback == nullptr) {
            ++iterActive;
            continue;
        }
        if (callback->AsObject().GetRefPtr() != saveCallback->AsObject().GetRefPtr()) {
            ++iterActive;
            continue;
        }
        reqId = iterActive->first;
        sptr<NetActivate> netActivate = iterActive->second;
        if (netActivate) {
            sptr<NetSupplier> supplier = netActivate->GetServiceSupply();
            if (supplier) {
                supplier->CancelRequest(reqId);
            }
        }
        NET_SUPPLIER_MAP::iterator iterSupplier;
        for (iterSupplier = netSuppliers_.begin(); iterSupplier != netSuppliers_.end(); ++iterSupplier) {
            if (iterSupplier->second != nullptr) {
                iterSupplier->second->CancelRequest(reqId);
            }
        }
        iterActive = netActivates_.erase(iterActive);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::RegUnRegNetDetectionCallbackAsync(int32_t netId, const sptr<INetDetectionCallback> &callback,
                                                          bool isReg)
{
    NETMGR_LOG_D("Enter NetConnService::RegUnRegNetDetectionCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    auto iterNetwork = networks_.find(netId);
    if ((iterNetwork == networks_.end()) || (iterNetwork->second == nullptr)) {
        NETMGR_LOG_E("Could not find the corresponding network.");
        return NET_CONN_ERR_NETID_NOT_FOUND;
    }
    if (isReg) {
        iterNetwork->second->RegisterNetDetectionCallback(callback);
        return NETMANAGER_SUCCESS;
    }
    return iterNetwork->second->UnRegisterNetDetectionCallback(callback);
}

int32_t NetConnService::UpdateNetStateForTestAsync(const sptr<NetSpecifier> &netSpecifier, int32_t netState)
{
    NETMGR_LOG_D("Test NetConnService::UpdateNetStateForTest(), begin");
    if (netSpecifier == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::UpdateNetSupplierInfoAsync(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    struct EventInfo eventInfo = {.updateSupplierId = supplierId};
    if (netSupplierInfo == nullptr) {
        NETMGR_LOG_E("netSupplierInfo is nullptr");
        eventInfo.errorType = static_cast<int32_t>(FAULT_UPDATE_SUPPLIERINFO_INV_PARAM);
        eventInfo.errorMsg = ERROR_MSG_NULL_SUPPLIER_INFO;
        EventReport::SendSupplierFaultEvent(eventInfo);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    NETMGR_LOG_I("Update supplier info: supplierId[%{public}d], netSupplierInfo[%{public}s]", supplierId,
                 netSupplierInfo->ToString(" ").c_str());
    eventInfo.supplierInfo = netSupplierInfo->ToString(" ");
    EventReport::SendSupplierBehaviorEvent(eventInfo);

    auto supplier = FindNetSupplier(supplierId);
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier is nullptr, netSuppliers_ size[%{public}zd]", netSuppliers_.size());
        eventInfo.errorType = static_cast<int32_t>(FAULT_UPDATE_SUPPLIERINFO_INV_PARAM);
        eventInfo.errorMsg = std::string(ERROR_MSG_CAN_NOT_FIND_SUPPLIER).append(std::to_string(supplierId));
        EventReport::SendSupplierFaultEvent(eventInfo);
        return NET_CONN_ERR_NO_SUPPLIER;
    }

    supplier->UpdateNetSupplierInfo(*netSupplierInfo);
    if (!netSupplierInfo->isAvailable_) {
        CallbackForSupplier(supplier, CALL_TYPE_LOST);
    } else {
        CallbackForSupplier(supplier, CALL_TYPE_UPDATE_CAP);
    }
    if (!netScore_->GetServiceScore(supplier)) {
        NETMGR_LOG_E("GetServiceScore fail.");
    }
    FindBestNetworkForAllRequest();
    NETMGR_LOG_D("UpdateNetSupplierInfo service out.");
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::UpdateNetLinkInfoAsync(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    NETMGR_LOG_D("UpdateNetLinkInfo service in. supplierId[%{public}d]", supplierId);
    struct EventInfo eventInfo = {.updateNetlinkId = supplierId};

    if (netLinkInfo == nullptr) {
        NETMGR_LOG_E("netLinkInfo is nullptr");
        eventInfo.errorType = static_cast<int32_t>(FAULT_UPDATE_NETLINK_INFO_INV_PARAM);
        eventInfo.errorMsg = ERROR_MSG_NULL_NET_LINK_INFO;
        EventReport::SendSupplierFaultEvent(eventInfo);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    eventInfo.netlinkInfo = netLinkInfo->ToString(" ");
    EventReport::SendSupplierBehaviorEvent(eventInfo);

    auto supplier = FindNetSupplier(supplierId);
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier is nullptr");
        eventInfo.errorType = static_cast<int32_t>(FAULT_UPDATE_NETLINK_INFO_INV_PARAM);
        eventInfo.errorMsg = std::string(ERROR_MSG_CAN_NOT_FIND_SUPPLIER).append(std::to_string(supplierId));
        EventReport::SendSupplierFaultEvent(eventInfo);
        return NET_CONN_ERR_NO_SUPPLIER;
    }
    // According to supplier id, get network from the list
    if (supplier->UpdateNetLinkInfo(*netLinkInfo) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("UpdateNetLinkInfo fail");
        eventInfo.errorType = static_cast<int32_t>(FAULT_UPDATE_NETLINK_INFO_FAILED);
        eventInfo.errorMsg = ERROR_MSG_UPDATE_NETLINK_INFO_FAILED;
        EventReport::SendSupplierFaultEvent(eventInfo);
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    CallbackForSupplier(supplier, CALL_TYPE_UPDATE_LINK);
    if (!netScore_->GetServiceScore(supplier)) {
        NETMGR_LOG_E("GetServiceScore fail.");
    }
    FindBestNetworkForAllRequest();
    NETMGR_LOG_D("UpdateNetLinkInfo service out.");
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::NetDetectionAsync(int32_t netId)
{
    NETMGR_LOG_D("Enter NetConnService::NetDetection");
    auto iterNetwork = networks_.find(netId);
    if ((iterNetwork == networks_.end()) || (iterNetwork->second == nullptr)) {
        NETMGR_LOG_E("Could not find the corresponding network.");
        return NET_CONN_ERR_NETID_NOT_FOUND;
    }
    iterNetwork->second->StartNetDetection(true);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::RestrictBackgroundChangedAsync(bool restrictBackground)
{
    NETMGR_LOG_D("Restrict background changed, background = %{public}d", restrictBackground);
    for (auto it = netSuppliers_.begin(); it != netSuppliers_.end(); ++it) {
        if (it->second == nullptr) {
            continue;
        }

        if (it->second->GetRestrictBackground() == restrictBackground) {
            NETMGR_LOG_D("it->second->GetRestrictBackground() == restrictBackground");
            return NET_CONN_ERR_NET_NO_RESTRICT_BACKGROUND;
        }

        if (it->second->GetNetSupplierType() == BEARER_VPN) {
            CallbackForSupplier(it->second, CALL_TYPE_BLOCK_STATUS);
        }
        it->second->SetRestrictBackground(restrictBackground);
    }
    return NETMANAGER_SUCCESS;
}

void NetConnService::SendGlobalHttpProxyChangeBroadcast()
{
    BroadcastInfo info;
    info.action = EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE;
    info.data = "Global HttpProxy Changed";
    info.ordered = true;
    std::map<std::string, std::string> param = {{"HttpProxy", httpProxy_.ToString()}};
    DelayedSingleton<BroadcastManager>::GetInstance()->SendBroadcast(info, param);
}

int32_t NetConnService::SetGlobalHttpProxyAsync(const HttpProxy &httpProxy)
{
    if (httpProxy_.GetHost() != httpProxy.GetHost() || httpProxy_.GetPort() != httpProxy.GetPort() ||
        httpProxy_.GetExclusionList() != httpProxy.GetExclusionList()) {
        httpProxy_ = httpProxy;
        SendGlobalHttpProxyChangeBroadcast();
        std::lock_guard<std::mutex> locker(netManagerMutex_);
        NetHttpProxyTracker httpProxyTracker;
        if (!httpProxyTracker.WriteToSystemParameter(httpProxy_)) {
            NETMGR_LOG_E("Write http proxy to system parameter failed");
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::ActivateNetwork(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                        const uint32_t &timeoutMS)
{
    NETMGR_LOG_D("ActivateNetwork Enter");
    if (netSpecifier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::weak_ptr<INetActivateCallback> timeoutCb = shared_from_this();
    sptr<NetActivate> request = new (std::nothrow) NetActivate(netSpecifier, callback, timeoutCb, timeoutMS);
    uint32_t reqId = request->GetRequestId();
    NETMGR_LOG_D("ActivateNetwork  reqId is [%{public}d]", reqId);
    netActivates_[reqId] = request;
    sptr<NetSupplier> bestNet = nullptr;
    int bestScore = static_cast<int>(FindBestNetworkForRequest(bestNet, request));
    if (bestScore != 0 && bestNet != nullptr) {
        NETMGR_LOG_I("ActivateNetwork:The bestScore is: [%{public}d], netHandle is [%{public}d]", bestScore,
                     bestNet->GetNetId());
        bestNet->SelectAsBestNetwork(reqId);
        request->SetServiceSupply(bestNet);
        CallbackForAvailable(bestNet, callback);
        if ((bestNet->GetNetSupplierType() == BEARER_CELLULAR) || (bestNet->GetNetSupplierType() == BEARER_WIFI)) {
            struct EventInfo eventInfo = {.capabilities = bestNet->GetNetCapabilities().ToString(" "),
                                          .supplierIdent = bestNet->GetNetSupplierIdent()};
            EventReport::SendRequestBehaviorEvent(eventInfo);
        }
        return NETMANAGER_SUCCESS;
    }
    if (timeoutMS == 0) {
        callback->NetUnavailable();
    }

    NETMGR_LOG_I("ActivateNetwork: can't found best network, send request to all networks.");
    SendRequestToAllNetwork(request);
    return NETMANAGER_SUCCESS;
}

void NetConnService::OnNetActivateTimeOut(uint32_t reqId)
{
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask([reqId, this]() {
            NETMGR_LOG_D("DeactivateNetwork Enter, reqId is [%{public}d]", reqId);
            auto iterActivate = netActivates_.find(reqId);
            if (iterActivate == netActivates_.end()) {
                NETMGR_LOG_E("not found the reqId: [%{public}d]", reqId);
                return;
            }
            if (iterActivate->second != nullptr) {
                sptr<NetSupplier> pNetService = iterActivate->second->GetServiceSupply();
                if (pNetService) {
                    pNetService->CancelRequest(reqId);
                }
            }

            NET_SUPPLIER_MAP::iterator iterSupplier;
            for (iterSupplier = netSuppliers_.begin(); iterSupplier != netSuppliers_.end(); ++iterSupplier) {
                if (iterSupplier->second == nullptr) {
                    continue;
                }
                iterSupplier->second->CancelRequest(reqId);
            }
        });
    }
}

sptr<NetSupplier> NetConnService::FindNetSupplier(uint32_t supplierId)
{
    auto iterSupplier = netSuppliers_.find(supplierId);
    if (iterSupplier != netSuppliers_.end()) {
        return iterSupplier->second;
    }
    return nullptr;
}

bool NetConnService::FindSameCallback(const sptr<INetConnCallback> &callback, uint32_t &reqId)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("callback is null");
        return false;
    }
    NET_ACTIVATE_MAP::iterator iterActive;
    for (iterActive = netActivates_.begin(); iterActive != netActivates_.end(); ++iterActive) {
        if (!iterActive->second) {
            continue;
        }
        sptr<INetConnCallback> saveCallback = iterActive->second->GetNetCallback();
        if (saveCallback == nullptr) {
            continue;
        }
        if (callback->AsObject().GetRefPtr() == saveCallback->AsObject().GetRefPtr()) {
            reqId = iterActive->first;
            return true;
        }
    }
    return false;
}

void NetConnService::FindBestNetworkForAllRequest()
{
    NETMGR_LOG_I("FindBestNetworkForAllRequest Enter");
    NET_ACTIVATE_MAP::iterator iterActive;
    sptr<NetSupplier> bestSupplier = nullptr;
    for (iterActive = netActivates_.begin(); iterActive != netActivates_.end(); ++iterActive) {
        if (!iterActive->second) {
            continue;
        }
        int score = static_cast<int>(FindBestNetworkForRequest(bestSupplier, iterActive->second));
        NETMGR_LOG_D("Find best supplier[%{public}d, %{public}s]for request[%{public}d]",
                     bestSupplier ? bestSupplier->GetSupplierId() : 0,
                     bestSupplier ? bestSupplier->GetNetSupplierIdent().c_str() : "null",
                     iterActive->second->GetRequestId());
        if (iterActive->second == defaultNetActivate_) {
            MakeDefaultNetWork(defaultNetSupplier_, bestSupplier);
        }
        sptr<NetSupplier> oldSupplier = iterActive->second->GetServiceSupply();
        sptr<INetConnCallback> callback = iterActive->second->GetNetCallback();
        if (!bestSupplier) {
            // not found the bestNetwork
            NotFindBestSupplier(iterActive->first, iterActive->second, oldSupplier, callback);
            continue;
        }
        SendBestScoreAllNetwork(iterActive->first, score, bestSupplier->GetSupplierId());
        if (bestSupplier == oldSupplier) {
            continue;
        }
        if (oldSupplier) {
            oldSupplier->RemoveBestRequest(iterActive->first);
        }
        iterActive->second->SetServiceSupply(bestSupplier);
        CallbackForAvailable(bestSupplier, callback);
        bestSupplier->SelectAsBestNetwork(iterActive->first);
    }
}

uint32_t NetConnService::FindBestNetworkForRequest(sptr<NetSupplier> &supplier, sptr<NetActivate> &netActivateNetwork)
{
    int bestScore = 0;
    supplier = nullptr;
    if (netActivateNetwork == nullptr) {
        NETMGR_LOG_E("netActivateNetwork is null");
        return bestScore;
    }
    NETMGR_LOG_I("FindBestNetworkForRequest Enter, request is [%{public}s]",
                 netActivateNetwork->GetNetSpecifier() ? netActivateNetwork->GetNetSpecifier()->ToString(" ").c_str()
                                                       : "null");
    NET_SUPPLIER_MAP::iterator iter;
    for (iter = netSuppliers_.begin(); iter != netSuppliers_.end(); ++iter) {
        if (iter->second == nullptr) {
            continue;
        }
        NETMGR_LOG_D("supplier info, supplier[%{public}d, %{public}s], realScore[%{public}d], isConnected[%{public}d]",
                     iter->second->GetSupplierId(), iter->second->GetNetSupplierIdent().c_str(),
                     iter->second->GetRealScore(), iter->second->IsConnected());
        if ((!iter->second->IsConnected()) || (!netActivateNetwork->MatchRequestAndNetwork(iter->second))) {
            NETMGR_LOG_D("Supplier[%{public}d] is not connected or not match request.", iter->second->GetSupplierId());
            continue;
        }
        int score = iter->second->GetRealScore();
        if (score > bestScore) {
            bestScore = score;
            supplier = iter->second;
        }
    }
    NETMGR_LOG_I("FindBestNetworkForRequest exit, bestScore[%{public}d], bestSupplier[%{public}d, %{public}s]",
                 bestScore, supplier ? supplier->GetSupplierId() : 0,
                 supplier ? supplier->GetNetSupplierIdent().c_str() : "null");
    return bestScore;
}

void NetConnService::RequestAllNetworkExceptDefault()
{
    if ((defaultNetSupplier_ == nullptr) || (defaultNetSupplier_->IsNetValidated())) {
        return;
    }
    NETMGR_LOG_I("Default supplier[%{public}d, %{public}s] is not valid,request to activate another network",
                 defaultNetSupplier_->GetSupplierId(), defaultNetSupplier_->GetNetSupplierIdent().c_str());
    if (defaultNetActivate_ == nullptr) {
        NETMGR_LOG_E("Default net request is null");
        return;
    }
    // Request activation of all networks except the default network
    uint32_t reqId = defaultNetActivate_->GetRequestId();
    for (const auto &netSupplier : netSuppliers_) {
        if (netSupplier.second == nullptr || netSupplier.second == defaultNetSupplier_) {
            continue;
        }
        if (netSupplier.second->GetNetScore() >= defaultNetSupplier_->GetNetScore()) {
            continue;
        }
        if (!defaultNetActivate_->MatchRequestAndNetwork(netSupplier.second)) {
            continue;
        }
        if (!netSupplier.second->RequestToConnect(reqId)) {
            NETMGR_LOG_E("Request to connect failed");
        }
    }
}

int32_t NetConnService::GenerateNetId()
{
    for (int32_t i = MIN_NET_ID; i <= MAX_NET_ID; ++i) {
        netIdLastValue_++;
        if (netIdLastValue_ > MAX_NET_ID) {
            netIdLastValue_ = MIN_NET_ID;
        }
        if (networks_.find(netIdLastValue_) == networks_.end()) {
            return netIdLastValue_;
        }
    }
    return INVALID_NET_ID;
}

void NetConnService::NotFindBestSupplier(uint32_t reqId, const sptr<NetActivate> &active,
                                         const sptr<NetSupplier> &supplier, const sptr<INetConnCallback> &callback)
{
    if (supplier != nullptr) {
        supplier->RemoveBestRequest(reqId);
        if (callback != nullptr) {
            sptr<NetHandle> netHandle = supplier->GetNetHandle();
            callback->NetLost(netHandle);
        }
    }
    if (active != nullptr) {
        active->SetServiceSupply(nullptr);
        SendRequestToAllNetwork(active);
    }
}

void NetConnService::SendAllRequestToNetwork(sptr<NetSupplier> supplier)
{
    NETMGR_LOG_I("SendAllRequestToNetwork.");
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier is null");
        return;
    }
    NET_ACTIVATE_MAP::iterator iter;
    for (iter = netActivates_.begin(); iter != netActivates_.end(); ++iter) {
        if (iter->second == nullptr) {
            continue;
        }
        if (!iter->second->MatchRequestAndNetwork(supplier)) {
            continue;
        }
        bool result = supplier->RequestToConnect(iter->first);
        if (!result) {
            NETMGR_LOG_E("connect supplier failed, result: %{public}d", result);
        }
    }
}

void NetConnService::SendRequestToAllNetwork(sptr<NetActivate> request)
{
    NETMGR_LOG_I("SendRequestToAllNetwork.");
    if (request == nullptr) {
        NETMGR_LOG_E("request is null");
        return;
    }

    uint32_t reqId = request->GetRequestId();
    NET_SUPPLIER_MAP::iterator iter;
    for (iter = netSuppliers_.begin(); iter != netSuppliers_.end(); ++iter) {
        if (iter->second == nullptr) {
            continue;
        }
        if (!request->MatchRequestAndNetwork(iter->second)) {
            continue;
        }
        bool result = iter->second->RequestToConnect(reqId);
        if (!result) {
            NETMGR_LOG_E("connect service failed, result %{public}d", result);
        }
    }
}

void NetConnService::SendBestScoreAllNetwork(uint32_t reqId, int32_t bestScore, uint32_t supplierId)
{
    NETMGR_LOG_I("SendBestScoreAllNetwork Enter");
    NET_SUPPLIER_MAP::iterator iter;
    for (iter = netSuppliers_.begin(); iter != netSuppliers_.end(); ++iter) {
        if (iter->second == nullptr) {
            continue;
        }
        iter->second->ReceiveBestScore(reqId, bestScore, supplierId);
    }
}

void NetConnService::CallbackForSupplier(sptr<NetSupplier> &supplier, CallbackType type)
{
    NETMGR_LOG_I("CallbackForSupplier Enter");
    if (supplier == nullptr) {
        return;
    }
    std::set<uint32_t> &bestReqList = supplier->GetBestRequestList();
    NETMGR_LOG_D("bestReqList size = %{public}zd", bestReqList.size());
    for (auto it : bestReqList) {
        auto reqIt = netActivates_.find(it);
        if ((reqIt == netActivates_.end()) || (reqIt->second == nullptr)) {
            NETMGR_LOG_D("netActivates_ not find reqId : %{public}d", it);
            continue;
        }
        sptr<INetConnCallback> callback = reqIt->second->GetNetCallback();
        if (!callback) {
            NETMGR_LOG_D("callback is nullptr");
            continue;
        }

        sptr<NetHandle> netHandle = supplier->GetNetHandle();
        switch (type) {
            case CALL_TYPE_LOST: {
                callback->NetLost(netHandle);
                break;
            }
            case CALL_TYPE_UPDATE_CAP: {
                sptr<NetAllCapabilities> pNetAllCap = std::make_unique<NetAllCapabilities>().release();
                *pNetAllCap = supplier->GetNetCapabilities();
                callback->NetCapabilitiesChange(netHandle, pNetAllCap);
                break;
            }
            case CALL_TYPE_UPDATE_LINK: {
                sptr<NetLinkInfo> pInfo = std::make_unique<NetLinkInfo>().release();
                auto network = supplier->GetNetwork();
                if (network != nullptr && pInfo != nullptr) {
                    *pInfo = network->GetNetLinkInfo();
                }
                callback->NetConnectionPropertiesChange(netHandle, pInfo);
                break;
            }
            case CALL_TYPE_BLOCK_STATUS: {
                bool Metered = supplier->HasNetCap(NET_CAPABILITY_NOT_METERED);
                bool newBlocked = NetManagerCenter::GetInstance().IsUidNetAccess(supplier->GetSupplierUid(), Metered);
                callback->NetBlockStatusChange(netHandle, newBlocked);
                break;
            }
            default:
                break;
        }
    }
}

void NetConnService::CallbackForAvailable(sptr<NetSupplier> &supplier, const sptr<INetConnCallback> &callback)
{
    if (supplier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("Input parameter is null.");
        return;
    }
    sptr<NetHandle> netHandle = supplier->GetNetHandle();
    callback->NetAvailable(netHandle);
    sptr<NetAllCapabilities> pNetAllCap = std::make_unique<NetAllCapabilities>().release();
    *pNetAllCap = supplier->GetNetCapabilities();
    callback->NetCapabilitiesChange(netHandle, pNetAllCap);
    sptr<NetLinkInfo> pInfo = std::make_unique<NetLinkInfo>().release();
    auto network = supplier->GetNetwork();
    if (network != nullptr && pInfo != nullptr) {
        *pInfo = network->GetNetLinkInfo();
    }
    callback->NetConnectionPropertiesChange(netHandle, pInfo);
}

void NetConnService::MakeDefaultNetWork(sptr<NetSupplier> &oldSupplier, sptr<NetSupplier> &newSupplier)
{
    NETMGR_LOG_I("MakeDefaultNetWork in, oldSupplier[%{public}d, %{public}s], newSupplier[%{public}d, %{public}s]",
                 oldSupplier ? oldSupplier->GetSupplierId() : 0,
                 oldSupplier ? oldSupplier->GetNetSupplierIdent().c_str() : "null",
                 newSupplier ? newSupplier->GetSupplierId() : 0,
                 newSupplier ? newSupplier->GetNetSupplierIdent().c_str() : "null");
    if (oldSupplier == newSupplier) {
        NETMGR_LOG_D("old supplier equal to new supplier.");
        return;
    }
    if (oldSupplier != nullptr) {
        oldSupplier->ClearDefault();
    }
    if (newSupplier != nullptr) {
        newSupplier->SetDefault();
    }
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    oldSupplier = newSupplier;
    NETMGR_LOG_I("Default supplier set to: [%{public}d, %{public}s]", oldSupplier ? oldSupplier->GetSupplierId() : 0,
                 oldSupplier ? oldSupplier->GetNetSupplierIdent().c_str() : "null");
}

void NetConnService::HandleDetectionResult(uint32_t supplierId, bool ifValid)
{
    NETMGR_LOG_I("Enter HandleDetectionResult, ifValid[%{public}d]", ifValid);
    auto supplier = FindNetSupplier(supplierId);
    if (supplier == nullptr) {
        NETMGR_LOG_E("supplier doesn't exist.");
        return;
    }
    supplier->SetNetValid(ifValid);
    CallbackForSupplier(supplier, CALL_TYPE_UPDATE_CAP);
    if (!netScore_->GetServiceScore(supplier)) {
        NETMGR_LOG_E("GetServiceScore fail.");
        return;
    }
    FindBestNetworkForAllRequest();
    if (!ifValid && defaultNetSupplier_ && defaultNetSupplier_->GetSupplierId() == supplierId) {
        RequestAllNetworkExceptDefault();
    }
}

std::list<sptr<NetSupplier>> NetConnService::GetNetSupplierFromList(NetBearType bearerType, const std::string &ident)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    std::list<sptr<NetSupplier>> ret;
    for (const auto &netSupplier : netSuppliers_) {
        if (netSupplier.second == nullptr) {
            continue;
        }
        if ((bearerType != netSupplier.second->GetNetSupplierType())) {
            continue;
        }
        if (!ident.empty() && netSupplier.second->GetNetSupplierIdent() != ident) {
            continue;
        }
        ret.push_back(netSupplier.second);
    }
    return ret;
}

sptr<NetSupplier> NetConnService::GetNetSupplierFromList(NetBearType bearerType, const std::string &ident,
                                                         const std::set<NetCap> &netCaps)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    for (const auto &netSupplier : netSuppliers_) {
        if (netSupplier.second == nullptr) {
            continue;
        }
        if ((bearerType == netSupplier.second->GetNetSupplierType()) &&
            (ident == netSupplier.second->GetNetSupplierIdent()) && netSupplier.second->CompareNetCaps(netCaps)) {
            return netSupplier.second;
        }
    }
    return nullptr;
}

int32_t NetConnService::GetDefaultNet(int32_t &netId)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    if (!defaultNetSupplier_) {
        NETMGR_LOG_E("not found the netId");
        return NETMANAGER_SUCCESS;
    }

    netId = defaultNetSupplier_->GetNetId();
    NETMGR_LOG_D("GetDefaultNet found the netId: [%{public}d]", netId);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetAddressesByName(const std::string &host, int32_t netId, std::vector<INetAddr> &addrList)
{
    return NetManagerCenter::GetInstance().GetAddressesByName(host, static_cast<uint16_t>(netId), addrList);
}

int32_t NetConnService::GetAddressByName(const std::string &host, int32_t netId, INetAddr &addr)
{
    std::vector<INetAddr> addrList;
    int ret = GetAddressesByName(host, netId, addrList);
    if (ret == NETMANAGER_SUCCESS) {
        if (!addrList.empty()) {
            addr = addrList[0];
            return ret;
        }
        return NET_CONN_ERR_NO_ADDRESS;
    }
    return ret;
}

int32_t NetConnService::GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList)
{
    if (bearerType < BEARER_CELLULAR || bearerType >= BEARER_DEFAULT) {
        NETMGR_LOG_E("netType parameter invalid");
        return NET_CONN_ERR_NET_TYPE_NOT_FOUND;
    }

    std::lock_guard<std::mutex> locker(netManagerMutex_);
    NET_SUPPLIER_MAP::iterator iterSupplier;
    for (iterSupplier = netSuppliers_.begin(); iterSupplier != netSuppliers_.end(); ++iterSupplier) {
        if (iterSupplier->second == nullptr) {
            continue;
        }
        auto supplierType = iterSupplier->second->GetNetSupplierType();
        if (bearerType == supplierType) {
            netIdList.push_back(iterSupplier->second->GetNetId());
        }
    }
    NETMGR_LOG_D("netSuppliers_ size[%{public}zd] networks_ size[%{public}zd]", netSuppliers_.size(), networks_.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetAllNets(std::list<int32_t> &netIdList)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    for (const auto &network : networks_) {
        if (network.second != nullptr && network.second->IsConnected()) {
            netIdList.push_back(network.second->GetNetId());
        }
    }
    NETMGR_LOG_D("netSuppliers_ size[%{public}zd] netIdList size[%{public}zd]", netSuppliers_.size(), netIdList.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetSpecificUidNet(int32_t uid, int32_t &netId)
{
    NETMGR_LOG_D("Enter GetSpecificUidNet, uid is [%{public}d].", uid);
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    netId = INVALID_NET_ID;
    NET_SUPPLIER_MAP::iterator iterSupplier;
    for (iterSupplier = netSuppliers_.begin(); iterSupplier != netSuppliers_.end(); ++iterSupplier) {
        if ((iterSupplier->second != nullptr) && (uid == iterSupplier->second->GetSupplierUid()) &&
            (iterSupplier->second->GetNetSupplierType() == BEARER_VPN)) {
            netId = iterSupplier->second->GetNetId();
            return NETMANAGER_SUCCESS;
        }
    }
    if (defaultNetSupplier_ != nullptr) {
        netId = defaultNetSupplier_->GetNetId();
    }
    NETMGR_LOG_D("GetDefaultNet found the netId: [%{public}d]", netId);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetConnectionProperties(int32_t netId, NetLinkInfo &info)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    auto iterNetwork = networks_.find(netId);
    if ((iterNetwork == networks_.end()) || (iterNetwork->second == nullptr)) {
        return NET_CONN_ERR_INVALID_NETWORK;
    }

    info = iterNetwork->second->GetNetLinkInfo();
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    NET_SUPPLIER_MAP::iterator iterSupplier;
    for (iterSupplier = netSuppliers_.begin(); iterSupplier != netSuppliers_.end(); ++iterSupplier) {
        if ((iterSupplier->second != nullptr) && (netId == iterSupplier->second->GetNetId())) {
            netAllCap = iterSupplier->second->GetNetCapabilities();
            return NETMANAGER_SUCCESS;
        }
    }
    return NET_CONN_ERR_INVALID_NETWORK;
}

int32_t NetConnService::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    if (bearerType < BEARER_CELLULAR || bearerType >= BEARER_DEFAULT) {
        return NET_CONN_ERR_NET_TYPE_NOT_FOUND;
    }

    auto suppliers = GetNetSupplierFromList(bearerType);
    for (auto supplier : suppliers) {
        if (supplier == nullptr) {
            continue;
        }
        std::shared_ptr<Network> network = supplier->GetNetwork();
        if (network == nullptr) {
            continue;
        }
        std::string ifaceName = network->GetNetLinkInfo().ifaceName_;
        if (!ifaceName.empty()) {
            ifaceNames.push_back(ifaceName);
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName)
{
    if (bearerType < BEARER_CELLULAR || bearerType >= BEARER_DEFAULT) {
        NETMGR_LOG_E("netType parameter invalid");
        return NET_CONN_ERR_NET_TYPE_NOT_FOUND;
    }

    auto suppliers = GetNetSupplierFromList(bearerType, ident);
    if (suppliers.empty()) {
        NETMGR_LOG_D("supplier is nullptr.");
        return NET_CONN_ERR_NO_SUPPLIER;
    }
    auto supplier = suppliers.front();
    std::shared_ptr<Network> network = supplier->GetNetwork();
    if (network == nullptr) {
        NETMGR_LOG_E("network is nullptr");
        return NET_CONN_ERR_INVALID_NETWORK;
    }

    ifaceName = network->GetNetLinkInfo().ifaceName_;

    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetGlobalHttpProxy(HttpProxy &httpProxy)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    if (httpProxy_.GetHost().empty()) {
        httpProxy.SetPort(0);
        NETMGR_LOG_E("The http proxy host is empty");
        return NETMANAGER_SUCCESS;
    }
    httpProxy = httpProxy_;
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList)
{
    if (ident.empty()) {
        NETMGR_LOG_E("The identifier in service is null");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    for (auto iterSupplier : netSuppliers_) {
        if (iterSupplier.second == nullptr) {
            continue;
        }
        if (iterSupplier.second->GetNetSupplierIdent() == ident) {
            int32_t netId = iterSupplier.second->GetNetId();
            netIdList.push_back(netId);
        }
    }
    return NETMANAGER_SUCCESS;
}

void NetConnService::GetDumpMessage(std::string &message)
{
    message.append("Net connect Info:\n");
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    if (defaultNetSupplier_) {
        message.append("\tSupplierId: " + std::to_string(defaultNetSupplier_->GetSupplierId()) + "\n");
        std::shared_ptr<Network> network = defaultNetSupplier_->GetNetwork();
        if (network) {
            message.append("\tNetId: " + std::to_string(network->GetNetId()) + "\n");
        } else {
            message.append("\tNetId: " + std::to_string(INVALID_NET_ID) + "\n");
        }
        message.append("\tConnStat: " + std::to_string(defaultNetSupplier_->IsConnected()) + "\n");
        message.append("\tIsAvailable: " + std::to_string(defaultNetSupplier_->IsNetValidated()) + "\n");
        message.append("\tIsRoaming: " + std::to_string(defaultNetSupplier_->GetRoaming()) + "\n");
        message.append("\tStrength: " + std::to_string(defaultNetSupplier_->GetStrength()) + "\n");
        message.append("\tFrequency: " + std::to_string(defaultNetSupplier_->GetFrequency()) + "\n");
        message.append("\tLinkUpBandwidthKbps: " +
                       std::to_string(defaultNetSupplier_->GetNetCapabilities().linkUpBandwidthKbps_) + "\n");
        message.append("\tLinkDownBandwidthKbps: " +
                       std::to_string(defaultNetSupplier_->GetNetCapabilities().linkDownBandwidthKbps_) + "\n");
        message.append("\tUid: " + std::to_string(defaultNetSupplier_->GetSupplierUid()) + "\n");
    } else {
        message.append("\tdefaultNetSupplier_ is nullptr\n");
        message.append("\tSupplierId: \n");
        message.append("\tNetId: 0\n");
        message.append("\tConnStat: 0\n");
        message.append("\tIsAvailable: \n");
        message.append("\tIsRoaming: 0\n");
        message.append("\tStrength: 0\n");
        message.append("\tFrequency: 0\n");
        message.append("\tLinkUpBandwidthKbps: 0\n");
        message.append("\tLinkDownBandwidthKbps: 0\n");
        message.append("\tUid: 0\n");
    }
}

int32_t NetConnService::HasDefaultNet(bool &flag)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    if (!defaultNetSupplier_) {
        flag = false;
        return NETMANAGER_SUCCESS;
    }
    flag = true;
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::IsDefaultNetMetered(bool &isMetered)
{
    std::lock_guard<std::mutex> locker(netManagerMutex_);
    if (defaultNetSupplier_) {
        isMetered = !defaultNetSupplier_->HasNetCap(NET_CAPABILITY_NOT_METERED);
    } else {
        isMetered = true;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::BindSocket(int32_t socket_fd, int32_t netId)
{
    NETMGR_LOG_D("Enter BindSocket.");
    return NetsysController::GetInstance().BindSocket(socket_fd, netId);
}

int32_t NetConnService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    NETMGR_LOG_D("Start Dump, fd: %{public}d", fd);
    std::string result;
    GetDumpMessage(result);
    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    return ret < 0 ? static_cast<int32_t>(NET_CONN_ERR_CREATE_DUMP_FAILED)
                   : static_cast<int32_t>(NETMANAGER_SUCCESS);
}

int32_t NetConnService::SetAirplaneMode(bool state)
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    BroadcastInfo info;
    info.action = EventFwk::CommonEventSupport::COMMON_EVENT_AIRPLANE_MODE_CHANGED;
    info.data = "Net Manager Airplane Mode Changed";
    info.code = static_cast<int32_t>(state);
    info.ordered = true;
    std::map<std::string, int32_t> param;
    DelayedSingleton<BroadcastManager>::GetInstance()->SendBroadcast(info, param);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnService::SetGlobalHttpProxy(const HttpProxy &httpProxy)
{
    int32_t result = NETMANAGER_ERROR;
    if (netConnEventHandler_) {
        netConnEventHandler_->PostSyncTask(
            [this, &httpProxy, &result]() { result = this->SetGlobalHttpProxyAsync(httpProxy); });
    }
    return result;
}

int32_t NetConnService::SetAppNet(int32_t netId)
{
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
