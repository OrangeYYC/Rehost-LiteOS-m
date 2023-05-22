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

#include "bandwidth_manager.h"

#include <cinttypes>

#include "iptables_wrapper.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
static constexpr const char *CHAIN_NAME_COSTLY_PTR = "ohbw_costly_";
BandwidthManager::BandwidthManager() : chainInitFlag_(false), dataSaverEnable_(false) {}

BandwidthManager::~BandwidthManager()
{
    DeInitChain();
}

inline void BandwidthManager::CheckChainInitialization()
{
    if (chainInitFlag_ == false) {
        InitChain();
        InitDefaultRules();
    }
}

std::string BandwidthManager::FetchChainName(ChainType chain)
{
    NETNATIVE_LOG_D("BandwidthManager FetchChainName: chain=%{public}d", chain);
    std::string chainName;
    switch (chain) {
        case ChainType::CHAIN_OHBW_INPUT:
            chainName = "ohbw_INPUT";
            break;
        case ChainType::CHAIN_OHBW_OUTPUT:
            chainName = "ohbw_OUTPUT";
            break;
        case ChainType::CHAIN_OHBW_FORWARD:
            chainName = "ohbw_FORWARD";
            break;
        case ChainType::CHAIN_OHBW_DENIED_LIST_BOX:
            chainName = "ohbw_denied_list_box";
            break;
        case ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX:
            chainName = "ohbw_allowed_list_box";
            break;
        case ChainType::CHAIN_OHBW_GLOBAL_ALERT:
            chainName = "ohbw_global_alert";
            break;
        case ChainType::CHAIN_OHBW_COSTLY_SHARED:
            chainName = "ohbw_costly_shared";
            break;
        case ChainType::CHAIN_OHBW_DATA_SAVER:
            chainName = "ohbw_data_saver";
            break;
        default:
            chainName = "oh_unusable";
            break;
    }
    return chainName;
}

int32_t BandwidthManager::InitChain()
{
    NETNATIVE_LOG_D("BandwidthManager InitChain");
    bool hasError = false;
    hasError = (IptablesNewChain(ChainType::CHAIN_OHBW_INPUT) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_OUTPUT) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_FORWARD) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_DENIED_LIST_BOX) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_GLOBAL_ALERT) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_COSTLY_SHARED) == NETMANAGER_ERROR) ||
               (IptablesNewChain(ChainType::CHAIN_OHBW_DATA_SAVER) == NETMANAGER_ERROR);
    chainInitFlag_ = true;
    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::DeInitChain()
{
    NETNATIVE_LOG_D("BandwidthManager DeInitChain");
    bool hasError = false;
    hasError = (IptablesDeleteChain(ChainType::CHAIN_OHBW_INPUT) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_OUTPUT) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_FORWARD) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_DENIED_LIST_BOX) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_GLOBAL_ALERT) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_COSTLY_SHARED) == NETMANAGER_ERROR) ||
               (IptablesDeleteChain(ChainType::CHAIN_OHBW_DATA_SAVER) == NETMANAGER_ERROR);
    chainInitFlag_ = false;
    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::InitDefaultBwChainRules()
{
    bool hasError = false;
    std::string command;
    std::string chainName;

    // -A INPUT -j ohbw_INPUT
    // -A OUTPUT -j ohbw_OUTPUT
    chainName = FetchChainName(ChainType::CHAIN_OHBW_INPUT);
    command = "-t filter -A INPUT -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    command = "-t filter -A OUTPUT -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -A ohbw_INPUT -p esp -j RETURN
    // -A ohbw_INPUT -m mark --mark 0x100000/0x100000 -j RETURN
    chainName = FetchChainName(ChainType::CHAIN_OHBW_INPUT);
    command = "-t filter -A " + chainName + " -p esp -j RETURN";
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command =
        "-t filter -A " + FetchChainName(ChainType::CHAIN_OHBW_INPUT) + " -m mark --mark 0x100000/0x100000 -j RETURN";
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::InitDefaultListBoxChainRules()
{
    bool hasError = false;
    std::string command;
    std::string chainName;
    std::string fChainName;

    // -A ohbw_OUPUT -j ohbw_denied_list_box
    // -A ohbw_denied_list_box -j ohbw_allowed_list_box
    // -A ohbw_allowed_list_box -j ohbw_data_saver
    // -A ohbw_data_saver -j RETURN
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_DENIED_LIST_BOX);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX);
    command = "-t filter -A " + fChainName + " -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_DATA_SAVER);
    command = "-t filter -A " + fChainName + " -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_DATA_SAVER);
    command = "-t filter -A " + chainName + " -j RETURN";
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::InitDefaultAlertChainRules()
{
    bool hasError = false;
    std::string command;
    std::string chainName;
    std::string fChainName;

    // -A ohbw_INPUT -j ohbw_global_alert
    // -A ohbw_OUTPUT -j ohbw_global_alert
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_INPUT);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_GLOBAL_ALERT);
    command = "-t filter -A " + fChainName + " -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    chainName = FetchChainName(ChainType::CHAIN_OHBW_GLOBAL_ALERT);
    command = "-t filter -A " + fChainName + " -j " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::InitDefaultRules()
{
    NETNATIVE_LOG_D("BandwidthManager InitDefaultRules");
    bool hasError = false;
    hasError = (InitDefaultBwChainRules() == NETMANAGER_ERROR) ||
               (InitDefaultListBoxChainRules() == NETMANAGER_ERROR) ||
               (InitDefaultAlertChainRules() == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::IptablesNewChain(ChainType chain)
{
    NETNATIVE_LOG_D("BandwidthManager NewChain: chain=%{public}d", chain);
    std::string command = "-t filter -N " + FetchChainName(chain);
    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t BandwidthManager::IptablesNewChain(const std::string &chainName)
{
    NETNATIVE_LOG_D("BandwidthManager NewChain: chain=%{public}s", chainName.c_str());
    std::string command = "-t filter -N " + chainName;
    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t BandwidthManager::IptablesDeleteChain(ChainType chain)
{
    NETNATIVE_LOG_D("BandwidthManager DeleteChain: chain=%{public}d", chain);
    bool hasError = false;
    std::string command = "-t filter -F " + FetchChainName(chain);
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -X " + FetchChainName(chain);
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::IptablesDeleteChain(const std::string &chainName)
{
    NETNATIVE_LOG_D("BandwidthManager DeleteChain: chain=%{public}s", chainName.c_str());
    bool hasError = false;
    std::string command = "-t filter -F " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -X " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::SetGlobalAlert(Operate operate, int64_t bytes)
{
    NETNATIVE_LOG_D("BandwidthManager SetGlobalAlert: operate=%{public}d, bytes=%{public}" PRId64, operate, bytes);
    bool hasError = false;
    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_GLOBAL_ALERT);
    if (operate == OP_SET) {
        globalAlertBytes_ = bytes;
        command = "-t filter -A " + chainName + " -m quota2 --quota " + std::to_string(bytes) + " --name globalAlert";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
    } else {
        if (bytes == globalAlertBytes_) {
            command =
                "-t filter -D " + chainName + " -m quota --quota " + std::to_string(bytes) + " --name globalAlert";
            hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(
                IPTYPE_IPV4, command) == NETMANAGER_ERROR);
            globalAlertBytes_ = 0;
        } else {
            NETNATIVE_LOGE("not match bytes, cannot remove global alert");
            return NETMANAGER_ERROR;
        }
    }

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::SetCostlyAlert(Operate operate, const std::string &iface, int64_t bytes)
{
    NETNATIVE_LOG_D("BandwidthManager SetCostlyAlert: operate=%{public}d, iface=%{public}s, bytes=%{public}" PRId64,
                    operate, iface.c_str(), bytes);
    bool hasError = false;
    std::string command;
    std::string chainName = std::string(CHAIN_NAME_COSTLY_PTR) + iface + "alert";
    if (operate == OP_SET) {
        ifaceAlertBytes_[iface] = bytes;
        command =
            "-t filter -A " + chainName + " -m quota2 --quota " + std::to_string(bytes) + " --name " + iface + "Alert";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
    } else {
        if (bytes == ifaceAlertBytes_[iface]) {
            command = "-t filter -D " + chainName + " -m quota2 --quota " + std::to_string(bytes) + " --name " + iface +
                      "Alert";
            hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(
                IPTYPE_IPV4, command) == NETMANAGER_ERROR);
            ifaceAlertBytes_[iface] = 0;
        } else {
            NETNATIVE_LOGE("not match bytes, cannot remove global alert");
            return NETMANAGER_ERROR;
        }
    }

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::EnableDataSaver(bool enable)
{
    NETNATIVE_LOG_D("BandwidthManager EnableDataSaver: enable=%{public}d", enable);
    bool hasError = false;
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_DATA_SAVER);
    if (enable == true && dataSaverEnable_ == false) {
        dataSaverEnable_ = true;
        command = "-t filter -R " + chainName + " 1 -j REJECT";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
        command = "-t filter -I " + chainName + " -m owner --uid-owner 0-9999 -j RETURN";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
    } else if (enable == false && dataSaverEnable_ == true) {
        dataSaverEnable_ = false;
        command = "-t filter -D " + chainName + " -m owner --uid-owner 0-9999 -j RETURN";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
        command = "-t filter -R " + chainName + " 1 -j RETURN";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
    } else {
        NETNATIVE_LOGE("DataSaver is already %{public}s, do not repeat", enable == true ? "true" : "false");
        return NETMANAGER_ERROR;
    }

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::SetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    if (!CommonUtils::CheckIfaceName(ifName)) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s", ifName.c_str());
        return NETMANAGER_ERROR;
    }
    NETNATIVE_LOG_D("BandwidthManager SetIfaceQuota: ifName=%{public}s, bytes=%{public}" PRId64, ifName.c_str(), bytes);
    bool hasError = false;
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    std::string command;
    std::string chainName = std::string(CHAIN_NAME_COSTLY_PTR) + ifName;
    std::string fChainName;
    std::string strMaxBytes = std::to_string(bytes);

    if (ifaceQuotaBytes_.count(ifName) > 0) {
        // -R ohbw_costly_iface 1 -m quota2 ! --quota 12345 --name iface -j REJECT
        command = "-t filter -D " + chainName + " -m quota2 ! --quota " + std::to_string(ifaceQuotaBytes_[ifName]) +
                  " --name " + ifName + " --jump REJECT";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
        command = "-t filter -A " + chainName + " -m quota2 ! --quota " + strMaxBytes + " --name " + ifName +
                  " --jump REJECT";
        hasError = hasError || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                                NETMANAGER_ERROR);
        ifaceQuotaBytes_[ifName] = bytes;
        return NETMANAGER_SUCCESS;
    }
    hasError = hasError || (IptablesNewChain(chainName) == NETMANAGER_ERROR);
    ifaceQuotaBytes_[ifName] = bytes;

    fChainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    std::string cChainName = FetchChainName(ChainType::CHAIN_OHBW_DENIED_LIST_BOX);
    command = "-t filter -A " + fChainName + " -o " + ifName + " -j " + cChainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    // -I ohbw_INPUT -i iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_INPUT);
    command = "-t filter -I " + fChainName + " -i " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -I ohbw_OUTPUT -o iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    command = "-t filter -I " + fChainName + " -o " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -A ohbw_FORWARD -i iface -j ohbw_costly_iface
    // -A ohbw_FORWARD -o iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_FORWARD);
    command = "-t filter -A " + fChainName + " -i " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -A " + fChainName + " -o " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -A ohbw_costly_iface -m quota2 ! --quota 12345 --name iface -j REJECT
    command =
        "-t filter -A " + chainName + " -m quota2 ! --quota " + strMaxBytes + " --name " + ifName + " --jump REJECT";
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::RemoveIfaceQuota(const std::string &ifName)
{
    if (!CommonUtils::CheckIfaceName(ifName)) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s", ifName.c_str());
        return NETMANAGER_ERROR;
    }
    NETNATIVE_LOG_D("BandwidthManager RemoveIfaceQuota: ifName=%{public}s", ifName.c_str());
    bool hasError = false;
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    std::string command;
    std::string chainName = std::string(CHAIN_NAME_COSTLY_PTR) + ifName;
    std::string fChainName;

    if (ifaceQuotaBytes_.count(ifName) == 0) {
        NETNATIVE_LOGE("RemoveIfaceQuota iface %s not exist, can not remove", ifName.c_str());
        return NETMANAGER_ERROR;
    } else {
        ifaceQuotaBytes_.erase(ifName);
    }

    fChainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    std::string cChainName = FetchChainName(ChainType::CHAIN_OHBW_DENIED_LIST_BOX);
    command = "-t filter -D " + fChainName + " -o " + ifName + " -j " + cChainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    // -D ohbw_INPUT -i iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_INPUT);
    command = "-t filter -D " + fChainName + " -i " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -D ohbw_OUTPUT -o iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_OUTPUT);
    command = "-t filter -D " + fChainName + " -o " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -D ohbw_FORWARD -i iface -j ohbw_costly_iface
    // -D ohbw_FORWARD -o iface -j ohbw_costly_iface
    fChainName = FetchChainName(ChainType::CHAIN_OHBW_FORWARD);
    command = "-t filter -D " + fChainName + " -i " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -D " + fChainName + " -o " + ifName + " --jump " + chainName;
    hasError = hasError ||
               (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    // -F ohbw_costly_iface
    // -X ohbw_costly_iface
    hasError = hasError || (IptablesDeleteChain(chainName) == NETMANAGER_ERROR);

    return hasError ? NETMANAGER_ERROR : NETMANAGER_SUCCESS;
}

int32_t BandwidthManager::AddDeniedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthManager AddDeniedList: uid=%{public}d", uid);
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    if (std::find(deniedListUids_.begin(), deniedListUids_.end(), uid) != deniedListUids_.end()) {
        NETNATIVE_LOGE("DeniedList uid exist, do not repeat");
        return NETMANAGER_ERROR;
    } else {
        deniedListUids_.push_back(uid);
    }

    std::string strUid = std::to_string(uid);
    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_DENIED_LIST_BOX);
    command = "-t filter -I " + chainName + " -m owner --uid-owner " + strUid + " -j REJECT";

    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t BandwidthManager::RemoveDeniedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthManager RemoveDeniedList: uid=%{public}d", uid);
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    std::vector<uint32_t>::iterator iter = std::find(deniedListUids_.begin(), deniedListUids_.end(), uid);
    if (iter == deniedListUids_.end()) {
        NETNATIVE_LOGE("AllowedList uid is not exist, can not remove");
        return NETMANAGER_ERROR;
    }
    deniedListUids_.erase(iter);

    std::string strUid = std::to_string(uid);
    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_DENIED_LIST_BOX);
    command = "-t filter -D " + chainName + " -m owner --uid-owner " + strUid + " -j REJECT";

    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t BandwidthManager::AddAllowedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthManager AddAllowedList: uid=%{public}d", uid);
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    if (std::find(allowedListUids_.begin(), allowedListUids_.end(), uid) != allowedListUids_.end()) {
        NETNATIVE_LOGE("AllowedList uid exist, do not repeat");
        return NETMANAGER_ERROR;
    } else {
        allowedListUids_.push_back(uid);
    }

    std::string strUid = std::to_string(uid);
    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX);
    command = "-t filter -I " + chainName + " -m owner --uid-owner " + strUid + " -j RETURN";

    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t BandwidthManager::RemoveAllowedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthManager RemoveAllowedList: uid=%{public}d", uid);
    std::unique_lock<std::mutex> lock(bandwidthMutex_);
    CheckChainInitialization();

    std::vector<uint32_t>::iterator iter = std::find(allowedListUids_.begin(), allowedListUids_.end(), uid);
    if (iter == allowedListUids_.end()) {
        NETNATIVE_LOGE("AllowedList uid exist, can not remove");
        return NETMANAGER_ERROR;
    } else {
        allowedListUids_.erase(iter);
    }

    std::string strUid = std::to_string(uid);

    std::string command;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX);
    command = "-t filter -D " + chainName + " -m owner --uid-owner " + strUid + " -j RETURN";

    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}
} // namespace nmd
} // namespace OHOS
