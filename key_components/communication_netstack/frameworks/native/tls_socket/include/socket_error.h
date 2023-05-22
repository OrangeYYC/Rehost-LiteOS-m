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

#ifndef COMMUNICATION_NETSTACK_SOCKET_ERROR_H
#define COMMUNICATION_NETSTACK_SOCKET_ERROR_H

#include <string>
#include <map>

namespace OHOS {
namespace NetStack {
enum TlsSocketError {
    TLSSOCKET_SUCCESS = 0,
    TLS_ERR_PERMISSION_DENIED = 201,
    TLS_ERR_SYS_BASE = 2303100,
    TLS_ERR_SYS_EINTR = 2303104,
    TLS_ERR_SYS_EIO = 2303105,
    TLS_ERR_SYS_EBADF = 2303109,
    TLS_ERR_SYS_EAGAIN = 2303111,
    TLS_ERR_SYS_EACCES = 2303113,
    TLS_ERR_SYS_EFAULT = 2303114,
    TLS_ERR_SYS_EINVAL = 2303122,
    TLS_ERR_SYS_ENOTSOCK = 2303188,
    TLS_ERR_SYS_EPROTOTYPE = 2303191,
    TLS_ERR_SYS_EADDRINUSE = 2303198,
    TLS_ERR_SYS_EADDRNOTAVAIL = 2303199,
    TLS_ERR_SYS_ENOTCONN = 2303207,
    TLS_ERR_SYS_ETIMEDOUT = 2303210,
    TLS_ERR_SSL_BASE = 2303500,
    TLS_ERR_SSL_NULL = 2303501,
    TLS_ERR_WANT_READ = 2303502,
    TLS_ERR_WANT_WRITE = 2303503,
    TLS_ERR_WANT_X509_LOOKUP = 2303504,
    TLS_ERR_SYSCALL = 2303505,
    TLS_ERR_ZERO_RETURN = 2303506,
    TLS_ERR_WANT_CONNECT = 2303507,
    TLS_ERR_WANT_ACCEPT = 2303508,
    TLS_ERR_WANT_ASYNC = 2303509,
    TLS_ERR_WANT_ASYNC_JOB = 2303510,
    TLS_ERR_WANT_CLIENT_HELLO_CB = 2303511,
    TLS_ERR_NO_BIND = 2303600,
};

std::string MakeErrorMessage(int error);
} // namespace NetStack
} // namespace OHOS
#endif // COMMUNICATION_NETSTACK_SOCKET_ERROR_H
