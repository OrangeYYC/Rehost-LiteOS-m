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

#include "sharing_manager.h"

#include <cerrno>
#include <fcntl.h>
#include <regex>
#include <unistd.h>

#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "route_manager.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
namespace {
constexpr const char *IPV4_FORWARDING_PROC_FILE = "/proc/sys/net/ipv4/ip_forward";
constexpr const char *IPV6_FORWARDING_PROC_FILE = "/proc/sys/net/ipv6/conf/all/forwarding";
constexpr const char *IPTABLES_TMP_BAK = "/data/service/el1/public/netmanager/ipfwd.bak";

constexpr const int MAX_MATCH_SIZE = 4;
constexpr const int TWO_LIST_CORRECT_DATA = 2;
constexpr const int NEXT_LIST_CORRECT_DATA = 1;
constexpr uint32_t NET_TRAFFIC_RESULT_INDEX_OFFSET = 2;

// commands of create tables
constexpr const char *CREATE_TETHERCTRL_NAT_POSTROUTING = "-t nat -N tetherctrl_nat_POSTROUTING";
constexpr const char *CREATE_TETHERCTRL_FORWARD = "-t filter -N tetherctrl_FORWARD";
constexpr const char *CREATE_TETHERCTRL_COUNTERS = "-t filter -N tetherctrl_counters";
constexpr const char *CREATE_TETHERCTRL_MANGLE_FORWARD = "-t mangle -N tetherctrl_mangle_FORWARD";

// commands of set nat
constexpr const char *APPEND_NAT_POSTROUTING = "-t nat -A POSTROUTING -j tetherctrl_nat_POSTROUTING";
constexpr const char *APPEND_MANGLE_FORWARD = "-t mangle -A FORWARD -j tetherctrl_mangle_FORWARD";
constexpr const char *APPEND_TETHERCTRL_MANGLE_FORWARD =
    "-t mangle -A tetherctrl_mangle_FORWARD "
    "-p tcp -m tcp --tcp-flags SYN SYN -j TCPMSS --clamp-mss-to-pmtu";
constexpr const char *CLEAR_TETHERCTRL_NAT_POSTROUTING = "-t nat -F tetherctrl_nat_POSTROUTING";
constexpr const char *CLEAR_TETHERCTRL_MANGLE_FORWARD = "-t mangle -F tetherctrl_mangle_FORWARD";
constexpr const char *DELETE_TETHERCTRL_NAT_POSTROUTING = "-t nat -D POSTROUTING -j tetherctrl_nat_POSTROUTING";
constexpr const char *DELETE_TETHERCTRL_MANGLE_FORWARD = "-t mangle -D FORWARD -j tetherctrl_mangle_FORWARD";

constexpr const char *IPATBLES_RESTORE_CMD_PATH = "/system/bin/iptables-restore";
constexpr const char *IPATBLES_SAVE_CMD_PATH = "/system/bin/iptables-save";

const std::string EnableNatCmd(const std::string &down)
{
    return "-t nat -A tetherctrl_nat_POSTROUTING -o " + down + " -j MASQUERADE";
}

// commands of set ipfwd, all commands with filter
constexpr const char *FORWARD_JUMP_TETHERCTRL_FORWARD = " FORWARD -j tetherctrl_FORWARD";
constexpr const char *SET_TETHERCTRL_FORWARD_DROP = " tetherctrl_FORWARD -j DROP";
const std::string SetTetherctrlForward1(const std::string &from, const std::string &to)
{
    return " tetherctrl_FORWARD -i " + from + " -o " + to +
           " -m state --state RELATED,ESTABLISHED"
           " -g tetherctrl_counters";
}

const std::string SetTetherctrlForward2(const std::string &from, const std::string &to)
{
    return " tetherctrl_FORWARD -i " + to + " -o " + from + " -m state --state INVALID -j DROP";
}

const std::string SetTetherctrlForward3(const std::string &from, const std::string &to)
{
    return " tetherctrl_FORWARD -i " + to + " -o " + from + " -g tetherctrl_counters";
}

const std::string SetTetherctrlCounters1(const std::string &from, const std::string &to)
{
    return " tetherctrl_counters -i " + to + " -o " + from + " -j RETURN";
}

const std::string SetTetherctrlCounters2(const std::string &from, const std::string &to)
{
    return " tetherctrl_counters -i " + from + " -o " + to + " -j RETURN";
}

bool WriteToFile(const char *fileName, const char *value)
{
    if (fileName == nullptr) {
        return false;
    }
    int fd = open(fileName, O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        NETNATIVE_LOGE("failed to open %{private}s: %{public}s", fileName, strerror(errno));
        return false;
    }

    const ssize_t len = strlen(value);
    if (write(fd, value, len) != len) {
        NETNATIVE_LOGE("faield to write %{public}s to %{private}s: %{public}s", value, fileName, strerror(errno));
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

void Rollback()
{
    NETNATIVE_LOGE("iptables rollback");
    std::string rollBak = std::string(IPATBLES_RESTORE_CMD_PATH) + " -T filter < ";
    rollBak.append(IPTABLES_TMP_BAK);
    CommonUtils::ForkExec(rollBak);
}
} // namespace

SharingManager::SharingManager()
{
    iptablesWrapper_ = DelayedSingleton<IptablesWrapper>::GetInstance();
}

void SharingManager::InitChildChains()
{
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, CREATE_TETHERCTRL_NAT_POSTROUTING);
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, CREATE_TETHERCTRL_FORWARD);
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, CREATE_TETHERCTRL_COUNTERS);
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, CREATE_TETHERCTRL_MANGLE_FORWARD);
    inited_ = true;
}

int32_t SharingManager::IpEnableForwarding(const std::string &requestor)
{
    NETNATIVE_LOG_D("IpEnableForwarding requestor: %{public}s", requestor.c_str());
    forwardingRequests_.insert(requestor);
    return SetIpFwdEnable();
}

int32_t SharingManager::IpDisableForwarding(const std::string &requestor)
{
    NETNATIVE_LOG_D("IpDisableForwarding requestor: %{public}s", requestor.c_str());
    forwardingRequests_.erase(requestor);
    return SetIpFwdEnable();
}

int32_t SharingManager::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    CheckInited();
    if (downstreamIface == upstreamIface) {
        NETNATIVE_LOGE("Duplicate interface specified: %{public}s %{public}s", downstreamIface.c_str(),
                       upstreamIface.c_str());
        return -1;
    }
    if (!CommonUtils::CheckIfaceName(upstreamIface)) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s", upstreamIface.c_str());
        return -1;
    }
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, APPEND_NAT_POSTROUTING);
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, APPEND_MANGLE_FORWARD);

    NETNATIVE_LOGI("EnableNat downstreamIface: %{public}s, upstreamIface: %{public}s", downstreamIface.c_str(),
                   upstreamIface.c_str());

    if (iptablesWrapper_->RunCommand(IPTYPE_IPV4, EnableNatCmd(upstreamIface)) !=
        NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("IptablesWrapper run command failed");
        return -1;
    }

    if (iptablesWrapper_->RunCommand(IPTYPE_IPV4, APPEND_TETHERCTRL_MANGLE_FORWARD) !=
        NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("IptablesWrapper run command failed");
        return -1;
    }
    return 0;
}

int32_t SharingManager::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    CheckInited();
    if (downstreamIface == upstreamIface) {
        NETNATIVE_LOGE("Duplicate interface specified: %{public}s %s", downstreamIface.c_str(),
                       upstreamIface.c_str());
        return -1;
    }
    if (!CommonUtils::CheckIfaceName(upstreamIface)) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s", upstreamIface.c_str());
        return -1;
    }

    NETNATIVE_LOGI("DisableNat downstreamIface: %{public}s, upstreamIface: %{public}s", downstreamIface.c_str(),
                   upstreamIface.c_str());

    if (iptablesWrapper_->RunCommand(IPTYPE_IPV4, CLEAR_TETHERCTRL_NAT_POSTROUTING) !=
        NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("IptablesWrapper run command failed");
        return -1;
    }
    if (iptablesWrapper_->RunCommand(IPTYPE_IPV4, CLEAR_TETHERCTRL_MANGLE_FORWARD) !=
        NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("IptablesWrapper run command failed");
        return -1;
    }

    iptablesWrapper_->RunCommand(IPTYPE_IPV4, DELETE_TETHERCTRL_NAT_POSTROUTING);
    iptablesWrapper_->RunCommand(IPTYPE_IPV4, DELETE_TETHERCTRL_MANGLE_FORWARD);
    return 0;
}

int32_t SharingManager::SetIpFwdEnable()
{
    bool disable = forwardingRequests_.empty();
    const char *value = disable ? "0" : "1";
    bool ipv4Success = WriteToFile(IPV4_FORWARDING_PROC_FILE, value);
    bool ipv6Success = WriteToFile(IPV6_FORWARDING_PROC_FILE, value);
    return (ipv4Success && ipv6Success) ? 0 : -1;
}

int32_t SharingManager::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    CheckInited();
    if (fromIface == toIface) {
        NETNATIVE_LOGE("Duplicate interface specified: %{public}s %{public}s", fromIface.c_str(), toIface.c_str());
        return -1;
    }
    if (!(CommonUtils::CheckIfaceName(fromIface)) || !(CommonUtils::CheckIfaceName(toIface))) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s %{public}s", fromIface.c_str(), toIface.c_str());
        return -1;
    }
    NETNATIVE_LOGI("IpfwdAddInterfaceForward fromIface: %{public}s, toIface: %{public}s", fromIface.c_str(),
                   toIface.c_str());

    if (interfaceForwards_.empty()) {
        SetForwardRules(true, FORWARD_JUMP_TETHERCTRL_FORWARD);
    }
    int32_t result = 0;
    std::string saveBak = std::string(IPATBLES_SAVE_CMD_PATH) + " -t filter > ";
    saveBak.append(IPTABLES_TMP_BAK);
    CommonUtils::ForkExec(saveBak);

    /*
     * Add a forward rule, when the status of packets is RELATED,
     * ESTABLISED and from fromIface to toIface, goto tetherctrl_counters
     */
    result = SetForwardRules(true, SetTetherctrlForward1(toIface, fromIface));
    if (result) {
        return result;
    }

    /*
     * Add a forward rule, when the status is INVALID and from toIface to fromIface, just drop
     */
    result = SetForwardRules(true, SetTetherctrlForward2(toIface, fromIface));
    if (result) {
        Rollback();
        return result;
    }

    /*
     * Add a forward rule, from toIface to fromIface, goto tetherctrl_counters
     */
    result = SetForwardRules(true, SetTetherctrlForward3(toIface, fromIface));
    if (result) {
        Rollback();
        return result;
    }

    /*
     * Add a forward rule, drop others
     */
    result = SetForwardRules(true, SET_TETHERCTRL_FORWARD_DROP);
    if (result) {
        Rollback();
        return result;
    }

    /*
     * Add a forward rule, if from toIface to fromIface return chain of father
     */
    result = SetForwardRules(true, SetTetherctrlCounters1(fromIface, toIface));
    if (result) {
        Rollback();
        return result;
    }

    /*
     * Add a forward rule, if from fromIface to toIface return chain of father
     */
    result = SetForwardRules(true, SetTetherctrlCounters2(fromIface, toIface));
    if (result) {
        Rollback();
        return result;
    }

    result = RouteManager::EnableSharing(fromIface, toIface);
    if (result) {
        Rollback();
        return result;
    }
    interfaceForwards_.insert(fromIface + toIface);
    return 0;
}

int32_t SharingManager::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    CheckInited();
    if (fromIface == toIface) {
        NETNATIVE_LOGE("Duplicate interface specified: %{public}s %{public}s", fromIface.c_str(), toIface.c_str());
        return -1;
    }
    if (!(CommonUtils::CheckIfaceName(fromIface)) || !(CommonUtils::CheckIfaceName(toIface))) {
        NETNATIVE_LOGE("iface name valid check fail: %{public}s %{public}s", fromIface.c_str(), toIface.c_str());
        return -1;
    }
    NETNATIVE_LOGI("IpfwdRemoveInterfaceForward fromIface: %{public}s, toIface: %{public}s", fromIface.c_str(),
                   toIface.c_str());

    SetForwardRules(false, SetTetherctrlForward1(toIface, fromIface));
    SetForwardRules(false, SetTetherctrlForward2(toIface, fromIface));
    SetForwardRules(false, SetTetherctrlForward3(toIface, fromIface));
    SetForwardRules(false, SET_TETHERCTRL_FORWARD_DROP);
    SetForwardRules(false, SetTetherctrlCounters1(fromIface, toIface));
    SetForwardRules(false, SetTetherctrlCounters2(fromIface, toIface));

    RouteManager::DisableSharing(fromIface, toIface);

    interfaceForwards_.erase(fromIface + toIface);
    if (interfaceForwards_.empty()) {
        SetForwardRules(false, FORWARD_JUMP_TETHERCTRL_FORWARD);
    }

    return 0;
}

int32_t SharingManager::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                 NetworkSharingTraffic &traffic)
{
    const std::string cmds = "-t filter -L tetherctrl_counters -nvx";
    std::string result = iptablesWrapper_->RunCommandForRes(IPTYPE_IPV4, cmds);

    const std::string num = "(\\d+)";
    const std::string iface = "([^\\s]+)";
    const std::string dst = "(0.0.0.0/0|::/0)";
    const std::string counters = "\\s*" + num + "\\s+" + num + " RETURN     all(  --  |      )" + iface + "\\s+" +
                                 iface + "\\s+" + dst + "\\s+" + dst;
    static const std::regex IP_RE(counters);

    bool isFindTx = false;
    bool isFindRx = false;
    const std::vector<std::string> lines = CommonUtils::Split(result, "\n");
    for (auto line : lines) {
        std::smatch matches;
        std::regex_search(line, matches, IP_RE);
        if (matches.size() < MAX_MATCH_SIZE) {
            continue;
        }
        for (uint32_t i = 0; i < matches.size() - 1; i++) {
            std::string tempMatch = matches[i];
            NETNATIVE_LOG_D("GetNetworkSharingTraffic matche[%{public}s]", tempMatch.c_str());
            if (matches[i] == downIface && matches[i + NEXT_LIST_CORRECT_DATA] == upIface &&
                ((i - TWO_LIST_CORRECT_DATA) >= 0)) {
                int64_t send =
                    static_cast<int64_t>(strtoul(matches[i - TWO_LIST_CORRECT_DATA].str().c_str(), nullptr, 0));
                isFindTx = true;
                traffic.send = send;
                traffic.all += send;
            } else if (matches[i] == upIface && matches[i + NEXT_LIST_CORRECT_DATA] == downIface &&
                       ((i - NET_TRAFFIC_RESULT_INDEX_OFFSET) >= 0)) {
                int64_t receive =
                    static_cast<int64_t>(strtoul(matches[i - TWO_LIST_CORRECT_DATA].str().c_str(), nullptr, 0));
                isFindRx = true;
                traffic.receive = receive;
                traffic.all += receive;
            }
            if (isFindTx && isFindRx) {
                NETNATIVE_LOG_D("GetNetworkSharingTraffic success total");
                return NETMANAGER_SUCCESS;
            }
        }
    }
    NETNATIVE_LOGE("GetNetworkSharingTraffic failed");
    return NETMANAGER_ERROR;
}

void SharingManager::CheckInited()
{
    std::lock_guard<std::mutex> guard(initedMutex_);
    if (inited_) {
        return;
    }
    InitChildChains();
}

int32_t SharingManager::SetForwardRules(bool set, const std::string &cmds)
{
    const std::string op = set ? "-A" : "-D";

    if (iptablesWrapper_->RunCommand(IPTYPE_IPV4, "-t filter " + op + cmds) !=
        NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("IptablesWrapper run command failed");
        return -1;
    }
    return 0;
}
} // namespace nmd
} // namespace OHOS
