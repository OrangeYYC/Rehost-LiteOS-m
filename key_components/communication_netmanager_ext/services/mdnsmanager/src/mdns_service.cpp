/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "mdns_service.h"

#include <sys/time.h>

#include "errorcode_convertor.h"
#include "hisysevent.h"
#include "netmgr_ext_log_wrapper.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr const char *NET_MDNS_REQUEST_FAULT = "NET_MDNS_REQUEST_FAULT";
constexpr const char *NET_MDNS_REQUEST_BEHAVIOR = "NET_MDNS_REQUEST_BEHAVIOR";

constexpr const char *EVENT_KEY_REQUEST_TYPE = "TYPE";
constexpr const char *EVENT_KEY_REQUEST_DATA = "DATA";
constexpr const char *EVENT_KEY_ERROR_TYPE = "ERROR_TYPE";
constexpr const char *EVENT_KEY_ERROR_MSG = "ERROR_MSG";

constexpr const char *EVENT_DATA_CALLBACK = "callback";

using HiSysEvent = OHOS::HiviewDFX::HiSysEvent;

struct EventInfo {
    int32_t type = 0;
    std::string data;
    int32_t errorType = 0;
};

void SendRequestEvent(const EventInfo &eventInfo)
{
    static NetBaseErrorCodeConvertor convertor;
    auto code = eventInfo.errorType;
    if (code == NETMANAGER_EXT_SUCCESS) {
        HiSysEventWrite(HiSysEvent::Domain::NETMANAGER_STANDARD, NET_MDNS_REQUEST_BEHAVIOR,
                        HiSysEvent::EventType::BEHAVIOR, EVENT_KEY_REQUEST_TYPE, eventInfo.type, EVENT_KEY_REQUEST_DATA,
                        eventInfo.data);
    } else {
        HiSysEventWrite(HiSysEvent::Domain::NETMANAGER_STANDARD, NET_MDNS_REQUEST_FAULT, HiSysEvent::EventType::FAULT,
                        EVENT_KEY_REQUEST_TYPE, eventInfo.type, EVENT_KEY_REQUEST_DATA, eventInfo.data,
                        EVENT_KEY_ERROR_TYPE, eventInfo.errorType, EVENT_KEY_ERROR_MSG,
                        convertor.ConvertErrorCode(code));
    }
}

} // namespace

const bool REGISTER_LOCAL_RESULT_MDNS =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<MDnsService>::GetInstance().get());

MDnsService::MDnsService()
    : SystemAbility(COMM_MDNS_MANAGER_SYS_ABILITY_ID, true), isRegistered_(false), state_(STATE_STOPPED)
{
}

MDnsService::~MDnsService() = default;

void MDnsService::OnStart()
{
    NETMGR_EXT_LOG_D("MDnsService::OnStart begin");
    if (state_ == STATE_RUNNING) {
        NETMGR_EXT_LOG_D("MDnsService the state is already running");
        return;
    }
    if (!Init()) {
        NETMGR_EXT_LOG_E("MDnsService init failed");
        return;
    }
    state_ = STATE_RUNNING;
    NETMGR_EXT_LOG_D("MDnsService::OnStart end");
}

void MDnsService::OnStop()
{
    state_ = STATE_STOPPED;
    isRegistered_ = false;
}

bool MDnsService::Init()
{
    if (!REGISTER_LOCAL_RESULT_MDNS) {
        NETMGR_EXT_LOG_E("MDnsService Register to local sa manager failed");
        return false;
    }
    if (!isRegistered_) {
        if (!Publish(DelayedSingleton<MDnsService>::GetInstance().get())) {
            NETMGR_EXT_LOG_E("MDnsService Register to sa manager failed");
            return false;
        }
        isRegistered_ = true;
    }
    NETMGR_EXT_LOG_D("Init mdns service OK");
    return true;
}

int32_t MDnsService::RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb)
{
    int32_t err = manager_.RegisterService(serviceInfo, cb);
    if (err != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("manager call failed, error code: [%{public}d]", err);
    }
    EventInfo eventInfo;
    eventInfo.type = CMD_REGISTER;
    eventInfo.data = serviceInfo.name + MDNS_DOMAIN_SPLITER_STR + serviceInfo.type + MDNS_HOSTPORT_SPLITER_STR +
                     std::to_string(serviceInfo.port);
    eventInfo.errorType = err;
    SendRequestEvent(eventInfo);
    return err;
}

int32_t MDnsService::UnRegisterService(const sptr<IRegistrationCallback> &cb)
{
    int32_t err = manager_.UnRegisterService(cb);
    if (err != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("manager call failed, error code: [%{public}d]", err);
    }
    EventInfo eventInfo;
    eventInfo.type = CMD_STOP_REGISTER;
    eventInfo.data = EVENT_DATA_CALLBACK;
    eventInfo.errorType = err;
    SendRequestEvent(eventInfo);
    return err;
}

int32_t MDnsService::StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb)
{
    int32_t err = manager_.StartDiscoverService(serviceType, cb);
    if (err != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("manager call failed, error code: [%{public}d]", err);
    }
    EventInfo eventInfo;
    eventInfo.type = CMD_DISCOVER;
    eventInfo.data = serviceType;
    eventInfo.errorType = err;
    SendRequestEvent(eventInfo);
    return err;
}

int32_t MDnsService::StopDiscoverService(const sptr<IDiscoveryCallback> &cb)
{
    int32_t err = manager_.StopDiscoverService(cb);
    if (err != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("manager call failed, error code: [%{public}d]", err);
    }
    EventInfo eventInfo;
    eventInfo.type = CMD_STOP_DISCOVER;
    eventInfo.data = EVENT_DATA_CALLBACK;
    eventInfo.errorType = err;
    SendRequestEvent(eventInfo);
    return err;
}

int32_t MDnsService::ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb)
{
    int32_t err = manager_.ResolveService(serviceInfo, cb);
    if (err != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("manager call failed, error code: [%{public}d]", err);
    }
    EventInfo eventInfo;
    eventInfo.type = CMD_RESOLVE;
    eventInfo.data = serviceInfo.name + MDNS_DOMAIN_SPLITER_STR + serviceInfo.type;
    eventInfo.errorType = err;
    SendRequestEvent(eventInfo);
    return err;
}

int32_t MDnsService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    NETMGR_EXT_LOG_D("Start Dump, fd: %{public}d", fd);
    std::string result;
    manager_.GetDumpMessage(result);
    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    return ret < 0 ? NET_MDNS_ERR_WRITE_DUMP : NETMANAGER_EXT_SUCCESS;
}

} // namespace NetManagerStandard
} // namespace OHOS