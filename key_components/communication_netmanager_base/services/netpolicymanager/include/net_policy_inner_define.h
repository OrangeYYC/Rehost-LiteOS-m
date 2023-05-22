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

#ifndef NET_POLICY_DEFINE_H
#define NET_POLICY_DEFINE_H

#include "common_event_manager.h"
#include "common_event_support.h"

#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
const std::string COMMON_EVENT_NET_QUOTA_LIMIT = "usual.event.NET_QUOTA_LIMIT";
const std::string COMMON_EVENT_NET_QUOTA_LIMIT_REMINDED = "usual.event.NET_QUOTA_LIMIT_REMINDED";
const std::string COMMON_EVENT_NET_QUOTA_WARNING = "usual.event.QUOTA_WARNING";
const std::string COMMON_EVENT_POWER_SAVE_MODE_CHANGED =
    EventFwk::CommonEventSupport::COMMON_EVENT_POWER_SAVE_MODE_CHANGED;
const std::string COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED =
    EventFwk::CommonEventSupport::COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED;
const std::string COMMON_EVENT_UID_REMOVED = EventFwk::CommonEventSupport::COMMON_EVENT_UID_REMOVED;

const mode_t CHOWN_RWX_USR_GRP = 0770;
constexpr int16_t PERIODDURATION_POS_NUM_ONE = 1;
constexpr int16_t DAY_ONE = 1;
constexpr int16_t DAY_THIRTY_ONE = 31;
constexpr int16_t LIMIT_ACTION_ONE = 1;
constexpr int16_t LIMIT_ACTION_THREE = 3;
constexpr int16_t LIMIT_CALLBACK_NUM = 200;
constexpr const char *POLICY_FILE_NAME = "/data/service/el1/public/netmanager/net_policy.json";
constexpr const char *CONFIG_HOS_VERSION = "hosVersion";
constexpr const char *CONFIG_UID_POLICY = "uidPolicy";
constexpr const char *CONFIG_UID = "uid";
constexpr const char *CONFIG_POLICY = "policy";
constexpr const char *HOS_VERSION = "1.0";
constexpr const char *CONFIG_BACKGROUND_POLICY = "backgroundPolicy";
constexpr const char *CONFIG_BACKGROUND_POLICY_STATUS = "status";
constexpr const char *CONFIG_QUOTA_POLICY = "quotaPolicy";
constexpr const char *CONFIG_QUOTA_POLICY_NETTYPE = "netType";
constexpr const char *CONFIG_QUOTA_POLICY_SUBSCRIBERID = "iccid";
constexpr const char *CONFIG_QUOTA_POLICY_PERIODSTARTTIME = "periodStartTime";
constexpr const char *CONFIG_QUOTA_POLICY_PERIODDURATION = "periodDuration";
constexpr const char *CONFIG_QUOTA_POLICY_WARNINGBYTES = "warningBytes";
constexpr const char *CONFIG_QUOTA_POLICY_LIMITBYTES = "limitBytes";
constexpr const char *CONFIG_QUOTA_POLICY_LASTLIMITSNOOZE = "lastLimitSnooze";
constexpr const char *CONFIG_QUOTA_POLICY_METERED = "metered";
constexpr const char *CONFIG_QUOTA_POLICY_SOURCE = "source";
constexpr const char *CONFIG_CELLULAR_POLICY = "cellularPolicy";
constexpr const char *CONFIG_CELLULAR_POLICY_SUBSCRIBERID = "iccid";
constexpr const char *CONFIG_CELLULAR_POLICY_PERIODSTARTTIME = "periodStartTime";
constexpr const char *CONFIG_CELLULAR_POLICY_PERIODDURATION = "periodDuration";
constexpr const char *CONFIG_CELLULAR_POLICY_TITLE = "title";
constexpr const char *CONFIG_CELLULAR_POLICY_SUMMARY = "summary";
constexpr const char *CONFIG_CELLULAR_POLICY_LIMITBYTES = "limitBytes";
constexpr const char *CONFIG_CELLULAR_POLICY_LIMITACTION = "limitAction";
constexpr const char *CONFIG_CELLULAR_POLICY_USEDBYTES = "usedBytes";
constexpr const char *CONFIG_CELLULAR_POLICY_USEDTIMEDURATION = "usedTimeDuration";
constexpr const char *CONFIG_CELLULAR_POLICY_POSSESSOR = "possessor";
constexpr const char *BACKGROUND_POLICY_ALLOW = "allow";
constexpr const char *BACKGROUND_POLICY_REJECT = "reject";
constexpr const char *IDENT_PREFIX_CELLULAR = "simId";
constexpr const char *IDENT_PREFIX_WIFI = "ssId";

constexpr const char *FIREWALL_CHAIN_NAME_DEVICE_IDLE = "deviceidle";

constexpr const char *UNKNOW_IFACE = "";

constexpr const char *NET_POLICY_SERVICE_NAME = "NetPolicyService";

enum {
    FIREWALL_CHAIN_NONE = 0,         // ChainType::CHAIN_NONE
    FIREWALL_CHAIN_DEVICE_IDLE = 16, // ChainType::CHAIN_OHFW_DOZABLE
    FIREWALL_CHAIN_POWER_SAVE = 17,  // ChainType::CHAIN_OHFW_UNDOZABLE
};

enum {
    FIREWALL_RULE_ALLOW = 1,
    FIREWALL_RULE_DENY = 2,
};

struct UidPolicy {
    std::string uid;
    std::string policy;
};

struct NetPolicyQuota {
    std::string netType;
    std::string iccid;
    std::string periodStartTime;
    std::string periodDuration;
    std::string warningBytes;
    std::string limitBytes;
    std::string lastLimitSnooze;
    std::string metered;
    std::string source;
};

struct NetPolicyCellular {
    std::string iccid;
    std::string periodStartTime;
    std::string periodDuration;
    std::string title;
    std::string summary;
    std::string limitBytes;
    std::string limitAction;
    std::string usedBytes;
    std::string usedTimeDuration;
    std::string possessor;
};

struct NetPolicy {
    std::string hosVersion;
    std::vector<UidPolicy> uidPolicies;
    std::string backgroundPolicyStatus;
    std::vector<NetPolicyQuota> netQuotaPolicies;
};

// link power_mode_module.h
enum {
    POWER_MODE_MIN = 600,
    NORMAL_MODE = POWER_MODE_MIN,
    SAVE_MODE,
    EXTREME_MODE,
    LOWPOWER_MODE,
    POWER_MODE_MAX = LOWPOWER_MODE
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_DEFINE_H