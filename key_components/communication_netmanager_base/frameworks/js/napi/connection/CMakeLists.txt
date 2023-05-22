# Copyright (C) 2022 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_directories(connection_module/include)
include_directories(async_work/include)
include_directories(connection_exec/include)
include_directories(async_context/include)
include_directories(options/include)
include_directories(constant/include)
include_directories(observer/include)

add_library(connection SHARED
    options/src/net_address.cpp
    async_context/src/getaddressbyname_context.cpp
    async_context/src/getdefaultnet_context.cpp
    async_context/src/parse_nethandle_context.cpp
    connection_exec/src/connection_exec.cpp
    async_work/src/connection_async_work.cpp
    connection_module/src/connection_module.cpp
    options/src/netconnection.cpp
    async_context/src/register_context.cpp
    async_context/src/bindsocket_context.cpp
    observer/src/net_conn_callback_observer.cpp
    )
