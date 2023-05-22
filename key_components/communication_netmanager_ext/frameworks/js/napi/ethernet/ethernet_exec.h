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

#ifndef NET_EXT_NAPI_ETHERNET_EXEC_H
#define NET_EXT_NAPI_ETHERNET_EXEC_H

#include <napi/native_api.h>

#include "get_all_active_ifaces_context.h"
#include "get_iface_config_context.h"
#include "is_iface_active_context.h"
#include "set_iface_config_context.h"

namespace OHOS {
namespace NetManagerStandard {
namespace EthernetExec {
bool ExecGetIfaceConfig(GetIfaceConfigContext *context);
napi_value GetIfaceConfigCallback(GetIfaceConfigContext *context);

bool ExecSetIfaceConfig(SetIfaceConfigContext *context);
napi_value SetIfaceConfigCallback(SetIfaceConfigContext *context);

bool ExecIsIfaceActive(IsIfaceActiveContext *context);
napi_value IsIfaceActiveCallback(IsIfaceActiveContext *context);

bool ExecGetAllActiveIfaces(GetAllActiveIfacesContext *context);
napi_value GetAllActiveIfacesCallback(GetAllActiveIfacesContext *context);
} // namespace EthernetExec
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_EXT_NAPI_ETHERNET_EXEC_H
