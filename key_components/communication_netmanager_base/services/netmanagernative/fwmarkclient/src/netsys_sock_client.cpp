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

#include "netsys_sock_client.h"

#include <atomic>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "fwmark_client.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace {
std::atomic_int g_netIdForApp(0);
std::atomic<const SocketDispatchType*> g_dispatch(nullptr);
std::atomic_bool g_hookFlag(false);
const SocketDispatchType* GetDispatch()
{
    return g_dispatch.load(std::memory_order_relaxed);
}
} // namespace

int HookSocket(int (*fn)(int, int, int), int domain, int type, int protocol)
{
    int fd = -1;
    if (fn) {
        fd = fn(domain, type, protocol);
    }

    if (fd < 0) {
        NETNATIVE_LOGE("musl create socket failed");
        return fd;
    }

    if (g_netIdForApp > 0) {
        if (OHOS::nmd::FwmarkClient().BindSocket(fd, g_netIdForApp) != OHOS::NetManagerStandard::NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("BindSocket [%{public}d] to netid [%{public}d] failed",
                fd, g_netIdForApp.load(std::memory_order_relaxed));
            return -1;
        }
    }

    return fd;
}

bool ohos_socket_hook_initialize(const SocketDispatchType* disptch, bool*, const char*)
{
    g_dispatch.store(disptch);
    g_hookFlag = true;
    return true;
}

void ohos_socket_hook_finalize(void)
{
    g_hookFlag = false;
}

int ohos_socket_hook_socket(int domain, int type, int protocol)
{
    g_hookFlag = false;
    int ret = HookSocket(GetDispatch()->socket, domain, type, protocol);
    g_hookFlag = true;
    return ret;
}

bool ohos_socket_hook_get_hook_flag(void)
{
    return g_hookFlag;
}

bool ohos_socket_hook_set_hook_flag(bool flag)
{
    g_hookFlag = flag;
    return true;
}

void SetNetForApp(int netId)
{
    g_netIdForApp = netId;
}

int GetNetForApp()
{
    return g_netIdForApp;
}
