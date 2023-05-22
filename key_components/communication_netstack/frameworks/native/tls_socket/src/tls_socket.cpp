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

#include "tls_socket.h"

#include <chrono>
#include <numeric>
#include <regex>
#include <securec.h>
#include <thread>
#include <memory>

#include <netinet/tcp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "netmanager_base_permission.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "tls.h"

namespace OHOS {
namespace NetStack {
namespace {
constexpr int WAIT_MS = 10;
constexpr int TIMEOUT_MS = 10000;
constexpr int REMOTE_CERT_LEN = 8192;
constexpr int COMMON_NAME_BUF_SIZE = 256;
constexpr int BUF_SIZE = 2048;
constexpr int SSL_RET_CODE = 0;
constexpr int SSL_ERROR_RETURN = -1;
constexpr int OFFSET = 2;
constexpr const char *SPLIT_ALT_NAMES = ",";
constexpr const char *SPLIT_HOST_NAME = ".";
constexpr const char *PROTOCOL_UNKNOW = "UNKNOW_PROTOCOL";
constexpr const char *UNKNOW_REASON = "Unknown reason";
constexpr const char *IP = "IP: ";
constexpr const char *HOST_NAME = "hostname: ";
constexpr const char *DNS = "DNS:";
constexpr const char *IP_ADDRESS = "IP Address:";
constexpr const char *SIGN_NID_RSA = "RSA+";
constexpr const char *SIGN_NID_RSA_PSS = "RSA-PSS+";
constexpr const char *SIGN_NID_DSA = "DSA+";
constexpr const char *SIGN_NID_ECDSA = "ECDSA+";
constexpr const char *SIGN_NID_ED = "Ed25519+";
constexpr const char *SIGN_NID_ED_FOUR_FOUR_EIGHT = "Ed448+";
constexpr const char *SIGN_NID_UNDEF_ADD = "UNDEF+";
constexpr const char *SIGN_NID_UNDEF = "UNDEF";
constexpr const char *OPERATOR_PLUS_SIGN = "+";
const std::regex JSON_STRING_PATTERN{R"(/^"(?:[^"\\\u0000-\u001f]|\\(?:["\\/bfnrt]|u[0-9a-fA-F]{4}))*"/)"};
const std::regex PATTERN{
    "((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|"
    "2[0-4][0-9]|[01]?[0-9][0-9]?)"};

int ConvertErrno()
{
    return TlsSocketError::TLS_ERR_SYS_BASE + errno;
}

int ConvertSSLError(ssl_st *ssl)
{
    if (!ssl) {
        return TLS_ERR_SSL_NULL;
    }
    return TlsSocketError::TLS_ERR_SSL_BASE + SSL_get_error(ssl, SSL_RET_CODE);
}

std::string MakeErrnoString()
{
    return strerror(errno);
}

std::string MakeSSLErrorString(int error)
{
    char err[MAX_ERR_LEN] = {0};
    ERR_error_string_n(error - TlsSocketError::TLS_ERR_SYS_BASE, err, sizeof(err));
    return err;
}

std::vector<std::string> SplitEscapedAltNames(std::string &altNames)
{
    std::vector<std::string> result;
    std::string currentToken;
    size_t offset = 0;
    while (offset != altNames.length()) {
        auto nextSep = altNames.find_first_of(", ");
        auto nextQuote = altNames.find_first_of('\"');
        if (nextQuote != std::string::npos && (nextSep != std::string::npos || nextQuote < nextSep)) {
            currentToken += altNames.substr(offset, nextQuote);
            std::regex jsonStringPattern(JSON_STRING_PATTERN);
            std::smatch match;
            std::string altNameSubStr = altNames.substr(nextQuote);
            bool ret = regex_match(altNameSubStr, match, jsonStringPattern);
            if (!ret) {
                return {""};
            }
            currentToken += result[0];
            offset = nextQuote + result[0].length();
        } else if (nextSep != std::string::npos) {
            currentToken += altNames.substr(offset, nextSep);
            result.push_back(currentToken);
            currentToken = "";
            offset = nextSep + OFFSET;
        } else {
            currentToken += altNames.substr(offset);
            offset = altNames.length();
        }
    }
    result.push_back(currentToken);
    return result;
}

bool IsIP(const std::string &ip)
{
    std::regex pattern(PATTERN);
    std::smatch res;
    return regex_match(ip, res, pattern);
}

std::vector<std::string> SplitHostName(std::string &hostName)
{
    transform(hostName.begin(), hostName.end(), hostName.begin(), ::tolower);
    return CommonUtils::Split(hostName, SPLIT_HOST_NAME);
}

bool SeekIntersection(std::vector<std::string> &vecA, std::vector<std::string> &vecB)
{
    std::vector<std::string> result;
    set_intersection(vecA.begin(), vecA.end(), vecB.begin(), vecB.end(), inserter(result, result.begin()));
    return !result.empty();
}
} // namespace

TLSSecureOptions::TLSSecureOptions(const TLSSecureOptions &tlsSecureOptions)
{
    *this = tlsSecureOptions;
}

TLSSecureOptions &TLSSecureOptions::operator=(const TLSSecureOptions &tlsSecureOptions)
{
    key_ = tlsSecureOptions.GetKey();
    caChain_ = tlsSecureOptions.GetCaChain();
    cert_ = tlsSecureOptions.GetCert();
    protocolChain_ = tlsSecureOptions.GetProtocolChain();
    crlChain_ = tlsSecureOptions.GetCrlChain();
    keyPass_ = tlsSecureOptions.GetKeyPass();
    key_ = tlsSecureOptions.GetKey();
    signatureAlgorithms_ = tlsSecureOptions.GetSignatureAlgorithms();
    cipherSuite_ = tlsSecureOptions.GetCipherSuite();
    useRemoteCipherPrefer_ = tlsSecureOptions.UseRemoteCipherPrefer();
    return *this;
}

void TLSSecureOptions::SetCaChain(const std::vector<std::string> &caChain)
{
    caChain_ = caChain;
}

void TLSSecureOptions::SetCert(const std::string &cert)
{
    cert_ = cert;
}

void TLSSecureOptions::SetKey(const SecureData &key)
{
    key_ = key;
}

void TLSSecureOptions::SetKeyPass(const SecureData &keyPass)
{
    keyPass_ = keyPass;
}

void TLSSecureOptions::SetProtocolChain(const std::vector<std::string> &protocolChain)
{
    protocolChain_ = protocolChain;
}

void TLSSecureOptions::SetUseRemoteCipherPrefer(bool useRemoteCipherPrefer)
{
    useRemoteCipherPrefer_ = useRemoteCipherPrefer;
}

void TLSSecureOptions::SetSignatureAlgorithms(const std::string &signatureAlgorithms)
{
    signatureAlgorithms_ = signatureAlgorithms;
}

void TLSSecureOptions::SetCipherSuite(const std::string &cipherSuite)
{
    cipherSuite_ = cipherSuite;
}

void TLSSecureOptions::SetCrlChain(const std::vector<std::string> &crlChain)
{
    crlChain_ = crlChain;
}

const std::vector<std::string> &TLSSecureOptions::GetCaChain() const
{
    return caChain_;
}

const std::string &TLSSecureOptions::GetCert() const
{
    return cert_;
}

const SecureData &TLSSecureOptions::GetKey() const
{
    return key_;
}

const SecureData &TLSSecureOptions::GetKeyPass() const
{
    return keyPass_;
}

const std::vector<std::string> &TLSSecureOptions::GetProtocolChain() const
{
    return protocolChain_;
}

bool TLSSecureOptions::UseRemoteCipherPrefer() const
{
    return useRemoteCipherPrefer_;
}

const std::string &TLSSecureOptions::GetSignatureAlgorithms() const
{
    return signatureAlgorithms_;
}

const std::string &TLSSecureOptions::GetCipherSuite() const
{
    return cipherSuite_;
}

const std::vector<std::string> &TLSSecureOptions::GetCrlChain() const
{
    return crlChain_;
}

void TLSConnectOptions::SetNetAddress(const NetAddress &address)
{
    address_.SetAddress(address.GetAddress());
    address_.SetPort(address.GetPort());
    address_.SetFamilyBySaFamily(address.GetSaFamily());
}

void TLSConnectOptions::SetTlsSecureOptions(TLSSecureOptions &tlsSecureOptions)
{
    tlsSecureOptions_ = tlsSecureOptions;
}

void TLSConnectOptions::SetCheckServerIdentity(const CheckServerIdentity &checkServerIdentity)
{
    checkServerIdentity_ = checkServerIdentity;
}

void TLSConnectOptions::SetAlpnProtocols(const std::vector<std::string> &alpnProtocols)
{
    alpnProtocols_ = alpnProtocols;
}

NetAddress TLSConnectOptions::GetNetAddress() const
{
    return address_;
}

TLSSecureOptions TLSConnectOptions::GetTlsSecureOptions() const
{
    return tlsSecureOptions_;
}

CheckServerIdentity TLSConnectOptions::GetCheckServerIdentity() const
{
    return checkServerIdentity_;
}

const std::vector<std::string> &TLSConnectOptions::GetAlpnProtocols() const
{
    return alpnProtocols_;
}

std::string TLSSocket::MakeAddressString(sockaddr *addr)
{
    if (!addr) {
        return {};
    }
    if (addr->sa_family == AF_INET) {
        auto *addr4 = reinterpret_cast<sockaddr_in *>(addr);
        const char *str = inet_ntoa(addr4->sin_addr);
        if (str == nullptr || strlen(str) == 0) {
            return {};
        }
        return str;
    } else if (addr->sa_family == AF_INET6) {
        auto *addr6 = reinterpret_cast<sockaddr_in6 *>(addr);
        char str[INET6_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET6, &addr6->sin6_addr, str, INET6_ADDRSTRLEN) == nullptr || strlen(str) == 0) {
            return {};
        }
        return str;
    }
    return {};
}

void TLSSocket::GetAddr(const NetAddress &address, sockaddr_in *addr4, sockaddr_in6 *addr6, sockaddr **addr,
                        socklen_t *len)
{
    if (!addr6 || !addr4 || !len) {
        return;
    }
    sa_family_t family = address.GetSaFamily();
    if (family == AF_INET) {
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(address.GetPort());
        addr4->sin_addr.s_addr = inet_addr(address.GetAddress().c_str());
        *addr = reinterpret_cast<sockaddr *>(addr4);
        *len = sizeof(sockaddr_in);
    } else if (family == AF_INET6) {
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(address.GetPort());
        inet_pton(AF_INET6, address.GetAddress().c_str(), &addr6->sin6_addr);
        *addr = reinterpret_cast<sockaddr *>(addr6);
        *len = sizeof(sockaddr_in6);
    }
}

void TLSSocket::MakeIpSocket(sa_family_t family)
{
    if (family != AF_INET && family != AF_INET6) {
        return;
    }
    int sock = socket(family, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("Create socket failed (%{public}d:%{public}s)", errno, MakeErrnoString().c_str());
        CallOnErrorCallback(resErr, MakeErrnoString());
        return;
    }
    sockFd_ = sock;
}

void TLSSocket::StartReadMessage()
{
    std::thread thread([this]() {
        isRunning_ = true;
        isRunOver_ = false;
        char buffer[MAX_BUFFER_SIZE];
        bzero(buffer, MAX_BUFFER_SIZE);
        while (isRunning_) {
            int len = tlsSocketInternal_.Recv(buffer, MAX_BUFFER_SIZE);
            if (!isRunning_) {
                break;
            }
            if (len < 0) {
                int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
                NETSTACK_LOGE("SSL_read function read error, errno is %{public}d, errno info is %{public}s", resErr,
                              MakeSSLErrorString(resErr).c_str());
                CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
                break;
            }

            if (len == 0) {
                continue;
            }

            SocketRemoteInfo remoteInfo;
            remoteInfo.SetSize(len);
            tlsSocketInternal_.MakeRemoteInfo(remoteInfo);
            CallOnMessageCallback(buffer, remoteInfo);
        }
        isRunOver_ = true;
    });
    thread.detach();
}

void TLSSocket::CallOnMessageCallback(const std::string &data, const OHOS::NetStack::SocketRemoteInfo &remoteInfo)
{
    OnMessageCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (onMessageCallback_) {
            func = onMessageCallback_;
        }
    }

    if (func) {
        func(data, remoteInfo);
    }
}

void TLSSocket::CallOnConnectCallback()
{
    OnConnectCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (onConnectCallback_) {
            func = onConnectCallback_;
        }
    }

    if (func) {
        func();
    }
}

void TLSSocket::CallOnCloseCallback()
{
    OnCloseCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (onCloseCallback_) {
            func = onCloseCallback_;
        }
    }

    if (func) {
        func();
    }
}

void TLSSocket::CallOnErrorCallback(int32_t err, const std::string &errString)
{
    OnErrorCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (onErrorCallback_) {
            func = onErrorCallback_;
        }
    }

    if (func) {
        func(err, errString);
    }
}

void TLSSocket::CallBindCallback(int32_t err, BindCallback callback)
{
    BindCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err);
    }
}

void TLSSocket::CallConnectCallback(int32_t err, ConnectCallback callback)
{
    ConnectCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err);
    }
}

void TLSSocket::CallSendCallback(int32_t err, SendCallback callback)
{
    SendCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err);
    }
}

void TLSSocket::CallCloseCallback(int32_t err, CloseCallback callback)
{
    CloseCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err);
    }
}

void TLSSocket::CallGetRemoteAddressCallback(int32_t err, const NetAddress &address, GetRemoteAddressCallback callback)
{
    GetRemoteAddressCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, address);
    }
}

void TLSSocket::CallGetStateCallback(int32_t err, const SocketStateBase &state, GetStateCallback callback)
{
    GetStateCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, state);
    }
}

void TLSSocket::CallSetExtraOptionsCallback(int32_t err, SetExtraOptionsCallback callback)
{
    SetExtraOptionsCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err);
    }
}

void TLSSocket::CallGetCertificateCallback(int32_t err, const X509CertRawData &cert, GetCertificateCallback callback)
{
    GetCertificateCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, cert);
    }
}

void TLSSocket::CallGetRemoteCertificateCallback(int32_t err, const X509CertRawData &cert,
                                                 GetRemoteCertificateCallback callback)
{
    GetRemoteCertificateCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, cert);
    }
}

void TLSSocket::CallGetProtocolCallback(int32_t err, const std::string &protocol, GetProtocolCallback callback)
{
    GetProtocolCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, protocol);
    }
}

void TLSSocket::CallGetCipherSuiteCallback(int32_t err, const std::vector<std::string> &suite,
                                           GetCipherSuiteCallback callback)
{
    GetCipherSuiteCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, suite);
    }
}

void TLSSocket::CallGetSignatureAlgorithmsCallback(int32_t err, const std::vector<std::string> &algorithms,
                                                   GetSignatureAlgorithmsCallback callback)
{
    GetSignatureAlgorithmsCallback func = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callback) {
            func = callback;
        }
    }

    if (func) {
        func(err, algorithms);
    }
}

void TLSSocket::Bind(const OHOS::NetStack::NetAddress &address, const OHOS::NetStack::BindCallback &callback)
{
    if (!NetManagerStandard::NetManagerPermission::CheckPermission(NetManagerStandard::Permission::INTERNET)) {
        NETSTACK_LOGE("Bind permission check fail.");
        CallBindCallback(TLS_ERR_PERMISSION_DENIED, callback);
        return;
    }

    if (sockFd_ >= 0) {
        CallBindCallback(TLSSOCKET_SUCCESS, callback);
        return;
    }

    MakeIpSocket(address.GetSaFamily());
    if (sockFd_ < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("make tcp socket failed errno is %{public}d %{public}s", errno, MakeErrnoString().c_str());
        CallOnErrorCallback(resErr, MakeErrnoString());
        CallBindCallback(resErr, callback);
        return;
    }

    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t len;
    GetAddr(address, &addr4, &addr6, &addr, &len);
    if (addr == nullptr) {
        NETSTACK_LOGE("TLSSocket::Bind Address Is Invalid");
        CallOnErrorCallback(-1, "Address Is Invalid");
        CallBindCallback(ConvertErrno(), callback);
        return;
    }
    CallBindCallback(TLSSOCKET_SUCCESS, callback);
}

void TLSSocket::Connect(OHOS::NetStack::TLSConnectOptions &tlsConnectOptions,
                        const OHOS::NetStack::ConnectCallback &callback)
{
    if (sockFd_ < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("connect error is %{public}s %{public}d", MakeErrnoString().c_str(), errno);
        CallOnErrorCallback(resErr, MakeErrnoString());
        callback(resErr);
        return;
    }

    auto res = tlsSocketInternal_.TlsConnectToHost(sockFd_, tlsConnectOptions);
    if (!res) {
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr);
        return;
    }
    StartReadMessage();
    CallOnConnectCallback();
    callback(TLSSOCKET_SUCCESS);
}

void TLSSocket::Send(const OHOS::NetStack::TCPSendOptions &tcpSendOptions, const OHOS::NetStack::SendCallback &callback)
{
    (void)tcpSendOptions;

    auto res = tlsSocketInternal_.Send(tcpSendOptions.GetData());
    if (!res) {
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        CallSendCallback(resErr, callback);
        return;
    }
    CallSendCallback(TLSSOCKET_SUCCESS, callback);
}

bool WaitConditionWithTimeout(const bool *flag, const int32_t timeoutMs)
{
    int maxWaitCnt = timeoutMs / WAIT_MS;
    int cnt = 0;
    while (!(*flag)) {
        if (cnt >= maxWaitCnt) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        cnt++;
    }
    return true;
}

void TLSSocket::Close(const OHOS::NetStack::CloseCallback &callback)
{
    if (!WaitConditionWithTimeout(&isRunning_, TIMEOUT_MS)) {
        callback(ConvertErrno());
        NETSTACK_LOGE("The error cause is that the runtime wait time is insufficient");
        return;
    }
    isRunning_ = false;
    if (!WaitConditionWithTimeout(&isRunOver_, TIMEOUT_MS)) {
        callback(ConvertErrno());
        NETSTACK_LOGE("The error is due to insufficient delay time");
        return;
    }
    auto res = tlsSocketInternal_.Close();
    if (!res) {
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        NETSTACK_LOGE("close error is %{public}s %{public}d", MakeSSLErrorString(resErr).c_str(), resErr);
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr);
        return;
    }
    CallOnCloseCallback();
    callback(TLSSOCKET_SUCCESS);
}

void TLSSocket::GetRemoteAddress(const OHOS::NetStack::GetRemoteAddressCallback &callback)
{
    sa_family_t family;
    socklen_t len = sizeof(family);
    int ret = getsockname(sockFd_, reinterpret_cast<sockaddr *>(&family), &len);
    if (ret < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("getsockname failed errno %{public}d", resErr);
        CallOnErrorCallback(resErr, MakeErrnoString());
        CallGetRemoteAddressCallback(resErr, {}, callback);
        return;
    }

    if (family == AF_INET) {
        GetIp4RemoteAddress(callback);
    } else if (family == AF_INET6) {
        GetIp6RemoteAddress(callback);
    }
}

void TLSSocket::GetIp4RemoteAddress(const OHOS::NetStack::GetRemoteAddressCallback &callback)
{
    sockaddr_in addr4 = {0};
    socklen_t len4 = sizeof(sockaddr_in);

    int ret = getpeername(sockFd_, reinterpret_cast<sockaddr *>(&addr4), &len4);
    if (ret < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("GetIp4RemoteAddress failed errno %{public}d", resErr);
        CallOnErrorCallback(resErr, MakeErrnoString());
        CallGetRemoteAddressCallback(resErr, {}, callback);
        return;
    }

    std::string address = MakeAddressString(reinterpret_cast<sockaddr *>(&addr4));
    if (address.empty()) {
        NETSTACK_LOGE("GetIp4RemoteAddress failed errno %{public}d", errno);
        CallOnErrorCallback(-1, "Address is invalid");
        CallGetRemoteAddressCallback(ConvertErrno(), {}, callback);
        return;
    }
    NetAddress netAddress;
    netAddress.SetAddress(address);
    netAddress.SetFamilyBySaFamily(AF_INET);
    netAddress.SetPort(ntohs(addr4.sin_port));
    CallGetRemoteAddressCallback(TLSSOCKET_SUCCESS, netAddress, callback);
}

void TLSSocket::GetIp6RemoteAddress(const OHOS::NetStack::GetRemoteAddressCallback &callback)
{
    sockaddr_in6 addr6 = {0};
    socklen_t len6 = sizeof(sockaddr_in6);

    int ret = getpeername(sockFd_, reinterpret_cast<sockaddr *>(&addr6), &len6);
    if (ret < 0) {
        int resErr = ConvertErrno();
        NETSTACK_LOGE("GetIp6RemoteAddress failed errno %{public}d", resErr);
        CallOnErrorCallback(resErr, MakeErrnoString());
        CallGetRemoteAddressCallback(resErr, {}, callback);
        return;
    }

    std::string address = MakeAddressString(reinterpret_cast<sockaddr *>(&addr6));
    if (address.empty()) {
        NETSTACK_LOGE("GetIp6RemoteAddress failed errno %{public}d", errno);
        CallOnErrorCallback(-1, "Address is invalid");
        CallGetRemoteAddressCallback(ConvertErrno(), {}, callback);
        return;
    }
    NetAddress netAddress;
    netAddress.SetAddress(address);
    netAddress.SetFamilyBySaFamily(AF_INET6);
    netAddress.SetPort(ntohs(addr6.sin6_port));
    CallGetRemoteAddressCallback(TLSSOCKET_SUCCESS, netAddress, callback);
}

void TLSSocket::GetState(const OHOS::NetStack::GetStateCallback &callback)
{
    int opt;
    socklen_t optLen = sizeof(int);
    int r = getsockopt(sockFd_, SOL_SOCKET, SO_TYPE, &opt, &optLen);
    if (r < 0) {
        SocketStateBase state;
        state.SetIsClose(true);
        CallGetStateCallback(ConvertErrno(), state, callback);
        return;
    }
    sa_family_t family;
    socklen_t len = sizeof(family);
    SocketStateBase state;
    int ret = getsockname(sockFd_, reinterpret_cast<sockaddr *>(&family), &len);
    state.SetIsBound(ret == 0);
    ret = getpeername(sockFd_, reinterpret_cast<sockaddr *>(&family), &len);
    state.SetIsConnected(ret == 0);
    CallGetStateCallback(TLSSOCKET_SUCCESS, state, callback);
}

bool TLSSocket::SetBaseOptions(const ExtraOptionsBase &option) const
{
    if (option.GetReceiveBufferSize() != 0) {
        int size = (int)option.GetReceiveBufferSize();
        if (setsockopt(sockFd_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&size), sizeof(size)) < 0) {
            return false;
        }
    }

    if (option.GetSendBufferSize() != 0) {
        int size = (int)option.GetSendBufferSize();
        if (setsockopt(sockFd_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&size), sizeof(size)) < 0) {
            return false;
        }
    }

    if (option.IsReuseAddress()) {
        int reuse = 1;
        if (setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<void *>(&reuse), sizeof(reuse)) < 0) {
            return false;
        }
    }

    if (option.GetSocketTimeout() != 0) {
        timeval timeout = {(int)option.GetSocketTimeout(), 0};
        if (setsockopt(sockFd_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<void *>(&timeout), sizeof(timeout)) < 0) {
            return false;
        }
        if (setsockopt(sockFd_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<void *>(&timeout), sizeof(timeout)) < 0) {
            return false;
        }
    }

    return true;
}

bool TLSSocket::SetExtraOptions(const TCPExtraOptions &option) const
{
    if (option.IsKeepAlive()) {
        int keepalive = 1;
        if (setsockopt(sockFd_, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {
            return false;
        }
    }

    if (option.IsOOBInline()) {
        int oobInline = 1;
        if (setsockopt(sockFd_, SOL_SOCKET, SO_OOBINLINE, &oobInline, sizeof(oobInline)) < 0) {
            return false;
        }
    }

    if (option.IsTCPNoDelay()) {
        int tcpNoDelay = 1;
        if (setsockopt(sockFd_, IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(tcpNoDelay)) < 0) {
            return false;
        }
    }

    linger soLinger = {0};
    soLinger.l_onoff = option.socketLinger.IsOn();
    soLinger.l_linger = (int)option.socketLinger.GetLinger();
    if (setsockopt(sockFd_, SOL_SOCKET, SO_LINGER, &soLinger, sizeof(soLinger)) < 0) {
        return false;
    }

    return true;
}

void TLSSocket::SetExtraOptions(const OHOS::NetStack::TCPExtraOptions &tcpExtraOptions,
                                const OHOS::NetStack::SetExtraOptionsCallback &callback)
{
    if (!SetBaseOptions(tcpExtraOptions)) {
        NETSTACK_LOGE("SetExtraOptions errno %{public}d", errno);
        CallOnErrorCallback(errno, MakeErrnoString());
        CallSetExtraOptionsCallback(ConvertErrno(), callback);
        return;
    }

    if (!SetExtraOptions(tcpExtraOptions)) {
        NETSTACK_LOGE("SetExtraOptions errno %{public}d", errno);
        CallOnErrorCallback(errno, MakeErrnoString());
        CallSetExtraOptionsCallback(ConvertErrno(), callback);
        return;
    }

    CallSetExtraOptionsCallback(TLSSOCKET_SUCCESS, callback);
}

void TLSSocket::GetCertificate(const GetCertificateCallback &callback)
{
    const auto &cert = tlsSocketInternal_.GetCertificate();
    NETSTACK_LOGI("cert der is %{public}d", cert.encodingFormat);

    if (!cert.data.Length()) {
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        NETSTACK_LOGE("GetCertificate errno %{public}d, %{public}s", resErr, MakeSSLErrorString(resErr).c_str());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr, {});
        return;
    }
    callback(TLSSOCKET_SUCCESS, cert);
}

void TLSSocket::GetRemoteCertificate(const GetRemoteCertificateCallback &callback)
{
    const auto &remoteCert = tlsSocketInternal_.GetRemoteCertRawData();
    if (!remoteCert.data.Length()) {
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        NETSTACK_LOGE("GetRemoteCertificate errno %{public}d, %{public}s", resErr, MakeSSLErrorString(resErr).c_str());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr, {});
        return;
    }
    callback(TLSSOCKET_SUCCESS, remoteCert);
}

void TLSSocket::GetProtocol(const GetProtocolCallback &callback)
{
    const auto &protocol = tlsSocketInternal_.GetProtocol();
    if (protocol.empty()) {
        NETSTACK_LOGE("GetProtocol errno %{public}d", errno);
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr, "");
        return;
    }
    callback(TLSSOCKET_SUCCESS, protocol);
}

void TLSSocket::GetCipherSuite(const GetCipherSuiteCallback &callback)
{
    const auto &cipherSuite = tlsSocketInternal_.GetCipherSuite();
    if (cipherSuite.empty()) {
        NETSTACK_LOGE("GetCipherSuite errno %{public}d", errno);
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr, cipherSuite);
        return;
    }
    callback(TLSSOCKET_SUCCESS, cipherSuite);
}

void TLSSocket::GetSignatureAlgorithms(const GetSignatureAlgorithmsCallback &callback)
{
    const auto &signatureAlgorithms = tlsSocketInternal_.GetSignatureAlgorithms();
    if (signatureAlgorithms.empty()) {
        NETSTACK_LOGE("GetSignatureAlgorithms errno %{public}d", errno);
        int resErr = ConvertSSLError(tlsSocketInternal_.GetSSL());
        CallOnErrorCallback(resErr, MakeSSLErrorString(resErr));
        callback(resErr, {});
        return;
    }
    callback(TLSSOCKET_SUCCESS, signatureAlgorithms);
}

void TLSSocket::OnMessage(const OHOS::NetStack::OnMessageCallback &onMessageCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    onMessageCallback_ = onMessageCallback;
}

void TLSSocket::OffMessage()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (onMessageCallback_) {
        onMessageCallback_ = nullptr;
    }
}

void TLSSocket::OnConnect(const OnConnectCallback &onConnectCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    onConnectCallback_ = onConnectCallback;
}

void TLSSocket::OffConnect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (onConnectCallback_) {
        onConnectCallback_ = nullptr;
    }
}

void TLSSocket::OnClose(const OnCloseCallback &onCloseCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    onCloseCallback_ = onCloseCallback;
}

void TLSSocket::OffClose()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (onCloseCallback_) {
        onCloseCallback_ = nullptr;
    }
}

void TLSSocket::OnError(const OnErrorCallback &onErrorCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    onErrorCallback_ = onErrorCallback;
}

void TLSSocket::OffError()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (onErrorCallback_) {
        onErrorCallback_ = nullptr;
    }
}

bool ExecSocketConnect(const std::string &hostName, int port, sa_family_t family, int socketDescriptor)
{
    struct sockaddr_in dest = {0};
    dest.sin_family = family;
    dest.sin_port = htons(port);
    if (!inet_aton(hostName.c_str(), reinterpret_cast<in_addr *>(&dest.sin_addr.s_addr))) {
        NETSTACK_LOGE("inet_aton is error, hostName is %s", hostName.c_str());
        return false;
    }
    int connectResult = connect(socketDescriptor, reinterpret_cast<struct sockaddr *>(&dest), sizeof(dest));
    if (connectResult == -1) {
        NETSTACK_LOGE("socket connect error!The error code is %{public}d, The error message is %{public}s", errno,
                      strerror(errno));
        return false;
    }
    return true;
}

bool TLSSocket::TLSSocketInternal::TlsConnectToHost(int sock, const TLSConnectOptions &options)
{
    SetTlsConfiguration(options);
    std::string cipherSuite = options.GetTlsSecureOptions().GetCipherSuite();
    if (!cipherSuite.empty()) {
        configuration_.SetCipherSuite(cipherSuite);
    }
    std::string signatureAlgorithms = options.GetTlsSecureOptions().GetSignatureAlgorithms();
    if (!signatureAlgorithms.empty()) {
        configuration_.SetSignatureAlgorithms(signatureAlgorithms);
    }
    const auto protocolVec = options.GetTlsSecureOptions().GetProtocolChain();
    if (!protocolVec.empty()) {
        configuration_.SetProtocol(protocolVec);
    }

    hostName_ = options.GetNetAddress().GetAddress();
    port_ = options.GetNetAddress().GetPort();
    family_ = options.GetNetAddress().GetSaFamily();
    socketDescriptor_ = sock;
    if (!ExecSocketConnect(options.GetNetAddress().GetAddress(), options.GetNetAddress().GetPort(),
                           options.GetNetAddress().GetSaFamily(), socketDescriptor_)) {
        return false;
    }
    return StartTlsConnected(options);
}

void TLSSocket::TLSSocketInternal::SetTlsConfiguration(const TLSConnectOptions &config)
{
    configuration_.SetPrivateKey(config.GetTlsSecureOptions().GetKey(), config.GetTlsSecureOptions().GetKeyPass());
    configuration_.SetLocalCertificate(config.GetTlsSecureOptions().GetCert());
    configuration_.SetCaCertificate(config.GetTlsSecureOptions().GetCaChain());
}

bool TLSSocket::TLSSocketInternal::Send(const std::string &data)
{
    NETSTACK_LOGD("data to send :%{public}s", data.c_str());
    if (data.empty()) {
        NETSTACK_LOGE("data is empty");
        return false;
    }
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return false;
    }
    int len = SSL_write(ssl_, data.c_str(), data.length());
    if (len < 0) {
        int resErr = ConvertSSLError(GetSSL());
        NETSTACK_LOGE("data '%{public}s' send failed!The error code is %{public}d, The error message is'%{public}s'",
                      data.c_str(), resErr, MakeSSLErrorString(resErr).c_str());
        return false;
    }
    NETSTACK_LOGD("data '%{public}s' Sent successfully,sent in total %{public}d bytes!", data.c_str(), len);
    return true;
}
int TLSSocket::TLSSocketInternal::Recv(char *buffer, int maxBufferSize)
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return SSL_ERROR_RETURN;
    }
    return SSL_read(ssl_, buffer, maxBufferSize);
}

bool TLSSocket::TLSSocketInternal::Close()
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return false;
    }
    int result = SSL_shutdown(ssl_);
    if (result < 0) {
        int resErr = ConvertSSLError(GetSSL());
        NETSTACK_LOGE("Error in shutdown, errno is %{public}d, error info is %{public}s", resErr,
                      MakeSSLErrorString(resErr).c_str());
        return false;
    }
    SSL_free(ssl_);
    ssl_ = nullptr;
    close(socketDescriptor_);
    socketDescriptor_ = -1;
    if (!tlsContextPointer_) {
        NETSTACK_LOGE("Tls context pointer is null");
        return false;
    }
    tlsContextPointer_->CloseCtx();
    return true;
}

bool TLSSocket::TLSSocketInternal::SetAlpnProtocols(const std::vector<std::string> &alpnProtocols)
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return false;
    }
    size_t pos = 0;
    size_t len = std::accumulate(alpnProtocols.begin(), alpnProtocols.end(), static_cast<size_t>(0),
                                 [](size_t init, const std::string &alpnProt) { return init + alpnProt.length(); });
    auto result = std::make_unique<unsigned char[]>(alpnProtocols.size() + len);
    for (const auto &str : alpnProtocols) {
        len = str.length();
        result[pos++] = len;
        if (!strcpy_s(reinterpret_cast<char *>(&result[pos]), len, str.c_str())) {
            NETSTACK_LOGE("strcpy_s failed");
            return false;
        }
        pos += len;
    }
    result[pos] = '\0';

    NETSTACK_LOGD("alpnProtocols after splicing %{public}s", result.get());
    if (SSL_set_alpn_protos(ssl_, result.get(), pos)) {
        int resErr = ConvertSSLError(GetSSL());
        NETSTACK_LOGE("Failed to set negotiable protocol list, errno is %{public}d, error info is %{public}s", resErr,
                      MakeSSLErrorString(resErr).c_str());
        return false;
    }
    return true;
}

void TLSSocket::TLSSocketInternal::MakeRemoteInfo(SocketRemoteInfo &remoteInfo)
{
    remoteInfo.SetAddress(hostName_);
    remoteInfo.SetPort(port_);
    remoteInfo.SetFamily(family_);
}

TLSConfiguration TLSSocket::TLSSocketInternal::GetTlsConfiguration() const
{
    return configuration_;
}

std::vector<std::string> TLSSocket::TLSSocketInternal::GetCipherSuite() const
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl in null");
        return {};
    }
    STACK_OF(SSL_CIPHER) *sk = SSL_get_ciphers(ssl_);
    if (!sk) {
        NETSTACK_LOGE("get ciphers failed");
        return {};
    }
    CipherSuite cipherSuite;
    std::vector<std::string> cipherSuiteVec;
    for (int i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        const SSL_CIPHER *c = sk_SSL_CIPHER_value(sk, i);
        cipherSuite.cipherName_ = SSL_CIPHER_get_name(c);
        cipherSuiteVec.push_back(cipherSuite.cipherName_);
    }
    return cipherSuiteVec;
}

std::string TLSSocket::TLSSocketInternal::GetRemoteCertificate() const
{
    return remoteCert_;
}

const X509CertRawData &TLSSocket::TLSSocketInternal::GetCertificate() const
{
    return configuration_.GetCertificate();
}

std::vector<std::string> TLSSocket::TLSSocketInternal::GetSignatureAlgorithms() const
{
    return signatureAlgorithms_;
}

std::string TLSSocket::TLSSocketInternal::GetProtocol() const
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl in null");
        return PROTOCOL_UNKNOW;
    }
    if (configuration_.GetProtocol() == TLS_V1_3) {
        return PROTOCOL_TLS_V13;
    }
    return PROTOCOL_TLS_V12;
}

bool TLSSocket::TLSSocketInternal::SetSharedSigals()
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return false;
    }
    int number = SSL_get_shared_sigalgs(ssl_, 0, nullptr, nullptr, nullptr, nullptr, nullptr);
    if (!number) {
        NETSTACK_LOGE("SSL_get_shared_sigalgs return value error");
        return false;
    }
    for (int i = 0; i < number; i++) {
        int hash_nid;
        int sign_nid;
        std::string sig_with_md;
        SSL_get_shared_sigalgs(ssl_, i, &sign_nid, &hash_nid, nullptr, nullptr, nullptr);
        switch (sign_nid) {
            case EVP_PKEY_RSA:
                sig_with_md = SIGN_NID_RSA;
                break;
            case EVP_PKEY_RSA_PSS:
                sig_with_md = SIGN_NID_RSA_PSS;
                break;
            case EVP_PKEY_DSA:
                sig_with_md = SIGN_NID_DSA;
                break;
            case EVP_PKEY_EC:
                sig_with_md = SIGN_NID_ECDSA;
                break;
            case NID_ED25519:
                sig_with_md = SIGN_NID_ED;
                break;
            case NID_ED448:
                sig_with_md = SIGN_NID_ED_FOUR_FOUR_EIGHT;
                break;
            default:
                const char *sn = OBJ_nid2sn(sign_nid);
                sig_with_md = (sn != nullptr) ? (std::string(sn) + OPERATOR_PLUS_SIGN) : SIGN_NID_UNDEF_ADD;
        }
        const char *sn_hash = OBJ_nid2sn(hash_nid);
        sig_with_md += (sn_hash != nullptr) ? std::string(sn_hash) : SIGN_NID_UNDEF;
        signatureAlgorithms_.push_back(sig_with_md);
    }
    return true;
}

bool TLSSocket::TLSSocketInternal::StartTlsConnected(const TLSConnectOptions &options)
{
    if (!CreatTlsContext()) {
        NETSTACK_LOGE("failed to create tls context");
        return false;
    }
    if (!StartShakingHands(options)) {
        NETSTACK_LOGE("failed to shaking hands");
        return false;
    }
    return true;
}

bool TLSSocket::TLSSocketInternal::CreatTlsContext()
{
    tlsContextPointer_ = TLSContext::CreateConfiguration(configuration_);
    if (!tlsContextPointer_) {
        NETSTACK_LOGE("failed to create tls context pointer");
        return false;
    }
    if (!(ssl_ = tlsContextPointer_->CreateSsl())) {
        NETSTACK_LOGE("failed to create ssl session");
        return false;
    }
    SSL_set_fd(ssl_, socketDescriptor_);
    SSL_set_connect_state(ssl_);
    return true;
}

static bool StartsWith(const std::string &s, const std::string &prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

void CheckIpAndDnsName(const std::string &hostName, std::vector<std::string> dnsNames, std::vector<std::string> ips,
                       const X509 *x509Certificates, std::tuple<bool, std::string> &result)
{
    bool valid = false;
    std::string reason = UNKNOW_REASON;
    int index = X509_get_ext_by_NID(x509Certificates, NID_commonName, -1);
    if (IsIP(hostName)) {
        auto it = find(ips.begin(), ips.end(), hostName);
        if (it == ips.end()) {
            reason = IP + hostName + " is not in the cert's list";
        }
        result = {valid, reason};
        return;
    }
    std::string tempHostName = "" + hostName;
    if (!dnsNames.empty() || index > 0) {
        std::vector<std::string> hostParts = SplitHostName(tempHostName);
        if (!dnsNames.empty()) {
            valid = SeekIntersection(hostParts, dnsNames);
            if (!valid) {
                reason = HOST_NAME + tempHostName + ". is not in the cert's altnames";
            }
        } else {
            char commonNameBuf[COMMON_NAME_BUF_SIZE] = {0};
            X509_NAME *pSubName = nullptr;
            int len = X509_NAME_get_text_by_NID(pSubName, NID_commonName, commonNameBuf, COMMON_NAME_BUF_SIZE);
            if (len > 0) {
                std::vector<std::string> commonNameVec;
                commonNameVec.emplace_back(commonNameBuf);
                valid = SeekIntersection(hostParts, commonNameVec);
                if (!valid) {
                    reason = HOST_NAME + tempHostName + ". is not cert's CN";
                }
            }
        }
        result = {valid, reason};
        return;
    }
    reason = "Cert does not contain a DNS name";
    result = {valid, reason};
}

std::string TLSSocket::TLSSocketInternal::CheckServerIdentityLegal(const std::string &hostName,
                                                                   const X509 *x509Certificates)
{
    std::string hostname = hostName;

    X509_NAME *subjectName = X509_get_subject_name(x509Certificates);
    if (!subjectName) {
        return "subject name is null";
    }
    char subNameBuf[BUF_SIZE] = {0};
    X509_NAME_oneline(subjectName, subNameBuf, BUF_SIZE);

    int index = X509_get_ext_by_NID(x509Certificates, NID_subject_alt_name, -1);
    if (index < 0) {
        return "X509 get ext nid error";
    }
    X509_EXTENSION *ext = X509_get_ext(x509Certificates, index);
    if (ext == nullptr) {
        return "X509 get ext error";
    }
    ASN1_OBJECT *obj = nullptr;
    obj = X509_EXTENSION_get_object(ext);
    char subAltNameBuf[BUF_SIZE] = {0};
    OBJ_obj2txt(subAltNameBuf, BUF_SIZE, obj, 0);
    NETSTACK_LOGD("extions obj : %{public}s\n", subAltNameBuf);

    ASN1_OCTET_STRING *extData = X509_EXTENSION_get_data(ext);
    std::string altNames = reinterpret_cast<char *>(extData->data);

    BIO *bio = BIO_new(BIO_s_file());
    if (!bio) {
        return "bio is null";
    }
    BIO_set_fp(bio, stdout, BIO_NOCLOSE);
    ASN1_STRING_print(bio, extData);
    std::vector<std::string> dnsNames = {};
    std::vector<std::string> ips = {};
    constexpr int DNS_NAME_IDX = 4;
    constexpr int IP_NAME_IDX = 11;
    hostname = "" + hostname;
    if (!altNames.empty()) {
        std::vector<std::string> splitAltNames;
        if (altNames.find('\"') != std::string::npos) {
            splitAltNames = SplitEscapedAltNames(altNames);
        } else {
            splitAltNames = CommonUtils::Split(altNames, SPLIT_ALT_NAMES);
        }
        for (auto const &iter : splitAltNames) {
            if (StartsWith(iter, DNS)) {
                dnsNames.push_back(iter.substr(DNS_NAME_IDX));
            } else if (StartsWith(iter, IP_ADDRESS)) {
                ips.push_back(iter.substr(IP_NAME_IDX));
            }
        }
    }
    std::tuple<bool, std::string> result;
    CheckIpAndDnsName(hostName, dnsNames, ips, x509Certificates, result);
    if (!std::get<0>(result)) {
        return "Hostname/IP does not match certificate's altnames: " + std::get<1>(result);
    }
    return HOST_NAME + hostname + ". is cert's CN";
}

bool TLSSocket::TLSSocketInternal::StartShakingHands(const TLSConnectOptions &options)
{
    if (!ssl_) {
        NETSTACK_LOGE("ssl is null");
        return false;
    }
    int result = SSL_connect(ssl_);
    if (result == -1) {
        int errorStatus = ConvertSSLError(ssl_);
        NETSTACK_LOGE("SSL connect is error, errno is %{public}d, error info is %{public}s", errorStatus,
                      MakeSSLErrorString(errorStatus).c_str());
        return false;
    }

    std::string list = SSL_get_cipher_list(ssl_, 0);
    NETSTACK_LOGI("SSL_get_cipher_list: %{public}s", list.c_str());
    configuration_.SetCipherSuite(list);
    if (!SetSharedSigals()) {
        NETSTACK_LOGE("Failed to set sharedSigalgs");
    }
    if (!GetRemoteCertificateFromPeer()) {
        NETSTACK_LOGE("Failed to get remote certificate");
    }
    if (!peerX509_) {
        NETSTACK_LOGE("peer x509Certificates is null");
        return false;
    }
    if (!SetRemoteCertRawData()) {
        NETSTACK_LOGE("Failed to set remote x509 certificata Serialization data");
    }
    CheckServerIdentity checkServerIdentity = options.GetCheckServerIdentity();
    if (!checkServerIdentity) {
        CheckServerIdentityLegal(hostName_, peerX509_);
    } else {
        checkServerIdentity(hostName_, {remoteCert_});
    }
    NETSTACK_LOGI("SSL Get Version: %{public}s, SSL Get Cipher: %{public}s", SSL_get_version(ssl_),
                  SSL_get_cipher(ssl_));
    return true;
}

bool TLSSocket::TLSSocketInternal::GetRemoteCertificateFromPeer()
{
    peerX509_ = SSL_get_peer_certificate(ssl_);
    if (peerX509_ == nullptr) {
        int resErr = ConvertSSLError(GetSSL());
        NETSTACK_LOGE("open fail errno, errno is %{public}d, error info is %{public}s", resErr,
                      MakeSSLErrorString(resErr).c_str());
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        NETSTACK_LOGE("TlsSocket::SetRemoteCertificate bio is null");
        return false;
    }
    X509_print(bio, peerX509_);
    char data[REMOTE_CERT_LEN] = {0};
    if (!BIO_read(bio, data, REMOTE_CERT_LEN)) {
        NETSTACK_LOGE("BIO_read function returns error");
        BIO_free(bio);
        return false;
    }
    BIO_free(bio);
    remoteCert_ = std::string(data);
    return true;
}

bool TLSSocket::TLSSocketInternal::SetRemoteCertRawData()
{
    if (peerX509_ == nullptr) {
        NETSTACK_LOGE("peerX509 is null");
        return false;
    }
    int32_t length = i2d_X509(peerX509_, nullptr);
    if (length <= 0) {
        NETSTACK_LOGE("Failed to convert peerX509 to der format");
        return false;
    }
    unsigned char *der = nullptr;
    (void)i2d_X509(peerX509_, &der);
    SecureData data(der, length);
    remoteRawData_.data = data;
    OPENSSL_free(der);
    remoteRawData_.encodingFormat = DER;
    return true;
}

const X509CertRawData &TLSSocket::TLSSocketInternal::GetRemoteCertRawData() const
{
    return remoteRawData_;
}

ssl_st *TLSSocket::TLSSocketInternal::GetSSL() const
{
    return ssl_;
}
} // namespace NetStack
} // namespace OHOS
