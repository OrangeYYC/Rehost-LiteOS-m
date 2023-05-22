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

#include "firewall_manager.h"

#include <fstream>
#include <sstream>

#include "iptables_wrapper.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
namespace {
static constexpr const char *CONFIG_FILE_PATH = "/proc/self/uid_map";
bool SetFireWallCommand(std::string chainName, std::string command)
{
    bool ret = false;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    command = "-t filter -A " + chainName + " -i lo -j RETURN";
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -A " + chainName + " -o lo -j RETURN";
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -A " + chainName + " -p tcp --tcp-flags RST RST -j RETURN";
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return ret;
}
} // namespace

static constexpr uint32_t SYSTEM_UID_RANGE = 9999;
static constexpr uint32_t DEFAULT_MAX_UID_RANGE = UINT_MAX;
FirewallManager::FirewallManager() : chainInitFlag_(false), firewallType_(FirewallType::TYPE_ALLOWED_LIST)
{
    strMaxUid_ = ReadMaxUidConfig();
    FirewallChainStatus status = {};
    status.enable = false;
    status.type = FirewallType::TYPE_ALLOWED_LIST;
    firewallChainStatus_[ChainType::CHAIN_OHFW_DOZABLE] = status;
    firewallChainStatus_[ChainType::CHAIN_OHFW_UNDOZABLE] = status;
}

FirewallManager::~FirewallManager()
{
    DeInitChain();
}

inline void FirewallManager::CheckChainInitialization()
{
    if (chainInitFlag_ == false) {
        InitChain();
        InitDefaultRules();
    }
}

std::string FirewallManager::ReadMaxUidConfig()
{
    NETNATIVE_LOG_D("FirewallManager::ReadMaxUidConfig");
    std::string maxUid;
    std::string content;

    std::ifstream file(CONFIG_FILE_PATH);
    if (!file.is_open()) {
        NETNATIVE_LOGE("FirewallManager::ReadMaxUidConfig fstream failed");
        return std::to_string(DEFAULT_MAX_UID_RANGE);
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    content = oss.str();
    auto index = content.find_last_of(' ');
    maxUid = content.substr(index + 1);
    // if unavailable value use default value
    if (maxUid.size() == 0) {
        return std::to_string(DEFAULT_MAX_UID_RANGE);
    }

    return maxUid;
}

int32_t FirewallManager::IsFirewallChian(ChainType chain)
{
    if (chain != ChainType::CHAIN_OHFW_DOZABLE && chain != ChainType::CHAIN_OHFW_UNDOZABLE) {
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

std::string FirewallManager::FetchChainName(ChainType chain)
{
    NETNATIVE_LOG_D("FirewallManager FetchChainName: chain=%{public}d", chain);
    std::string chainName;
    switch (chain) {
        case ChainType::CHAIN_OHFW_INPUT:
            chainName = "ohfw_INPUT";
            break;
        case ChainType::CHAIN_OHFW_OUTPUT:
            chainName = "ohfw_OUTPUT";
            break;
        case ChainType::CHAIN_OHFW_FORWARD:
            chainName = "ohfw_FORWARD";
            break;
        case ChainType::CHAIN_OHFW_DOZABLE:
            chainName = "ohfw_dozable";
            break;
        case ChainType::CHAIN_OHFW_UNDOZABLE:
            chainName = "ohfw_undozable";
            break;
        default:
            chainName = "oh_unusable";
            break;
    }
    return chainName;
}

FirewallType FirewallManager::FetchChainType(ChainType chain)
{
    FirewallType firewallType;
    switch (chain) {
        case ChainType::CHAIN_OHFW_DOZABLE:
            firewallType = FirewallType::TYPE_ALLOWED_LIST;
            break;
        case ChainType::CHAIN_OHFW_UNDOZABLE:
            firewallType = FirewallType::TYPE_DENIDE_LIST;
            break;
        default:
            firewallType = FirewallType::TYPE_ALLOWED_LIST;
            break;
    }
    return firewallType;
}

int32_t FirewallManager::InitChain()
{
    NETNATIVE_LOG_D("FirewallManager InitChain");
    bool ret = false;
    ret = (IptablesNewChain(ChainType::CHAIN_OHFW_INPUT) == NETMANAGER_ERROR) ||
          (IptablesNewChain(ChainType::CHAIN_OHFW_OUTPUT) == NETMANAGER_ERROR) ||
          (IptablesNewChain(ChainType::CHAIN_OHFW_FORWARD) == NETMANAGER_ERROR) ||
          (IptablesNewChain(ChainType::CHAIN_OHFW_DOZABLE) == NETMANAGER_ERROR) ||
          (IptablesNewChain(ChainType::CHAIN_OHFW_UNDOZABLE) == NETMANAGER_ERROR);
    chainInitFlag_ = true;
    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::DeInitChain()
{
    NETNATIVE_LOG_D("FirewallManager DeInitChain");
    bool ret = false;
    ret = (IptablesDeleteChain(ChainType::CHAIN_OHFW_INPUT) == NETMANAGER_ERROR) ||
          (IptablesDeleteChain(ChainType::CHAIN_OHFW_OUTPUT) == NETMANAGER_ERROR) ||
          (IptablesDeleteChain(ChainType::CHAIN_OHFW_FORWARD) == NETMANAGER_ERROR) ||
          (IptablesDeleteChain(ChainType::CHAIN_OHFW_DOZABLE) == NETMANAGER_ERROR) ||
          (IptablesDeleteChain(ChainType::CHAIN_OHFW_UNDOZABLE) == NETMANAGER_ERROR);
    chainInitFlag_ = false;
    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::InitDefaultRules()
{
    NETNATIVE_LOG_D("FirewallManager InitDefaultRules");
    bool ret = false;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHFW_INPUT);
    std::string command = "-t filter -A INPUT -j " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHFW_OUTPUT);
    command = "-t filter -A OUTPUT -j " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::ClearAllRules()
{
    NETNATIVE_LOG_D("FirewallManager ClearAllRules");
    bool ret = false;
    std::string chainName = FetchChainName(ChainType::CHAIN_OHFW_INPUT);
    std::string command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHFW_OUTPUT);
    command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHFW_FORWARD);
    command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHFW_DOZABLE);
    command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    chainName = FetchChainName(ChainType::CHAIN_OHFW_DOZABLE);
    command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::IptablesNewChain(ChainType chain)
{
    NETNATIVE_LOG_D("FirewallManager NewChain: chain=%{public}d", chain);
    std::string command = "-t filter -N " + FetchChainName(chain);
    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t FirewallManager::IptablesDeleteChain(ChainType chain)
{
    NETNATIVE_LOG_D("FirewallManager DeleteChain: chain=%{public}d", chain);
    bool ret = false;
    std::string command = "-t filter -F " + FetchChainName(chain);
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -X " + FetchChainName(chain);
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::IptablesSetRule(const std::string &chainName, const std::string &option,
                                         const std::string &target, uint32_t uid)
{
    NETNATIVE_LOG_D("FirewallManager IptablesSetRule");
    std::string command =
        "-t filter " + option + " " + chainName + " -m owner --uid-owner " + std::to_string(uid) + " -j " + target;
    return DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command);
}

int32_t FirewallManager::SetUidsAllowedListChain(ChainType chain, const std::vector<uint32_t> &uids)
{
    NETNATIVE_LOG_D("FirewallManager SetUidsAllowedListChain: chain=%{public}d", chain);
    if (chain != ChainType::CHAIN_OHFW_DOZABLE) {
        return NETMANAGER_ERROR;
    }

    bool ret = false;
    std::unique_lock<std::mutex> lock(firewallMutex_);
    CheckChainInitialization();

    std::string command;
    const auto &chainName = FetchChainName(chain);

    command = "-t filter -F " + chainName;
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);

    std::for_each(uids.begin(), uids.end(), [&command, &chainName, &ret](uint32_t uid) {
        std::string strUid = std::to_string(uid);
        command = "-t filter -A " + chainName + " -m owner --uid-owner " + strUid + " -j RETURN";
        ret = ret || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                      NETMANAGER_ERROR);
    });

    command =
        "-t filter -A " + chainName + " -m owner --uid-owner 0-" + std::to_string(SYSTEM_UID_RANGE) + " -j RETURN";
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    command = "-t filter -A " + chainName + " -m owner ! --uid-owner 0-" + strMaxUid_ + " -j RETURN";

    ret = SetFireWallCommand(chainName, command);

    command = "-t filter -A " + chainName + " -j DROP";
    ret = ret ||
          (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) == NETMANAGER_ERROR);
    if (ret == false) {
        FirewallChainStatus status = firewallChainStatus_[chain];
        status.uids = uids;
        firewallChainStatus_[chain] = status;
    }

    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::SetUidsDeniedListChain(ChainType chain, const std::vector<uint32_t> &uids)
{
    NETNATIVE_LOG_D("FirewallManager SetUidsDeniedListChain: chain=%{public}d", chain);
    if (chain != ChainType::CHAIN_OHFW_UNDOZABLE) {
        return NETMANAGER_ERROR;
    }

    std::unique_lock<std::mutex> lock(firewallMutex_);
    bool ret = false;
    CheckChainInitialization();

    std::string command;
    const auto &chainName = FetchChainName(chain);

    command = "-t filter -F " + chainName;

    ret = SetFireWallCommand(chainName, command);

    std::for_each(uids.begin(), uids.end(), [&command, &chainName, &ret](uint32_t uid) {
        std::string strUid = std::to_string(uid);
        command = "-t filter -A " + chainName + " -m owner --uid-owner " + strUid + " -j DROP";
        ret = ret || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                      NETMANAGER_ERROR);
    });

    if (ret == false) {
        FirewallChainStatus status = firewallChainStatus_[chain];
        status.uids = uids;
        firewallChainStatus_[chain] = status;
    }

    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::EnableChain(ChainType chain, bool enable)
{
    NETNATIVE_LOG_D("FirewallManager EnableChain: chain=%{public}d, enable=%{public}d", chain, enable);
    if (IsFirewallChian(chain) == NETMANAGER_ERROR) {
        return NETMANAGER_ERROR;
    }

    bool ret = false;
    std::unique_lock<std::mutex> lock(firewallMutex_);
    CheckChainInitialization();

    std::string chainName;
    std::string fChainName;
    chainName = FetchChainName(chain);
    std::string command;

    if (enable == true && firewallChainStatus_[chain].enable == false) {
        fChainName = FetchChainName(ChainType::CHAIN_OHFW_OUTPUT);
        command = "-t filter -A " + fChainName + " -j " + chainName;
        ret = ret || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                      NETMANAGER_ERROR);
    } else if (enable == false && firewallChainStatus_[chain].enable == true) {
        fChainName = FetchChainName(ChainType::CHAIN_OHFW_OUTPUT);
        command = "-t filter -D " + fChainName + " -j " + chainName;
        ret = ret || (DelayedSingleton<IptablesWrapper>::GetInstance()->RunCommand(IPTYPE_IPV4, command) ==
                      NETMANAGER_ERROR);
    } else {
        NETNATIVE_LOGI("FirewallManager::EnableChain chain was %{public}s, do not repeat",
                       enable == true ? "true" : "false");
        return NETMANAGER_ERROR;
    }

    if (ret == false) {
        firewallType_ = FetchChainType(chain);
        firewallChainStatus_[chain].enable = enable;
    }

    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t FirewallManager::SetUidRule(ChainType chain, uint32_t uid, FirewallRule firewallRule)
{
    NETNATIVE_LOG_D("FirewallManager SetUidRule: chain=%{public}d, uid=%{public}d, firewallRule=%{public}d", chain,
                    uid, firewallRule);
    if (IsFirewallChian(chain) == NETMANAGER_ERROR) {
        return NETMANAGER_ERROR;
    }

    bool ret = false;
    std::unique_lock<std::mutex> lock(firewallMutex_);
    CheckChainInitialization();

    std::string op;
    std::string target;
    std::string chainName = FetchChainName(chain);
    FirewallType firewallType = FetchChainType(chain);
    if (firewallType == FirewallType::TYPE_DENIDE_LIST) {
        target = "DROP";
        op = (firewallRule == FirewallRule::RULE_DENY) ? "-A" : "-D";
    } else {
        target = "RETURN";
        op = (firewallRule == FirewallRule::RULE_ALLOW) ? "-I" : "-D";
    }

    FirewallChainStatus status = firewallChainStatus_[chain];
    std::vector<uint32_t>::iterator iter = std::find(status.uids.begin(), status.uids.end(), uid);
    if (op != "-D" && iter == status.uids.end()) {
        status.uids.push_back(uid);
        ret = ret || (IptablesSetRule(chainName, op, target, uid) == NETMANAGER_ERROR);
    } else if (op == "-D" && iter != status.uids.end()) {
        status.uids.erase(iter);
        ret = ret || (IptablesSetRule(chainName, op, target, uid) == NETMANAGER_ERROR);
    } else {
        NETNATIVE_LOGE("FirewallManager::SetUidRule error");
        return NETMANAGER_ERROR;
    }

    if (ret == false) {
        firewallChainStatus_[chain] = status;
    }

    return ret == false ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}
} // namespace nmd
} // namespace OHOS
