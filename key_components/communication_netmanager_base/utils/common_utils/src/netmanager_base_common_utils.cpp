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

#include "netmanager_base_common_utils.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <regex>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <type_traits>
#include <unistd.h>
#include <vector>

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "securec.h"

namespace OHOS::NetManagerStandard::CommonUtils {
constexpr int32_t INET_OPTION_SUC = 1;
constexpr int32_t DECIMAL_SYSTEM = 10;
constexpr uint32_t CONST_MASK = 0x80000000;
constexpr size_t MAX_DISPLAY_NUM = 2;
constexpr uint32_t IPV4_DOT_NUM = 3;
constexpr int32_t MIN_BYTE = 0;
constexpr int32_t MAX_BYTE = 255;
constexpr uint32_t BIT_NUM_BYTE = 8;
constexpr int32_t BITS_24 = 24;
constexpr int32_t BITS_16 = 16;
constexpr int32_t BITS_8 = 8;
constexpr uint32_t INTERFACE_NAME_MAX_SIZE = 16;
constexpr int32_t CHAR_ARRAY_SIZE_MAX = 1024;
constexpr int32_t PIPE_FD_NUM = 2;
constexpr int32_t PIPE_OUT = 0;
constexpr int32_t PIPE_IN = 1;
const std::string IPADDR_DELIMITER = ".";
constexpr const char *CMD_SEP = " ";
const std::regex IP_PATTERN{
    "((2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)\\.){3}(2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)"};

const std::regex IP_MASK_PATTERN{
    "((2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)\\.){3}(2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)/"
    "(3[0-2]|[1-2]\\d|\\d)"};

const std::regex IPV6_PATTERN{"([\\da-fA-F]{0,4}:){2,7}([\\da-fA-F]{0,4})"};

const std::regex IPV6_MASK_PATTERN{"([\\da-fA-F]{0,4}:){2,7}([\\da-fA-F]{0,4})/(1[0-2][0-8]|[1-9]\\d|[1-9])"};

constexpr int32_t NET_MASK_MAX_LENGTH = 32;
constexpr int32_t NET_MASK_GROUP_COUNT = 4;
std::mutex g_commonUtilsMutex;
std::mutex g_forkExecMutex;

std::string Strip(const std::string &str, char ch)
{
    auto size = static_cast<int64_t>(str.size());
    int64_t i = 0;
    while (i < size && str[i] == ch) {
        ++i;
    }
    int64_t j = size - 1;
    while (j > 0 && str[j] == ch) {
        --j;
    }
    if (i >= 0 && i < size && j >= 0 && j < size && j - i + 1 > 0) {
        return str.substr(i, j - i + 1);
    }
    return "";
}

std::string ToLower(const std::string &s)
{
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), tolower);
    return res;
}

bool IsValidIPV4(const std::string &ip)
{
    if (ip.empty()) {
        return false;
    }
    struct in_addr s;
    return inet_pton(AF_INET, ip.c_str(), reinterpret_cast<void *>(&s)) == INET_OPTION_SUC;
}

bool IsValidIPV6(const std::string &ip)
{
    if (ip.empty()) {
        return false;
    }
    struct in6_addr s;
    return inet_pton(AF_INET6, ip.c_str(), reinterpret_cast<void *>(&s)) == INET_OPTION_SUC;
}

int8_t GetAddrFamily(const std::string &ip)
{
    if (IsValidIPV4(ip)) {
        return AF_INET;
    }
    if (IsValidIPV6(ip)) {
        return AF_INET6;
    }
    return 0;
}

int GetMaskLength(const std::string &mask)
{
    int netMask = 0;
    unsigned int maskTmp = ntohl(static_cast<int>(inet_addr(mask.c_str())));
    while (maskTmp & CONST_MASK) {
        ++netMask;
        maskTmp = (maskTmp << 1);
    }
    return netMask;
}

std::string GetMaskByLength(uint32_t length)
{
    const auto mask = length == 0 ? 0 : -1 << (NET_MASK_MAX_LENGTH - length);
    auto maskGroup = new int[NET_MASK_GROUP_COUNT];
    for (int i = 0; i < NET_MASK_GROUP_COUNT; i++) {
        int pos = NET_MASK_GROUP_COUNT - 1 - i;
        maskGroup[pos] = (static_cast<uint32_t>(mask) >> (i * BIT_NUM_BYTE)) & 0x000000ff;
    }
    std::string sMask = "" + std::to_string(maskGroup[0]);
    for (int i = 1; i < NET_MASK_GROUP_COUNT; i++) {
        sMask = sMask + "." + std::to_string(maskGroup[i]);
    }
    delete[] maskGroup;
    return sMask;
}

std::string ConvertIpv4Address(uint32_t addressIpv4)
{
    if (addressIpv4 == 0) {
        return "";
    }

    std::ostringstream stream;
    stream << ((addressIpv4 >> BITS_24) & 0xFF) << IPADDR_DELIMITER << ((addressIpv4 >> BITS_16) & 0xFF)
           << IPADDR_DELIMITER << ((addressIpv4 >> BITS_8) & 0xFF) << IPADDR_DELIMITER << (addressIpv4 & 0xFF);
    return stream.str();
}

uint32_t ConvertIpv4Address(const std::string &address)
{
    std::string tmpAddress = address;
    uint32_t addrInt = 0;
    uint32_t i = 0;
    for (i = 0; i < IPV4_DOT_NUM; i++) {
        std::string::size_type npos = tmpAddress.find(IPADDR_DELIMITER);
        if (npos == std::string::npos) {
            break;
        }
        const auto &value = tmpAddress.substr(0, npos);
        int32_t itmp = std::atoi(value.c_str());
        if ((itmp < MIN_BYTE) || (itmp > MAX_BYTE)) {
            break;
        }
        uint32_t utmp = static_cast<uint32_t>(itmp);
        addrInt += utmp << ((IPV4_DOT_NUM - i) * BIT_NUM_BYTE);
        tmpAddress = tmpAddress.substr(npos + 1);
    }

    if (i != IPV4_DOT_NUM) {
        return 0;
    }
    int32_t itmp = std::atoi(tmpAddress.c_str());
    if ((itmp < MIN_BYTE) || (itmp > MAX_BYTE)) {
        return 0;
    }
    uint32_t utmp = static_cast<uint32_t>(itmp);
    addrInt += utmp;

    return addrInt;
}

int32_t Ipv4PrefixLen(const std::string &ip)
{
    constexpr int32_t BIT32 = 32;
    constexpr int32_t BIT24 = 24;
    constexpr int32_t BIT16 = 16;
    constexpr int32_t BIT8 = 8;
    if (ip.empty()) {
        return 0;
    }
    int32_t ret = 0;
    uint32_t ipNum = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    uint8_t c4 = 0;
    int32_t cnt = 0;
    ret = sscanf_s(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &c1, &c2, &c3, &c4);
    if (ret != sizeof(int32_t)) {
        return 0;
    }
    ipNum = (c1 << static_cast<uint32_t>(BIT24)) | (c2 << static_cast<uint32_t>(BIT16)) |
            (c3 << static_cast<uint32_t>(BIT8)) | c4;
    if (ipNum == 0xFFFFFFFF) {
        return BIT32;
    }
    if (ipNum == 0xFFFFFF00) {
        return BIT24;
    }
    if (ipNum == 0xFFFF0000) {
        return BIT16;
    }
    if (ipNum == 0xFF000000) {
        return BIT8;
    }
    for (int32_t i = 0; i < BIT32; i++) {
        if ((ipNum << i) & 0x80000000) {
            cnt++;
        } else {
            break;
        }
    }
    return cnt;
}

bool ParseInt(const std::string &str, int32_t *value)
{
    char *end;
    long long v = strtoll(str.c_str(), &end, 10);
    if (std::string(end) == str || *end != '\0' || v < INT_MIN || v > INT_MAX) {
        return false;
    }
    *value = v;
    return true;
}

int64_t ConvertToInt64(const std::string &str)
{
    return strtoll(str.c_str(), nullptr, DECIMAL_SYSTEM);
}

std::string MaskIpv4(std::string &maskedResult)
{
    int maxDisplayNum = MAX_DISPLAY_NUM;
    for (char &i : maskedResult) {
        if (i == '/') {
            break;
        }
        if (maxDisplayNum > 0) {
            if (i == '.') {
                maxDisplayNum--;
            }
        } else {
            if (i != '.') {
                i = '*';
            }
        }
    }
    return maskedResult;
}

std::string MaskIpv6(std::string &maskedResult)
{
    size_t colonCount = 0;
    for (char &i : maskedResult) {
        if (i == '/') {
            break;
        }
        if (i == ':') {
            colonCount++;
        }

        if (colonCount >= MAX_DISPLAY_NUM) { // An legal ipv6 address has at least 2 ':'.
            if (i != ':' && i != '/') {
                i = '*';
            }
        }
    }
    return maskedResult;
}

std::string ToAnonymousIp(const std::string &input)
{
    std::lock_guard<std::mutex> lock(g_commonUtilsMutex);
    std::string maskedResult{input};
    // Mask ipv4 address.
    if (std::regex_match(maskedResult, IP_PATTERN) || std::regex_match(maskedResult, IP_MASK_PATTERN)) {
        return MaskIpv4(maskedResult);
    }
    // Mask ipv6 address.
    if (std::regex_match(maskedResult, IPV6_PATTERN) || std::regex_match(maskedResult, IPV6_MASK_PATTERN)) {
        return MaskIpv6(maskedResult);
    }
    return input;
}

int32_t StrToInt(const std::string& value, int32_t defaultErr)
{
    errno = 0;
    char* pEnd = nullptr;
    int64_t result = std::strtol(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || (result < INT_MIN || result > LONG_MAX) || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}


uint32_t StrToUint(const std::string& value, uint32_t defaultErr)
{
    errno = 0;
    char* pEnd = nullptr;
    uint64_t result = std::strtoul(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

bool StrToBool(const std::string &value, bool defaultErr)
{
    errno = 0;
    char* pEnd = nullptr;
    uint64_t result = std::strtoul(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    }
    return static_cast<bool>(result);
}

int64_t StrToLong(const std::string& value, int64_t defaultErr)
{
    errno = 0;
    char* pEnd = nullptr;
    int64_t result = std::strtoll(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

uint64_t StrToUint64(const std::string& value, uint64_t defaultErr)
{
    errno = 0;
    char* pEnd = nullptr;
    uint64_t result = std::strtoull(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

bool CheckIfaceName(const std::string &name)
{
    uint32_t index = 0;
    if (name.empty()) {
        return false;
    }
    size_t len = name.size();
    if (len > INTERFACE_NAME_MAX_SIZE) {
        return false;
    }
    while (index < len) {
        if ((index == 0) && !isalnum(name[index])) {
            return false;
        }
        if (!isalnum(name[index]) && (name[index] != '-') && (name[index] != '_') && (name[index] != '.') &&
            (name[index] != ':')) {
            return false;
        }
        index++;
    }
    return true;
}

std::vector<const char *> FormatCmd(const std::vector<std::string> &cmd)
{
    std::vector<const char *> res;
    res.reserve(cmd.size() + 1);

    // string is converted to char * and the result is saved in res
    std::transform(cmd.begin(), cmd.end(), std::back_inserter(res), [](const std::string &str) { return str.c_str(); });
    res.emplace_back(nullptr);
    return res;
}

int32_t ForkExecChildProcess(const int32_t *pipeFd, int32_t count, const std::vector<const char *> &args)
{
    if (count != PIPE_FD_NUM) {
        NETMGR_LOG_E("fork exec parent process failed");
        _exit(-1);
    }
    if (close(pipeFd[PIPE_OUT]) != 0) {
        NETMGR_LOG_E("close failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        _exit(-1);
    }
    if (dup2(pipeFd[PIPE_IN], STDOUT_FILENO) == -1) {
        NETMGR_LOG_E("dup2 failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        _exit(-1);
    }
    if (execv(args[0], const_cast<char *const *>(&args[0])) == -1) {
        NETMGR_LOG_E("execv command failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
    }
    _exit(-1);
}

int32_t ForkExecParentProcess(const int32_t *pipeFd, int32_t count, pid_t childPid, std::string *out)
{
    if (count != PIPE_FD_NUM) {
        NETMGR_LOG_E("fork exec parent process failed");
        return NETMANAGER_ERROR;
    }
    if (out != nullptr) {
        char buf[CHAR_ARRAY_SIZE_MAX] = {0};
        out->clear();
        if (close(pipeFd[PIPE_IN]) != 0) {
            NETMGR_LOG_E("close failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        }
        while (read(pipeFd[PIPE_OUT], buf, CHAR_ARRAY_SIZE_MAX - 1) > 0) {
            out->append(buf);
        }
        return NETMANAGER_SUCCESS;
    }
    pid_t pidRet = waitpid(childPid, nullptr, 0);
    if (pidRet != childPid) {
        NETMGR_LOG_E("waitpid[%{public}d] failed, pidRet:%{public}d", childPid, pidRet);
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

int32_t ForkExec(const std::string &command, std::string *out)
{
    std::unique_lock<std::mutex> lock(g_forkExecMutex);
    const std::vector<std::string> cmd = Split(command, CMD_SEP);
    std::vector<const char *> args = FormatCmd(cmd);
    int32_t pipeFd[PIPE_FD_NUM] = {0};
    if (pipe(pipeFd) < 0) {
        NETMGR_LOG_E("creat pipe failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERROR;
    }
    pid_t pid = fork();
    if (pid < 0) {
        NETMGR_LOG_E("fork failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERROR;
    }
    if (pid == 0) {
        ForkExecChildProcess(pipeFd, PIPE_FD_NUM, args);
        return NETMANAGER_SUCCESS;
    } else {
        return ForkExecParentProcess(pipeFd, PIPE_FD_NUM, pid, out);
    }
}
} // namespace OHOS::NetManagerStandard::CommonUtils
