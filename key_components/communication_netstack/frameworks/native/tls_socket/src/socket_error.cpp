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

#include "socket_error.h"

#include <cstring>
#include <map>

#include <openssl/err.h>
#include <openssl/ssl.h>

namespace OHOS {
namespace NetStack {
static constexpr int32_t ERROR_DIVISOR = 1000;
static constexpr int32_t ERROR_RANGE = 500;
static constexpr const size_t MAX_ERR_LEN = 1024;

std::string MakeErrorMessage(int error)
{
    static const std::map<int32_t, std::string> ERROR_MAP = {
        {TLS_ERR_PERMISSION_DENIED, "Permission denied"},
        {TLS_ERR_SYS_EINTR, "Interrupted system call"},
        {TLS_ERR_SYS_EIO, "I/O error"},
        {TLS_ERR_SYS_EBADF, "Bad file number"},
        {TLS_ERR_SYS_EAGAIN, "Resource temporarily unavailable try again"},
        {TLS_ERR_SYS_EACCES, "System permission denied"},
        {TLS_ERR_SYS_EFAULT, "Bad address"},
        {TLS_ERR_SYS_EINVAL, "Invalid system argument"},
        {TLS_ERR_SYS_ENOTSOCK, "Socket operation on non-socket"},
        {TLS_ERR_SYS_EPROTOTYPE, "Protocol wrong type for socket"},
        {TLS_ERR_SYS_EADDRINUSE, "Address already in use"},
        {TLS_ERR_SYS_EADDRNOTAVAIL, "Cannot assign requested address"},
        {TLS_ERR_SYS_ENOTCONN, "Transport endpoint is not connected"},
        {TLS_ERR_SYS_ETIMEDOUT, "Connection timed out"},
        {TLS_ERR_SSL_NULL, "SSL is null"},
        {TLS_ERR_WANT_READ, "Error in tls reading"},
        {TLS_ERR_WANT_WRITE, "Error in tls writing"},
        {TLS_ERR_WANT_X509_LOOKUP, "Error looking up x509"},
        {TLS_ERR_SYSCALL, "Error occurred in the tls system call"},
        {TLS_ERR_ZERO_RETURN, "Error clearing tls connection"},
        {TLS_ERR_WANT_CONNECT, "Error occurred in the tls connection"},
        {TLS_ERR_WANT_ACCEPT, "Error occurred in the tls accept"},
        {TLS_ERR_WANT_ASYNC, "Error occurred in the tls async"},
        {TLS_ERR_WANT_ASYNC_JOB, "Error occurred in the tls async work"},
        {TLS_ERR_WANT_CLIENT_HELLO_CB, "Error occured in client hello"},
        {TLS_ERR_NO_BIND, "No bind socket"},
    };
    auto search = ERROR_MAP.find(error);
    if (search != ERROR_MAP.end()) {
        return search->second;
    }
    if ((error % ERROR_DIVISOR) < ERROR_RANGE) {
        return strerror(errno);
    }
    char err[MAX_ERR_LEN] = {0};
    ERR_error_string_n(error - TLS_ERR_SYS_BASE, err, sizeof(err));
    return err;
}
} // namespace NetStack
} // namespace OHOS
