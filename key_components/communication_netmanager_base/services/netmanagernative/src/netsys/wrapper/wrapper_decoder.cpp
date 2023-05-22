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

#include "wrapper_decoder.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <vector>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <linux/genetlink.h>
#include <linux/if_addr.h>
#include <linux/if_link.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_log.h>

#include "netlink_define.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard::CommonUtils;
using namespace NetlinkDefine;
namespace {
constexpr int16_t LOCAL_QLOG_NL_EVENT = 112;
constexpr int16_t LOCAL_NFLOG_PACKET = NFNL_SUBSYS_ULOG << 8 | NFULNL_MSG_PACKET;

constexpr int16_t ULOG_MAC_LEN = 80;
constexpr int16_t ULOG_PREFIX_LEN = 32;

// The message key for Ascii decode.
constexpr const char *SYMBOL_AT = "@";
constexpr const char *SYMBOL_EQUAL = "=";
constexpr const char *KEY_ACTION = "ACTION=";
constexpr const char *KEY_SEQNUM = "SEQNUM=";
constexpr const char *KEY_SUBSYSTEM = "SUBSYSTEM=";
constexpr const char *VALUE_ADD = "add";
constexpr const char *VALUE_REMOVE = "remove";
constexpr const char *VALUE_CHANGE = "change";
constexpr const char *SYMBOL_ADDRESS_SPLIT = "_";
constexpr const char *ADDRESS_SPLIT = "-";
constexpr const char *ROUTE_DEFAULT_ADDR = "0.0.0.0";
constexpr const char *ROUTE_DEFAULT_SPLIT = "::";
constexpr const char *ROUTE_ADDRESS_SPLIT = "/";
constexpr const char *RTA_GATEWAY_STR = "RTA_GATEWAY";
constexpr const char *RTA_DST_STR = "RTA_DST";
constexpr const char *RTA_OIF_STR = "RTA_OIF";

struct ULogMessage {
    uint64_t mark;
    int64_t timeStampSec;
    int64_t timeStampUsec;
    uint32_t hook;
    char indevName[IFNAMSIZ];
    char outdevName[IFNAMSIZ];
    size_t dataLen;
    char prefix[ULOG_PREFIX_LEN];
    uint8_t macLen;
    uint8_t mac[ULOG_MAC_LEN];
    uint8_t payload[0];
};

const std::map<int32_t, std::string> MSG_NAME_MAP = {
    {RTM_NEWLINK, "RTM_NEWLINK"},
    {RTM_DELLINK, "RTM_DELLINK"},
    {RTM_NEWADDR, "RTM_NEWADDR"},
    {RTM_DELADDR, "RTM_DELADDR"},
    {RTM_NEWROUTE, "RTM_NEWROUTE"},
    {RTM_DELROUTE, "RTM_DELROUTE"},
    {RTM_NEWNDUSEROPT, "RTM_NEWNDUSEROPT"},
    {LOCAL_QLOG_NL_EVENT, "LOCAL_QLOG_NL_EVENT"},
    {LOCAL_NFLOG_PACKET, "LOCAL_NFLOG_PACKET"},
};

struct ServerInfo {
    std::string ipAddr;
    std::string ifName;
    ServerInfo(const std::string &ipAddr, const std::string &ifName) : ipAddr(ipAddr), ifName(ifName) {}
};

struct ServerList {
    std::vector<ServerInfo> infos;
    std::string Serialize()
    {
        std::string str;
        for (const auto &info : infos) {
            str += info.ipAddr + SYMBOL_ADDRESS_SPLIT;
            if (!info.ifName.empty()) {
                str += info.ifName + ADDRESS_SPLIT;
            }
        }
        return str;
    }

    void Add(const std::string &ipAddr, const std::string &ifName)
    {
        infos.emplace_back(ipAddr, ifName);
    }
};

const std::string CastNameToStr(int32_t form)
{
    const auto &itr = MSG_NAME_MAP.find(form);
    if (itr == MSG_NAME_MAP.end()) {
        return {};
    }
    return itr->second;
}

bool CheckRtNetlinkLength(const nlmsghdr *hdrMsg, size_t size)
{
    int32_t type = hdrMsg->nlmsg_type;
    bool ret = hdrMsg->nlmsg_len >= NLMSG_LENGTH(int(size));
    if (!ret) {
        if (MSG_NAME_MAP.find(type) != MSG_NAME_MAP.end()) {
            const std::string &netlinkType = MSG_NAME_MAP.at(type);
            NETNATIVE_LOGE("Got a short %{public}s message\n", netlinkType.c_str());
        }
    }
    return ret;
}

inline bool IsDataEmpty(bool isValid, const std::string &attrName, const std::string &msgName)
{
    if (isValid) {
        NETNATIVE_LOGE("Error Msg Repeated: attrName : %{public}s msgName: %{public}s", attrName.c_str(),
                       msgName.c_str());
    }
    return !isValid;
}

inline bool IsPayloadValidated(rtattr *dest, uint32_t size)
{
    int destLen = RTA_PAYLOAD(dest);
    int32_t rtaSize = static_cast<int32_t>(size);
    bool ret = destLen >= rtaSize;
    if (!ret) {
        NETNATIVE_LOGE("Short IFA_CACHEINFO dest = %{public}d, size = %{public}d", destLen, rtaSize);
    }
    return ret;
}

const std::map<std::string, NetsysEventMessage::Type> ASCII_PARAM_LIST = {
    {"IFINDEX", NetsysEventMessage::Type::IFINDEX},
    {"INTERFACE", NetsysEventMessage::Type::INTERFACE}};

const std::map<std::string, NetsysEventMessage::SubSys> SUB_SYS_LIST = {
    {"net", NetsysEventMessage::SubSys::NET},
};
} // namespace

WrapperDecoder::WrapperDecoder(std::shared_ptr<NetsysEventMessage> message) : message_(message) {}

bool WrapperDecoder::DecodeAscii(const char *buffer, int32_t buffSize)
{
    std::string buf = buffer;
    const char *start = buffer;
    const char *end = start + buffSize;
    std::vector<std::string> recvmsg;
    if (buffSize == 0) {
        return false;
    }
    const auto msg = Split(buf, SYMBOL_AT);
    const std::string path = msg[1];
    if (path.empty()) {
        return false;
    }
    const auto action = msg[0];
    if (action.empty()) {
        return false;
    }

    // Skip the first line.
    start += strlen(start) + 1;
    while (start < end) {
        if (start != nullptr) {
            recvmsg.emplace_back(start);
        }
        // Skip to next line.
        start += strlen(start) + 1;
    }

    // Split the message and push them into params.
    PushAsciiMessage(recvmsg);
    return true;
}

bool WrapperDecoder::PushAsciiMessage(const std::vector<std::string> &recvmsg)
{
    for (auto &i : recvmsg) {
        if (i.compare(0, strlen(KEY_ACTION), KEY_ACTION) == 0) {
            if (i.compare(strlen(KEY_ACTION), strlen(VALUE_ADD), VALUE_ADD) == 0) {
                message_->SetAction(NetsysEventMessage::Action::ADD);
            } else if (i.compare(strlen(KEY_ACTION), strlen(VALUE_REMOVE), VALUE_REMOVE) == 0) {
                message_->SetAction(NetsysEventMessage::Action::REMOVE);
            } else if (i.compare(strlen(KEY_ACTION), strlen(VALUE_CHANGE), VALUE_CHANGE) == 0) {
                message_->SetAction(NetsysEventMessage::Action::CHANGE);
            }
        } else if (i.compare(0, strlen(KEY_SEQNUM), KEY_SEQNUM) == 0) {
            const auto seq = Split(i, SYMBOL_EQUAL);
            if (seq.size() == SPLIT_SIZE) {
                message_->SetSeq(StrToInt(seq[1]));
            }
        } else if (i.compare(0, strlen(KEY_SUBSYSTEM), KEY_SUBSYSTEM) == 0) {
            const auto subsys = Split(i, SYMBOL_EQUAL);
            if ((subsys.size() == SPLIT_SIZE) && (SUB_SYS_LIST.find(subsys[1]) != SUB_SYS_LIST.end())) {
                message_->SetSubSys(SUB_SYS_LIST.at(subsys[1]));
            }
        } else {
            SaveOtherMsg(i);
        }
    }
    return true;
}

bool WrapperDecoder::DecodeBinary(const char *buffer, int32_t buffSize)
{
    const nlmsghdr *hdrMsg = nullptr;
    bool result = false;
    for (hdrMsg = reinterpret_cast<const nlmsghdr *>(buffer);
         NLMSG_OK(hdrMsg, (unsigned)buffSize) && (hdrMsg->nlmsg_type != NLMSG_DONE);
         hdrMsg = NLMSG_NEXT(hdrMsg, buffSize)) {
        if (CastNameToStr(hdrMsg->nlmsg_type).empty()) {
            NETNATIVE_LOGW("Netlink message type is unexpected as : %{public}d\n", hdrMsg->nlmsg_type);
            continue;
        }
        switch (hdrMsg->nlmsg_type) {
            case RTM_NEWLINK:
                result = InterpreteInfoMsg(hdrMsg);
                break;
            case LOCAL_QLOG_NL_EVENT:
                result = InterpreteUlogMsg(hdrMsg);
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:
                result = InterpreteAddressMsg(hdrMsg);
                break;
            case RTM_NEWROUTE:
            case RTM_DELROUTE:
                result = InterpreteRtMsg(hdrMsg);
                break;
            default:
                result = false;
                NETNATIVE_LOG_D("message type error as :%{public}d\n", hdrMsg->nlmsg_type);
                break;
        }
        if (result) {
            return true;
        }
    }
    return false;
}

bool WrapperDecoder::InterpreteInfoMsg(const nlmsghdr *hdrMsg)
{
    if (hdrMsg == nullptr) {
        return false;
    }
    auto info = reinterpret_cast<ifinfomsg *>(NLMSG_DATA(hdrMsg));
    if (info == nullptr || !CheckRtNetlinkLength(hdrMsg, sizeof(*info)) || (info->ifi_flags & IFF_LOOPBACK) != 0) {
        return false;
    }

    int32_t len = IFLA_PAYLOAD(hdrMsg);
    rtattr *rtaInfo = nullptr;
    for (rtaInfo = IFLA_RTA(info); RTA_OK(rtaInfo, len); rtaInfo = RTA_NEXT(rtaInfo, len)) {
        if (rtaInfo == nullptr) {
            NETNATIVE_LOGE("Unavailable ifinfomsg\n");
            return false;
        }
        if (rtaInfo->rta_type == IFLA_IFNAME) {
            message_->PushMessage(NetsysEventMessage::Type::INTERFACE,
                                  std::string(reinterpret_cast<const char *>(RTA_DATA(rtaInfo))));
            message_->PushMessage(NetsysEventMessage::Type::IFINDEX, std::to_string(info->ifi_index));
            auto action = (info->ifi_flags & IFF_LOWER_UP) ? NetsysEventMessage::Action::LINKUP
                                                           : NetsysEventMessage::Action::LINKDOWN;
            message_->SetAction(action);
            message_->SetSubSys(NetsysEventMessage::SubSys::NET);
            return true;
        }
    }
    return false;
}

bool WrapperDecoder::InterpreteUlogMsg(const nlmsghdr *hdrMsg)
{
    std::string devname;
    ULogMessage *pm = reinterpret_cast<ULogMessage *>(NLMSG_DATA(hdrMsg));
    if (!CheckRtNetlinkLength(hdrMsg, sizeof(*pm))) {
        return false;
    }
    devname = pm->indevName[0] ? pm->indevName : pm->outdevName;
    message_->PushMessage(NetsysEventMessage::Type::ALERT_NAME, std::string(pm->prefix));
    message_->PushMessage(NetsysEventMessage::Type::INTERFACE, devname);
    message_->SetSubSys(NetsysEventMessage::SubSys::QLOG);
    message_->SetAction(NetsysEventMessage::Action::CHANGE);
    return true;
}

bool WrapperDecoder::InterpreteAddressMsg(const nlmsghdr *hdrMsg)
{
    ifaddrmsg *addrMsg = reinterpret_cast<ifaddrmsg *>(NLMSG_DATA(hdrMsg));
    ifa_cacheinfo *cacheInfo = nullptr;
    char addresses[INET6_ADDRSTRLEN] = "";
    char interfaceName[IFNAMSIZ] = "";
    uint32_t flags;

    if (!CheckRtNetlinkLength(hdrMsg, sizeof(*addrMsg))) {
        return false;
    }
    int32_t nlType = hdrMsg->nlmsg_type;
    if (nlType != RTM_NEWADDR && nlType != RTM_DELADDR) {
        NETNATIVE_LOGE("InterpreteAddressMsg on incorrect message nlType 0x%{public}x\n", nlType);
        return false;
    }
    const std::string rtMsgType = CastNameToStr(nlType);
    flags = addrMsg->ifa_flags;
    int32_t len = IFA_PAYLOAD(hdrMsg);
    for (rtattr *rtAttr = IFA_RTA(addrMsg); RTA_OK(rtAttr, len); rtAttr = RTA_NEXT(rtAttr, len)) {
        if (rtAttr == nullptr) {
            NETNATIVE_LOGE("Invalid ifaddrmsg\n");
            return false;
        }
        switch (rtAttr->rta_type) {
            case IFA_ADDRESS:
                if (!InterpreteIFaceAddr(addrMsg, addresses, sizeof(addresses), rtMsgType, interfaceName, rtAttr)) {
                    // This is not an error, but we don't want to continue.
                    NETNATIVE_LOG_D("InterpreteIFaceAddr failed");
                }
                break;
            case IFA_CACHEINFO:
                if (IsDataEmpty(cacheInfo, "IFA_CACHEINFO", rtMsgType) &&
                    !IsPayloadValidated(rtAttr, sizeof(*cacheInfo))) {
                    break;
                }
                cacheInfo = reinterpret_cast<ifa_cacheinfo *>(RTA_DATA(rtAttr));
                break;
            case IFA_FLAGS:
                flags = *(reinterpret_cast<uint32_t *>(RTA_DATA(rtAttr)));
                break;
            default:
                break;
        }
    }
    if (addresses[0] == '\0') {
        NETNATIVE_LOGE("IFA_ADDRESS not exist in %{public}s\n", rtMsgType.c_str());
        return false;
    }
    message_->SetAction((nlType == RTM_NEWADDR) ? NetsysEventMessage::Action::ADDRESSUPDATE
                                                : NetsysEventMessage::Action::ADDRESSREMOVED);
    return SaveAddressMsg(addresses, addrMsg, std::to_string(flags), cacheInfo, interfaceName);
}

bool WrapperDecoder::InterpreteIFaceAddr(ifaddrmsg *ifAddr, char *addrStr, socklen_t sockLen,
                                         const std::string &msgType, char *ifName, rtattr *rta)
{
    if (*addrStr != 0) {
        NETNATIVE_LOGE("IFA_ADDRESS already exist in %{public}s", msgType.c_str());
        return false;
    }

    switch (ifAddr->ifa_family) {
        case AF_INET: {
            in_addr *ipv4Addr = reinterpret_cast<in_addr *>(RTA_DATA(rta));
            if (!IsPayloadValidated(rta, sizeof(*ipv4Addr))) {
                return false;
            }
            inet_ntop(AF_INET, ipv4Addr, addrStr, sockLen);
            break;
        }
        case AF_INET6: {
            in6_addr *ipv6Addr = reinterpret_cast<in6_addr *>(RTA_DATA(rta));
            if (!IsPayloadValidated(rta, sizeof(*ipv6Addr))) {
                return false;
            }
            inet_ntop(AF_INET6, ipv6Addr, addrStr, sockLen);
            break;
        }
        default:
            NETNATIVE_LOGE("Address family is unknown %{public}d\n", ifAddr->ifa_family);
            return false;
    }

    if (!if_indextoname(ifAddr->ifa_index, ifName)) {
        NETNATIVE_LOGW("The interface index %{public}d in %{public}s is unknown", ifAddr->ifa_index, msgType.c_str());
    }
    return true;
}

bool WrapperDecoder::SaveAddressMsg(const std::string addrStr, const ifaddrmsg *addrMsg, const std::string flags,
                                    const ifa_cacheinfo *cacheInfo, const std::string interfaceName)
{
    if (addrStr.empty()) {
        NETNATIVE_LOGE("No IFA_ADDRESS");
        return false;
    }
    message_->SetSubSys(NetsysEventMessage::SubSys::NET);
    message_->PushMessage(NetsysEventMessage::Type::INTERFACE, interfaceName);
    message_->PushMessage(NetsysEventMessage::Type::ADDRESS,
                          addrStr + ROUTE_ADDRESS_SPLIT + std::to_string(addrMsg->ifa_prefixlen));
    message_->PushMessage(NetsysEventMessage::Type::FLAGS, flags);
    message_->PushMessage(NetsysEventMessage::Type::SCOPE, std::to_string(addrMsg->ifa_scope));
    message_->PushMessage(NetsysEventMessage::Type::IFINDEX, std::to_string(addrMsg->ifa_index));
    if (cacheInfo != nullptr) {
        message_->PushMessage(NetsysEventMessage::Type::PREFERRED, std::to_string(cacheInfo->ifa_prefered));
        message_->PushMessage(NetsysEventMessage::Type::VALID, std::to_string(cacheInfo->ifa_valid));
        message_->PushMessage(NetsysEventMessage::Type::CSTAMP, std::to_string(cacheInfo->cstamp));
        message_->PushMessage(NetsysEventMessage::Type::TSTAMP, std::to_string(cacheInfo->tstamp));
    }

    return true;
}

bool WrapperDecoder::InterpreteRtMsg(const nlmsghdr *hdrMsg)
{
    uint8_t type = hdrMsg->nlmsg_type;
    rtmsg *rtMsg = CheckRtParam(hdrMsg, type);
    if (rtMsg == nullptr) {
        return false;
    }
    char device[IFNAMSIZ] = {0};
    char dst[INET6_ADDRSTRLEN] = {0};
    char gateWay[INET6_ADDRSTRLEN] = {0};
    int32_t rtmFamily = rtMsg->rtm_family;
    int32_t rtmDstLen = rtMsg->rtm_dst_len;
    std::string msgName = CastNameToStr(type);
    size_t size = RTM_PAYLOAD(hdrMsg);
    rtattr *rtAttr = nullptr;
    for (rtAttr = RTM_RTA(rtMsg); RTA_OK(rtAttr, (int)size); rtAttr = RTA_NEXT(rtAttr, size)) {
        switch (rtAttr->rta_type) {
            case RTA_GATEWAY:
                if (IsDataEmpty(*gateWay, RTA_GATEWAY_STR, msgName) &&
                    !inet_ntop(rtmFamily, RTA_DATA(rtAttr), gateWay, sizeof(gateWay))) {
                    return false;
                }
                break;
            case RTA_DST:
                if (IsDataEmpty(*dst, RTA_DST_STR, msgName) &&
                    !inet_ntop(rtmFamily, RTA_DATA(rtAttr), dst, sizeof(dst))) {
                    return false;
                }
                break;
            case RTA_OIF:
                if (IsDataEmpty(*device, RTA_OIF_STR, msgName) &&
                    if_indextoname(*(reinterpret_cast<int32_t *>(RTA_DATA(rtAttr))), device) == nullptr) {
                    return false;
                }
                break;
            default:
                break;
        }
    }
    if (!SaveRtMsg(dst, gateWay, device, rtmDstLen, rtmFamily)) {
        return false;
    }
    auto action =
        (type == RTM_NEWROUTE) ? NetsysEventMessage::Action::ROUTEUPDATED : NetsysEventMessage::Action::ROUTEREMOVED;
    message_->SetAction(action);
    return true;
}

rtmsg *WrapperDecoder::CheckRtParam(const nlmsghdr *hdrMsg, uint8_t type)
{
    if (type != RTM_NEWROUTE && type != RTM_DELROUTE) {
        NETNATIVE_LOGE("read message error type %{public}d\n", type);
        return nullptr;
    }

    rtmsg *rtm = reinterpret_cast<rtmsg *>(NLMSG_DATA(hdrMsg));
    if (!CheckRtNetlinkLength(hdrMsg, sizeof(*rtm))) {
        return nullptr;
    }

    if ((rtm->rtm_protocol != RTPROT_KERNEL && rtm->rtm_protocol != RTPROT_RA) ||
        (rtm->rtm_scope != RT_SCOPE_UNIVERSE) || (rtm->rtm_type != RTN_UNICAST) || (rtm->rtm_src_len != 0) ||
        (rtm->rtm_flags & RTM_F_CLONED)) {
        return nullptr;
    }
    return rtm;
}

bool WrapperDecoder::SaveRtMsg(std::string dst, const std::string gateWay, const std::string device, int32_t length,
                               int32_t family)
{
    if (length == 0) {
        if (family == AF_INET) {
            dst.append(ROUTE_DEFAULT_ADDR);
        }
        if (family == AF_INET6) {
            dst.append(ROUTE_DEFAULT_SPLIT);
        }
    }
    if (dst.empty() || (gateWay.empty() && device.empty())) {
        NETNATIVE_LOGE("read message error dstSize: %{public}zu, gateWaySize: %{public}zu, deviceSize: %{public}zu",
                       dst.size(), gateWay.size(), device.size());
        return false;
    }

    message_->SetSubSys(NetsysEventMessage::SubSys::NET);
    message_->PushMessage(NetsysEventMessage::Type::ROUTE, dst + ROUTE_ADDRESS_SPLIT + std::to_string(length));
    message_->PushMessage(NetsysEventMessage::Type::GATEWAY, gateWay);
    message_->PushMessage(NetsysEventMessage::Type::INTERFACE, device);
    return true;
}

void WrapperDecoder::SaveOtherMsg(const std::string &info)
{
    auto msgList = Split(info, SYMBOL_EQUAL);
    if (msgList.size() == SPLIT_SIZE) {
        const auto key = msgList[0];
        auto value = msgList[1];
        if (ASCII_PARAM_LIST.find(key) != ASCII_PARAM_LIST.end()) {
            message_->PushMessage(ASCII_PARAM_LIST.at(key), value);
        }
    }
}
} // namespace nmd
} // namespace OHOS
