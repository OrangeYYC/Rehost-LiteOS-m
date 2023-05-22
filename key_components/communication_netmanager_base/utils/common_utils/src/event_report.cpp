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

#include "event_report.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
// event name
constexpr const char *NET_CONN_SUPPLER_FAULT = "NET_CONN_SUPPLER_FAULT";
constexpr const char *NET_CONN_REQUEST_FAULT = "NET_CONN_REQUEST_FAULT";
constexpr const char *NET_CONN_MONITOR_FAULT = "NET_CONN_MONITOR_FAULT";
constexpr const char *NET_CONN_SUPPLER_STAT = "NET_CONN_SUPPLER_STAT";
constexpr const char *NET_CONN_REQUEST_STAT = "NET_CONN_REQUEST_STAT";
constexpr const char *NET_CONN_MONITOR_STAT = "NET_CONN_MONITOR_STAT";
// event params
constexpr const char *EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERID = "UPDATE_SUPPLIERID";
constexpr const char *EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERINFO = "UPDATE_SUPPLIERINFO";
constexpr const char *EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKID = "UPDATE_NETLINKID";
constexpr const char *EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKINFO = "UPDATE_NETLINKINFO";
constexpr const char *EVENT_KEY_ERROR_TYPE = "ERROR_TYPE";
constexpr const char *EVENT_KEY_ERROR_MSG = "ERROR_MSG";
constexpr const char *EVENT_KEY_NET_REQUEST_CAPABILITY = "CAPABILITIES";
constexpr const char *EVENT_KEY_NET_MONITOR_SOCKETFD = "SOCKETFD";
constexpr const char *EVENT_KEY_NET_MONITOR_NETID = "NETID";
constexpr const char *EVENT_KEY_NET_SUPPLIER_REGISTER_BEARERTYPE = "REGISTER_BEARERTYPE";
constexpr const char *EVENT_KEY_NET_SUPPLIER_REGISTER_IDENT = "REGISTER_IDENT";
constexpr const char *EVENT_KEY_NET_SUPPLIER_REGISTER_SUPPLIERID = "REGISTER_SUPPLIERID";
constexpr const char *EVENT_KEY_NET_REQUEST_CALLBACK_AVAILABLE = "CALLBACK_AVAILABLE";
constexpr const char *EVENT_KEY_NET_REQUEST_SUPPLIERIDENT = "SUPPLIERIDENT";
constexpr const char *EVENT_KEY_NET_MONITOR_STATUS = "STATUS";
} // namespace

void EventReport::SendSupplierFaultEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_SUPPLER_FAULT,
        HiSysEvent::EventType::FAULT,
        EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERID, eventInfo.updateSupplierId,
        EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERINFO, eventInfo.supplierInfo,
        EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKID, eventInfo.updateNetlinkId,
        EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKINFO, eventInfo.netlinkInfo,
        EVENT_KEY_ERROR_TYPE, eventInfo.errorType,
        EVENT_KEY_ERROR_MSG, eventInfo.errorMsg);
}

void EventReport::SendSupplierBehaviorEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_SUPPLER_STAT,
        HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_NET_SUPPLIER_REGISTER_BEARERTYPE, eventInfo.bearerType,
        EVENT_KEY_NET_SUPPLIER_REGISTER_IDENT, eventInfo.ident,
        EVENT_KEY_NET_SUPPLIER_REGISTER_SUPPLIERID, eventInfo.supplierId,
        EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERID, eventInfo.updateSupplierId,
        EVENT_KEY_NET_SUPPLIER_UPDATE_SUPPLIERINFO, eventInfo.supplierInfo,
        EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKID, eventInfo.updateNetlinkId,
        EVENT_KEY_NET_SUPPLIER_UPDATE_NETLINKINFO, eventInfo.netlinkInfo);
}

void EventReport::SendRequestFaultEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_REQUEST_FAULT,
        HiSysEvent::EventType::FAULT,
        EVENT_KEY_NET_REQUEST_CAPABILITY, eventInfo.capabilities,
        EVENT_KEY_NET_REQUEST_SUPPLIERIDENT, eventInfo.supplierIdent,
        EVENT_KEY_ERROR_TYPE, eventInfo.errorType,
        EVENT_KEY_ERROR_MSG, eventInfo.errorMsg);
}

void EventReport::SendRequestBehaviorEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_REQUEST_STAT,
        HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_NET_REQUEST_CAPABILITY, eventInfo.capabilities,
        EVENT_KEY_NET_REQUEST_SUPPLIERIDENT, eventInfo.supplierIdent,
        EVENT_KEY_NET_REQUEST_CALLBACK_AVAILABLE, eventInfo.callbackAvailable);
}

void EventReport::SendMonitorFaultEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_MONITOR_FAULT,
        HiSysEvent::EventType::FAULT,
        EVENT_KEY_NET_MONITOR_SOCKETFD, eventInfo.socketFd,
        EVENT_KEY_NET_MONITOR_NETID, eventInfo.netId,
        EVENT_KEY_ERROR_TYPE, eventInfo.errorType,
        EVENT_KEY_ERROR_MSG, eventInfo.errorMsg);
}

void EventReport::SendMonitorBehaviorEvent(const EventInfo &eventInfo)
{
    HiSysEventWrite(
        HiSysEvent::Domain::NETMANAGER_STANDARD,
        NET_CONN_MONITOR_STAT,
        HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_NET_MONITOR_STATUS, eventInfo.monitorStatus);
}
} // namespace NetManagerStandard
} // namespace OHOS