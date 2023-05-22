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

#include "socket_exec.h"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <memory>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "context_key.h"
#include "event_list.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "napi_utils.h"
#include "securec.h"

static constexpr const int DEFAULT_BUFFER_SIZE = 8192;

static constexpr const int DEFAULT_POLL_TIMEOUT = 500; // 0.5 Seconds

static constexpr const int ADDRESS_INVALID = -1;

static constexpr const int NO_MEMORY = -2;

static constexpr const int MSEC_TO_USEC = 1000;

static constexpr const int MAX_SEC = 999999999;

namespace OHOS::NetStack::SocketExec {
struct MessageData {
    MessageData() = delete;
    MessageData(void *d, size_t l, const SocketRemoteInfo &info) : data(d), len(l), remoteInfo(info) {}
    ~MessageData()
    {
        if (data) {
            free(data);
        }
    }

    void *data;
    size_t len;
    SocketRemoteInfo remoteInfo;
};

static void SetIsBound(sa_family_t family, GetStateContext *context, const sockaddr_in *addr4,
                       const sockaddr_in6 *addr6)
{
    if (family == AF_INET) {
        context->state_.SetIsBound(ntohs(addr4->sin_port) != 0);
    } else if (family == AF_INET6) {
        context->state_.SetIsBound(ntohs(addr6->sin6_port) != 0);
    }
}

static void SetIsConnected(sa_family_t family, GetStateContext *context, const sockaddr_in *addr4,
                           const sockaddr_in6 *addr6)
{
    if (family == AF_INET) {
        context->state_.SetIsConnected(ntohs(addr4->sin_port) != 0);
    } else if (family == AF_INET6) {
        context->state_.SetIsConnected(ntohs(addr6->sin6_port) != 0);
    }
}

template <napi_value (*MakeJsValue)(napi_env, void *)> static void CallbackTemplate(uv_work_t *work, int status)
{
    (void)status;

    auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
    napi_env env = workWrapper->env;
    auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
    std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);

    napi_value obj = MakeJsValue(env, workWrapper->data);

    std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env), obj};
    workWrapper->manager->Emit(workWrapper->type, arg);

    delete workWrapper;
    delete work;
}

static napi_value MakeError(napi_env env, void *errCode)
{
    auto code = reinterpret_cast<int32_t *>(errCode);
    auto deleter = [](const int32_t *p) { delete p; };
    std::unique_ptr<int32_t, decltype(deleter)> handler(code, deleter);

    napi_value err = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, err) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetInt32Property(env, err, KEY_ERROR_CODE, *code);
    return err;
}

static std::string MakeAddressString(sockaddr *addr)
{
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

static napi_value MakeJsMessageParam(napi_env env, napi_value msgBuffer, SocketRemoteInfo *remoteInfo)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return nullptr;
    }
    if (NapiUtils::ValueIsArrayBuffer(env, msgBuffer)) {
        NapiUtils::SetNamedProperty(env, obj, KEY_MESSAGE, msgBuffer);
    }
    napi_value jsRemoteInfo = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, jsRemoteInfo) != napi_object) {
        return nullptr;
    }
    NapiUtils::SetStringPropertyUtf8(env, jsRemoteInfo, KEY_ADDRESS, remoteInfo->GetAddress());
    NapiUtils::SetStringPropertyUtf8(env, jsRemoteInfo, KEY_FAMILY, remoteInfo->GetFamily());
    NapiUtils::SetUint32Property(env, jsRemoteInfo, KEY_PORT, remoteInfo->GetPort());
    NapiUtils::SetUint32Property(env, jsRemoteInfo, KEY_SIZE, remoteInfo->GetSize());

    NapiUtils::SetNamedProperty(env, obj, KEY_REMOTE_INFO, jsRemoteInfo);
    return obj;
}

static napi_value MakeMessage(napi_env env, void *para)
{
    auto messageData = reinterpret_cast<MessageData *>(para);
    auto deleter = [](const MessageData *p) { delete p; };
    std::unique_ptr<MessageData, decltype(deleter)> handler(messageData, deleter);

    if (messageData->data == nullptr || messageData->len == 0) {
        return MakeJsMessageParam(env, NapiUtils::GetUndefined(env), &messageData->remoteInfo);
    }

    void *dataHandle = nullptr;
    napi_value msgBuffer = NapiUtils::CreateArrayBuffer(env, messageData->len, &dataHandle);
    if (dataHandle == nullptr || !NapiUtils::ValueIsArrayBuffer(env, msgBuffer)) {
        return MakeJsMessageParam(env, NapiUtils::GetUndefined(env), &messageData->remoteInfo);
    }

    int result = memcpy_s(dataHandle, messageData->len, messageData->data, messageData->len);
    if (result != EOK) {
        NETSTACK_LOGI("copy ret %{public}d", result);
        return NapiUtils::GetUndefined(env);
    }

    return MakeJsMessageParam(env, msgBuffer, &messageData->remoteInfo);
}

static void OnRecvMessage(EventManager *manager, void *data, size_t len, sockaddr *addr)
{
    if (data == nullptr || len == 0) {
        return;
    }

    SocketRemoteInfo remoteInfo;
    std::string address = MakeAddressString(addr);
    if (address.empty()) {
        manager->EmitByUv(EVENT_ERROR, new int32_t(ADDRESS_INVALID), CallbackTemplate<MakeError>);
        return;
    }
    remoteInfo.SetAddress(address);
    remoteInfo.SetFamily(addr->sa_family);
    if (addr->sa_family == AF_INET) {
        auto *addr4 = reinterpret_cast<sockaddr_in *>(addr);
        remoteInfo.SetPort(ntohs(addr4->sin_port));
    } else if (addr->sa_family == AF_INET6) {
        auto *addr6 = reinterpret_cast<sockaddr_in6 *>(addr);
        remoteInfo.SetPort(ntohs(addr6->sin6_port));
    }
    remoteInfo.SetSize(len);

    manager->EmitByUv(EVENT_MESSAGE, new MessageData(data, len, remoteInfo), CallbackTemplate<MakeMessage>);
}

class MessageCallback {
public:
    MessageCallback() = delete;

    virtual ~MessageCallback() = default;

    explicit MessageCallback(EventManager *manager) : manager_(manager) {}

    virtual void OnError(int err) const = 0;

    virtual bool OnMessage(int sock, void *data, size_t dataLen, sockaddr *addr) const = 0;

protected:
    EventManager *manager_;
};

class TcpMessageCallback final : public MessageCallback {
public:
    TcpMessageCallback() = delete;

    ~TcpMessageCallback() override = default;

    explicit TcpMessageCallback(EventManager *manager) : MessageCallback(manager) {}

    void OnError(int err) const override
    {
        manager_->EmitByUv(EVENT_ERROR, new int(err), CallbackTemplate<MakeError>);
    }

    bool OnMessage(int sock, void *data, size_t dataLen, sockaddr *addr) const override
    {
        (void)addr;

        sa_family_t family;
        socklen_t len = sizeof(family);
        int ret = getsockname(sock, reinterpret_cast<sockaddr *>(&family), &len);
        if (ret < 0) {
            return false;
        }

        if (family == AF_INET) {
            sockaddr_in addr4 = {0};
            socklen_t len4 = sizeof(sockaddr_in);

            ret = getpeername(sock, reinterpret_cast<sockaddr *>(&addr4), &len4);
            if (ret < 0) {
                return false;
            }
            OnRecvMessage(manager_, data, dataLen, reinterpret_cast<sockaddr *>(&addr4));
            return true;
        } else if (family == AF_INET6) {
            sockaddr_in6 addr6 = {0};
            socklen_t len6 = sizeof(sockaddr_in6);

            ret = getpeername(sock, reinterpret_cast<sockaddr *>(&addr6), &len6);
            if (ret < 0) {
                return false;
            }
            OnRecvMessage(manager_, data, dataLen, reinterpret_cast<sockaddr *>(&addr6));
            return true;
        }
        return false;
    }
};

class UdpMessageCallback final : public MessageCallback {
public:
    UdpMessageCallback() = delete;

    ~UdpMessageCallback() override = default;

    explicit UdpMessageCallback(EventManager *manager) : MessageCallback(manager) {}

    void OnError(int err) const override
    {
        manager_->EmitByUv(EVENT_ERROR, new int(err), CallbackTemplate<MakeError>);
    }

    bool OnMessage(int sock, void *data, size_t dataLen, sockaddr *addr) const override
    {
        OnRecvMessage(manager_, data, dataLen, addr);
        return true;
    }
};

static bool MakeNonBlock(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    while (flags == -1 && errno == EINTR) {
        flags = fcntl(sock, F_GETFL, 0);
    }
    if (flags == -1) {
        NETSTACK_LOGE("make non block failed %{public}s", strerror(errno));
        return false;
    }
    int ret = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    while (ret == -1 && errno == EINTR) {
        ret = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    }
    if (ret == -1) {
        NETSTACK_LOGE("make non block failed %{public}s", strerror(errno));
        return false;
    }
    return true;
}

static bool PollSendData(int sock, const char *data, size_t size, sockaddr *addr, socklen_t addrLen)
{
    int bufferSize = DEFAULT_BUFFER_SIZE;
    int opt = 0;
    socklen_t optLen = sizeof(opt);
    if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&opt), &optLen) >= 0 && opt > 0) {
        bufferSize = opt;
    }
    int sockType = 0;
    optLen = sizeof(sockType);
    if (getsockopt(sock, SOL_SOCKET, SO_TYPE, reinterpret_cast<void *>(&sockType), &optLen) < 0) {
        NETSTACK_LOGI("get sock opt sock type failed = %{public}s", strerror(errno));
        return false;
    }

    auto curPos = data;
    auto leftSize = size;
    nfds_t num = 1;
    pollfd fds[1] = {{0}};
    fds[0].fd = sock;
    fds[0].events = 0;
    fds[0].events |= POLLOUT;

    while (leftSize > 0) {
        int ret = poll(fds, num, DEFAULT_POLL_TIMEOUT);
        if (ret == -1) {
            NETSTACK_LOGE("poll to send failed %{public}s", strerror(errno));
            return false;
        }
        if (ret == 0) {
            NETSTACK_LOGE("poll to send timeout");
            return false;
        }

        size_t sendSize = (sockType == SOCK_STREAM ? leftSize : std::min<size_t>(leftSize, bufferSize));
        auto sendLen = sendto(sock, curPos, sendSize, 0, addr, addrLen);
        if (sendLen < 0) {
            if (errno == EAGAIN) {
                continue;
            }
            NETSTACK_LOGE("send failed %{public}s", strerror(errno));
            return false;
        }
        if (sendLen == 0) {
            break;
        }
        curPos += sendLen;
        leftSize -= sendLen;
    }

    if (leftSize != 0) {
        NETSTACK_LOGE("send not complete");
        return false;
    }
    return true;
}

static int ConfirmBufferSize(int sock)
{
    int bufferSize = DEFAULT_BUFFER_SIZE;
    int opt = 0;
    socklen_t optLen = sizeof(opt);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&opt), &optLen) >= 0 && opt > 0) {
        bufferSize = opt;
    }
    return bufferSize;
}

static void PollRecvData(int sock, sockaddr *addr, socklen_t addrLen, const MessageCallback &callback)
{
    int bufferSize = ConfirmBufferSize(sock);

    auto deleter = [](char *s) { free(reinterpret_cast<void *>(s)); };
    std::unique_ptr<char, decltype(deleter)> buf(reinterpret_cast<char *>(malloc(bufferSize)), deleter);
    if (buf == nullptr) {
        callback.OnError(NO_MEMORY);
        return;
    }

    auto addrDeleter = [](sockaddr *a) { free(reinterpret_cast<void *>(a)); };
    std::unique_ptr<sockaddr, decltype(addrDeleter)> pAddr(addr, addrDeleter);

    nfds_t num = 1;
    pollfd fds[1] = {{0}};
    fds[0].fd = sock;
    fds[0].events = 0;
    fds[0].events |= POLLIN;

    while (true) {
        int ret = poll(fds, num, DEFAULT_POLL_TIMEOUT);
        if (ret < 0) {
            NETSTACK_LOGE("poll to recv failed %{public}s", strerror(errno));
            callback.OnError(errno);
            return;
        }
        if (ret == 0) {
            continue;
        }
        (void)memset_s(buf.get(), bufferSize, 0, bufferSize);
        socklen_t tempAddrLen = addrLen;
        auto recvLen = recvfrom(sock, buf.get(), bufferSize, 0, addr, &tempAddrLen);
        if (recvLen < 0) {
            if (errno == EAGAIN) {
                continue;
            }
            NETSTACK_LOGE("recv failed %{public}s", strerror(errno));
            callback.OnError(errno);
            return;
        }
        if (recvLen == 0) {
            continue;
        }

        void *data = malloc(recvLen);
        if (data == nullptr) {
            callback.OnError(NO_MEMORY);
            return;
        }
        if (memcpy_s(data, recvLen, buf.get(), recvLen) != EOK || !callback.OnMessage(sock, data, recvLen, addr)) {
            free(data);
        }
    }
}

static bool NonBlockConnect(int sock, sockaddr *addr, socklen_t addrLen, uint32_t timeoutMSec)
{
    int ret = connect(sock, addr, addrLen);
    if (ret >= 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        return false;
    }

    fd_set set = {0};
    FD_ZERO(&set);
    FD_SET(sock, &set);
    if (timeoutMSec == 0) {
        timeoutMSec = DEFAULT_CONNECT_TIMEOUT;
    }

    timeval timeout = {
        .tv_sec = (timeoutMSec / MSEC_TO_USEC) % MAX_SEC,
        .tv_usec = (timeoutMSec % MSEC_TO_USEC) * MSEC_TO_USEC,
    };

    ret = select(sock + 1, nullptr, &set, nullptr, &timeout);
    if (ret < 0) {
        NETSTACK_LOGE("select error: %{public}s\n", strerror(errno));
        return false;
    } else if (ret == 0) {
        NETSTACK_LOGE("timeout!");
        return false;
    }

    int err = 0;
    socklen_t optLen = sizeof(err);
    ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<void *>(&err), &optLen);
    if (ret < 0) {
        return false;
    }
    if (err != 0) {
        return false;
    }
    return true;
}

static void GetAddr(NetAddress *address, sockaddr_in *addr4, sockaddr_in6 *addr6, sockaddr **addr, socklen_t *len)
{
    sa_family_t family = address->GetSaFamily();
    if (family == AF_INET) {
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(address->GetPort());
        addr4->sin_addr.s_addr = inet_addr(address->GetAddress().c_str());
        *addr = reinterpret_cast<sockaddr *>(addr4);
        *len = sizeof(sockaddr_in);
    } else if (family == AF_INET6) {
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(address->GetPort());
        inet_pton(AF_INET6, address->GetAddress().c_str(), &addr6->sin6_addr);
        *addr = reinterpret_cast<sockaddr *>(addr6);
        *len = sizeof(sockaddr_in6);
    }
}

static bool SetBaseOptions(int sock, ExtraOptionsBase *option)
{
    if (option->GetReceiveBufferSize() != 0) {
        int size = (int)option->GetReceiveBufferSize();
        if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&size), sizeof(size)) < 0) {
            return false;
        }
    }

    if (option->GetSendBufferSize() != 0) {
        int size = (int)option->GetSendBufferSize();
        if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&size), sizeof(size)) < 0) {
            return false;
        }
    }

    if (option->IsReuseAddress()) {
        int reuse = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<void *>(&reuse), sizeof(reuse)) < 0) {
            return false;
        }
    }

    if (option->GetSocketTimeout() != 0) {
        timeval timeout = {(int)option->GetSocketTimeout(), 0};
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<void *>(&timeout), sizeof(timeout)) < 0) {
            return false;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<void *>(&timeout), sizeof(timeout)) < 0) {
            return false;
        }
    }

    return true;
}

int MakeTcpSocket(sa_family_t family)
{
    if (family != AF_INET && family != AF_INET6) {
        return -1;
    }
    int sock = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        NETSTACK_LOGE("make tcp socket failed errno is %{public}d %{public}s", errno, strerror(errno));
        return -1;
    }
    if (!MakeNonBlock(sock)) {
        close(sock);
        return -1;
    }
    return sock;
}

int MakeUdpSocket(sa_family_t family)
{
    if (family != AF_INET && family != AF_INET6) {
        return -1;
    }
    int sock = socket(family, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        NETSTACK_LOGE("make udp socket failed errno is %{public}d %{public}s", errno, strerror(errno));
        return -1;
    }
    if (!MakeNonBlock(sock)) {
        close(sock);
        return -1;
    }
    return sock;
}

bool ExecBind(BindContext *context)
{
    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t len;
    GetAddr(&context->address_, &addr4, &addr6, &addr, &len);
    if (addr == nullptr) {
        NETSTACK_LOGE("addr family error");
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }

    if (bind(context->GetSocketFd(), addr, len) < 0) {
        if (errno != EADDRINUSE) {
            NETSTACK_LOGE("bind error is %{public}s %{public}d", strerror(errno), errno);
            context->SetErrorCode(errno);
            return false;
        }
        if (addr->sa_family == AF_INET) {
            NETSTACK_LOGI("distribute a random port");
            addr4.sin_port = 0; /* distribute a random port */
        } else if (addr->sa_family == AF_INET6) {
            NETSTACK_LOGI("distribute a random port");
            addr6.sin6_port = 0; /* distribute a random port */
        }
        if (bind(context->GetSocketFd(), addr, len) < 0) {
            NETSTACK_LOGE("rebind error is %{public}s %{public}d", strerror(errno), errno);
            context->SetErrorCode(errno);
            return false;
        }
        NETSTACK_LOGI("rebind success");
    }
    NETSTACK_LOGI("bind success");

    return true;
}

bool ExecUdpBind(BindContext *context)
{
    if (!ExecBind(context)) {
        return false;
    }

    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t len;
    GetAddr(&context->address_, &addr4, &addr6, &addr, &len);
    if (addr == nullptr) {
        NETSTACK_LOGE("addr family error");
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }

    if (addr->sa_family == AF_INET) {
        auto pAddr4 = reinterpret_cast<sockaddr *>(malloc(sizeof(addr4)));
        if (pAddr4 == nullptr) {
            NETSTACK_LOGE("no memory!");
            return false;
        }
        NETSTACK_LOGI("copy ret = %{public}d", memcpy_s(pAddr4, sizeof(addr4), &addr4, sizeof(addr4)));
        std::thread serviceThread(PollRecvData, context->GetSocketFd(), pAddr4, sizeof(addr4),
                                  UdpMessageCallback(context->GetManager()));
        serviceThread.detach();
    } else if (addr->sa_family == AF_INET6) {
        auto pAddr6 = reinterpret_cast<sockaddr *>(malloc(sizeof(addr6)));
        if (pAddr6 == nullptr) {
            NETSTACK_LOGE("no memory!");
            return false;
        }
        NETSTACK_LOGI("copy ret = %{public}d", memcpy_s(pAddr6, sizeof(addr6), &addr6, sizeof(addr6)));
        std::thread serviceThread(PollRecvData, context->GetSocketFd(), pAddr6, sizeof(addr6),
                                  UdpMessageCallback(context->GetManager()));
        serviceThread.detach();
    }

    return true;
}

bool ExecUdpSend(UdpSendContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t len;
    GetAddr(&context->options.address, &addr4, &addr6, &addr, &len);
    if (addr == nullptr) {
        NETSTACK_LOGE("addr family error");
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }

    if (!PollSendData(context->GetSocketFd(), context->options.GetData().c_str(), context->options.GetData().size(),
                      addr, len)) {
        context->SetErrorCode(errno);
        return false;
    }
    return true;
}

bool ExecTcpBind(BindContext *context)
{
    return ExecBind(context);
}

bool ExecConnect(ConnectContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t len;
    GetAddr(&context->options.address, &addr4, &addr6, &addr, &len);
    if (addr == nullptr) {
        NETSTACK_LOGE("addr family error");
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }

    if (!NonBlockConnect(context->GetSocketFd(), addr, len, context->options.GetTimeout())) {
        NETSTACK_LOGE("connect errno %{public}d %{public}s", errno, strerror(errno));
        context->SetErrorCode(errno);
        return false;
    }

    NETSTACK_LOGI("connect success");
    std::thread serviceThread(PollRecvData, context->GetSocketFd(), nullptr, 0,
                              TcpMessageCallback(context->GetManager()));
    serviceThread.detach();
    return true;
}

bool ExecTcpSend(TcpSendContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    std::string encoding = context->options.GetEncoding();
    (void)encoding;
    /* no use for now */

    sa_family_t family;
    socklen_t len = sizeof(sa_family_t);
    if (getsockname(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&family), &len) < 0) {
        NETSTACK_LOGE("get sock name failed");
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }
    bool connected = false;
    if (family == AF_INET) {
        sockaddr_in addr4 = {0};
        socklen_t len4 = sizeof(addr4);
        int ret = getpeername(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&addr4), &len4);
        if (ret >= 0 && addr4.sin_port != 0) {
            connected = true;
        }
    } else if (family == AF_INET6) {
        sockaddr_in6 addr6 = {0};
        socklen_t len6 = sizeof(addr6);
        int ret = getpeername(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&addr6), &len6);
        if (ret >= 0 && addr6.sin6_port != 0) {
            connected = true;
        }
    }

    if (!connected) {
        NETSTACK_LOGE("sock is not connect to remote %{public}s", strerror(errno));
        context->SetErrorCode(errno);
        return false;
    }

    if (!PollSendData(context->GetSocketFd(), context->options.GetData().c_str(), context->options.GetData().size(),
                      nullptr, 0)) {
        NETSTACK_LOGE("send errno %{public}d %{public}s", errno, strerror(errno));
        context->SetErrorCode(errno);
        return false;
    }
    return true;
}

bool ExecClose(CloseContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    (void)context;

    return true;
}

bool ExecGetState(GetStateContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    int opt;
    socklen_t optLen = sizeof(int);
    int r = getsockopt(context->GetSocketFd(), SOL_SOCKET, SO_TYPE, &opt, &optLen);
    if (r < 0) {
        context->state_.SetIsClose(true);
        return true;
    }

    sa_family_t family;
    socklen_t len = sizeof(family);
    int ret = getsockname(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&family), &len);
    if (ret < 0) {
        context->SetErrorCode(errno);
        return false;
    }

    sockaddr_in addr4 = {0};
    sockaddr_in6 addr6 = {0};
    sockaddr *addr = nullptr;
    socklen_t addrLen;
    if (family == AF_INET) {
        addr = reinterpret_cast<sockaddr *>(&addr4);
        addrLen = sizeof(addr4);
    } else if (family == AF_INET6) {
        addr = reinterpret_cast<sockaddr *>(&addr6);
        addrLen = sizeof(addr6);
    }

    if (addr == nullptr) {
        context->SetErrorCode(ADDRESS_INVALID);
        return false;
    }

    (void)memset_s(addr, addrLen, 0, addrLen);
    len = addrLen;
    ret = getsockname(context->GetSocketFd(), addr, &len);
    if (ret < 0) {
        context->SetErrorCode(errno);
        return false;
    }

    SetIsBound(family, context, &addr4, &addr6);

    if (opt != SOCK_STREAM) {
        return true;
    }

    (void)memset_s(addr, addrLen, 0, addrLen);
    len = addrLen;
    ret = getpeername(context->GetSocketFd(), addr, &len);
    if (ret < 0) {
        context->SetErrorCode(errno);
        return false;
    }

    SetIsConnected(family, context, &addr4, &addr6);

    return true;
}

bool ExecGetRemoteAddress(GetRemoteAddressContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    sa_family_t family;
    socklen_t len = sizeof(family);
    int ret = getsockname(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&family), &len);
    if (ret < 0) {
        context->SetErrorCode(errno);
        return false;
    }

    if (family == AF_INET) {
        sockaddr_in addr4 = {0};
        socklen_t len4 = sizeof(sockaddr_in);

        ret = getpeername(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&addr4), &len4);
        if (ret < 0) {
            context->SetErrorCode(errno);
            return false;
        }

        std::string address = MakeAddressString(reinterpret_cast<sockaddr *>(&addr4));
        if (address.empty()) {
            context->SetErrorCode(ADDRESS_INVALID);
            return false;
        }
        context->address_.SetAddress(address);
        context->address_.SetFamilyBySaFamily(family);
        context->address_.SetPort(ntohs(addr4.sin_port));
        return true;
    } else if (family == AF_INET6) {
        sockaddr_in6 addr6 = {0};
        socklen_t len6 = sizeof(sockaddr_in6);

        ret = getpeername(context->GetSocketFd(), reinterpret_cast<sockaddr *>(&addr6), &len6);
        if (ret < 0) {
            context->SetErrorCode(errno);
            return false;
        }

        std::string address = MakeAddressString(reinterpret_cast<sockaddr *>(&addr6));
        if (address.empty()) {
            context->SetErrorCode(ADDRESS_INVALID);
            return false;
        }
        context->address_.SetAddress(address);
        context->address_.SetFamilyBySaFamily(family);
        context->address_.SetPort(ntohs(addr6.sin6_port));
        return true;
    }

    return false;
}

bool ExecTcpSetExtraOptions(TcpSetExtraOptionsContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    if (!SetBaseOptions(context->GetSocketFd(), &context->options_)) {
        context->SetErrorCode(errno);
        return false;
    }

    if (context->options_.IsKeepAlive()) {
        int keepalive = 1;
        if (setsockopt(context->GetSocketFd(), SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {
            context->SetErrorCode(errno);
            return false;
        }
    }

    if (context->options_.IsOOBInline()) {
        int oobInline = 1;
        if (setsockopt(context->GetSocketFd(), SOL_SOCKET, SO_OOBINLINE, &oobInline, sizeof(oobInline)) < 0) {
            context->SetErrorCode(errno);
            return false;
        }
    }

    if (context->options_.IsTCPNoDelay()) {
        int tcpNoDelay = 1;
        if (setsockopt(context->GetSocketFd(), IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(tcpNoDelay)) < 0) {
            context->SetErrorCode(errno);
            return false;
        }
    }

    linger soLinger = {0};
    soLinger.l_onoff = context->options_.socketLinger.IsOn();
    soLinger.l_linger = (int)context->options_.socketLinger.GetLinger();
    if (setsockopt(context->GetSocketFd(), SOL_SOCKET, SO_LINGER, &soLinger, sizeof(soLinger)) < 0) {
        context->SetErrorCode(errno);
        return false;
    }

    return true;
}

bool ExecUdpSetExtraOptions(UdpSetExtraOptionsContext *context)
{
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }

    if (!SetBaseOptions(context->GetSocketFd(), &context->options)) {
        context->SetErrorCode(errno);
        return false;
    }

    if (context->options.IsBroadcast()) {
        int broadcast = 1;
        if (setsockopt(context->GetSocketFd(), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
            context->SetErrorCode(errno);
            return false;
        }
    }

    return true;
}

napi_value BindCallback(BindContext *context)
{
    context->Emit(EVENT_LISTENING, std::make_pair(NapiUtils::GetUndefined(context->GetEnv()),
                                                  NapiUtils::GetUndefined(context->GetEnv())));
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value UdpSendCallback(UdpSendContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value ConnectCallback(ConnectContext *context)
{
    context->Emit(EVENT_CONNECT, std::make_pair(NapiUtils::GetUndefined(context->GetEnv()),
                                                NapiUtils::GetUndefined(context->GetEnv())));
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value TcpSendCallback(TcpSendContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value CloseCallback(CloseContext *context)
{
    int ret = close(context->GetSocketFd());
    if (ret < 0) {
        NETSTACK_LOGE("sock closed error %{public}s sock = %{public}d, ret = %{public}d", strerror(errno),
                      context->GetSocketFd(), ret);
    } else {
        NETSTACK_LOGI("sock %{public}d closed success", context->GetSocketFd());
    }
    context->SetSocketFd(0);
    context->Emit(EVENT_CLOSE, std::make_pair(NapiUtils::GetUndefined(context->GetEnv()),
                                              NapiUtils::GetUndefined(context->GetEnv())));
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value GetStateCallback(GetStateContext *context)
{
    napi_value obj = NapiUtils::CreateObject(context->GetEnv());
    if (NapiUtils::GetValueType(context->GetEnv(), obj) != napi_object) {
        return NapiUtils::GetUndefined(context->GetEnv());
    }

    NapiUtils::SetBooleanProperty(context->GetEnv(), obj, KEY_IS_BOUND, context->state_.IsBound());
    NapiUtils::SetBooleanProperty(context->GetEnv(), obj, KEY_IS_CLOSE, context->state_.IsClose());
    NapiUtils::SetBooleanProperty(context->GetEnv(), obj, KEY_IS_CONNECTED, context->state_.IsConnected());

    return obj;
}

napi_value GetRemoteAddressCallback(GetRemoteAddressContext *context)
{
    napi_value obj = NapiUtils::CreateObject(context->GetEnv());
    if (NapiUtils::GetValueType(context->GetEnv(), obj) != napi_object) {
        return NapiUtils::GetUndefined(context->GetEnv());
    }

    NapiUtils::SetStringPropertyUtf8(context->GetEnv(), obj, KEY_ADDRESS, context->address_.GetAddress());
    NapiUtils::SetUint32Property(context->GetEnv(), obj, KEY_FAMILY, context->address_.GetJsValueFamily());
    NapiUtils::SetUint32Property(context->GetEnv(), obj, KEY_PORT, context->address_.GetPort());

    return obj;
}

napi_value TcpSetExtraOptionsCallback(TcpSetExtraOptionsContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value UdpSetExtraOptionsCallback(UdpSetExtraOptionsContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}
} // namespace OHOS::NetStack::SocketExec
