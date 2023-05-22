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

#include "monitor.h"

#include <cstddef>
#include <utility>

#include <napi/native_api.h>
#include <napi/native_common.h>
#include <securec.h>
#include <uv.h>

#include "module_template.h"
#include "napi_utils.h"
#include "netstack_log.h"
#include "tls_socket.h"

namespace OHOS {
namespace NetStack {
namespace {
constexpr int PARAM_OPTION = 1;
constexpr int PARAM_OPTION_CALLBACK = 2;
constexpr std::string_view EVENT_MESSAGE = "message";
constexpr std::string_view EVENT_CONNECT = "connect";
constexpr std::string_view EVENT_CLOSE = "close";
constexpr std::string_view EVENT_ERROR = "error";

constexpr const char *PROPERTY_ADDRESS = "address";
constexpr const char *PROPERTY_FAMILY = "family";
constexpr const char *PROPERTY_PORT = "port";
constexpr const char *PROPERTY_SIZE = "size";
constexpr const char *ON_MESSAGE = "message";
constexpr const char *ON_REMOTE_INFO = "remoteInfo";

void EventMessageCallback(uv_work_t *work, int status)
{
    (void)status;
    if (work == nullptr) {
        NETSTACK_LOGE("work is nullptr");
        return;
    }
    auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
    if (workWrapper == nullptr) {
        NETSTACK_LOGE("workWrapper is nullptr");
        delete work;
        return;
    }
    auto monitor = static_cast<Monitor *>(workWrapper->data);
    if (monitor == nullptr) {
        NETSTACK_LOGE("monitor is nullptr");
        delete workWrapper;
        delete work;
        return;
    }
    napi_handle_scope scope = NapiUtils::OpenScope(workWrapper->env);
    napi_value obj = NapiUtils::CreateObject(workWrapper->env);
    napi_value remoteInfo = NapiUtils::CreateObject(workWrapper->env);
    void *data = nullptr;
    napi_value arrayBuffer = NapiUtils::CreateArrayBuffer(workWrapper->env, monitor->data_.size(), &data);
    if (data != nullptr && arrayBuffer != nullptr) {
        if (memcpy_s(data, monitor->data_.size(), monitor->data_.c_str(), monitor->data_.size()) != EOK) {
            NETSTACK_LOGE("memcpy_s failed!");
            delete workWrapper;
            delete work;
            return;
        }
    }
    napi_value message = nullptr;
    napi_create_typedarray(workWrapper->env, napi_uint8_array, monitor->data_.size(), arrayBuffer, 0, &message);
    napi_value address = NapiUtils::CreateStringUtf8(workWrapper->env, monitor->remoteInfo_.GetAddress());
    napi_value family = NapiUtils::CreateStringUtf8(workWrapper->env, monitor->remoteInfo_.GetFamily());
    napi_value port = NapiUtils::CreateInt32(workWrapper->env, monitor->remoteInfo_.GetPort());
    napi_value size = NapiUtils::CreateInt32(workWrapper->env, monitor->remoteInfo_.GetSize());
    NapiUtils::SetNamedProperty(workWrapper->env, remoteInfo, PROPERTY_ADDRESS, address);
    NapiUtils::SetNamedProperty(workWrapper->env, remoteInfo, PROPERTY_FAMILY, family);
    NapiUtils::SetNamedProperty(workWrapper->env, remoteInfo, PROPERTY_PORT, port);
    NapiUtils::SetNamedProperty(workWrapper->env, remoteInfo, PROPERTY_SIZE, size);
    NapiUtils::SetNamedProperty(workWrapper->env, obj, ON_MESSAGE, message);
    NapiUtils::SetNamedProperty(workWrapper->env, obj, ON_REMOTE_INFO, remoteInfo);
    if (workWrapper->manager == nullptr) {
        NETSTACK_LOGE("manager is nullptr");
        delete workWrapper;
        delete work;
        return;
    }
    workWrapper->manager->Emit(workWrapper->type, std::make_pair(NapiUtils::GetUndefined(workWrapper->env), obj));
    NapiUtils::CloseScope(workWrapper->env, scope);
    delete workWrapper;
    delete work;
}

void EventConnectCloseCallback(uv_work_t *work, int status)
{
    (void)status;
    if (work == nullptr) {
        NETSTACK_LOGE("work is nullptr");
        return;
    }
    auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
    if (workWrapper == nullptr) {
        NETSTACK_LOGE("workWrapper is nullptr");
        delete work;
        return;
    }
    napi_handle_scope scope = NapiUtils::OpenScope(workWrapper->env);
    std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env),
                                             NapiUtils::GetUndefined(workWrapper->env)};
    if (workWrapper->manager == nullptr) {
        NETSTACK_LOGE("manager is nullptr");
        delete workWrapper;
        delete work;
        return;
    }
    workWrapper->manager->Emit(workWrapper->type, arg);
    NapiUtils::CloseScope(workWrapper->env, scope);
    delete workWrapper;
    delete work;
}

void EventErrorCallback(uv_work_t *work, int status)
{
    (void)status;
    if (work == nullptr) {
        NETSTACK_LOGE("work is nullptr");
        return;
    }
    auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
    if (workWrapper == nullptr) {
        NETSTACK_LOGE("workWrapper is nullptr");
        delete work;
        return;
    }
    auto monitor = static_cast<Monitor *>(workWrapper->data);
    if (monitor == nullptr) {
        NETSTACK_LOGE("monitor is nullptr");
        delete workWrapper;
        delete work;
        return;
    }
    napi_handle_scope scope = NapiUtils::OpenScope(workWrapper->env);
    napi_value obj = NapiUtils::CreateObject(workWrapper->env);
    napi_value errorNumber = NapiUtils::CreateInt32(workWrapper->env, monitor->errorNumber_);
    napi_value errorString = NapiUtils::CreateStringUtf8(workWrapper->env, monitor->errorString_);
    NapiUtils::SetNamedProperty(workWrapper->env, obj, "errorNumber", errorNumber);
    NapiUtils::SetNamedProperty(workWrapper->env, obj, "errorString", errorString);
    std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env), obj};
    if (workWrapper->manager == nullptr) {
        NETSTACK_LOGE("manager is nullptr");
        delete workWrapper;
        delete work;
        return;
    }
    workWrapper->manager->Emit(workWrapper->type, arg);
    NapiUtils::CloseScope(workWrapper->env, scope);
    delete workWrapper;
    delete work;
}
} // namespace

Monitor::Monitor() : manager_(nullptr) {}

Monitor::~Monitor()
{
    if (manager_ != nullptr) {
        delete manager_;
    }
}

napi_value Monitor::On(napi_env env, napi_callback_info info)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));
    if (paramsCount == PARAM_OPTION) {
        return NapiUtils::GetUndefined(env);
    }
    if (paramsCount != PARAM_OPTION_CALLBACK) {
        if (NapiUtils::GetValueType(env, params[0]) != napi_string) {
            napi_throw_error(env, std::to_string(PARSE_ERROR_CODE).c_str(), PARSE_ERROR_MSG);
        }
        if (NapiUtils::GetValueType(env, params[1]) != napi_function) {
            return NapiUtils::GetUndefined(env);
        }
    }
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager_));
    if (manager_ == nullptr) {
        NETSTACK_LOGE("manager is nullptr");
        return NapiUtils::GetUndefined(env);
    }
    auto tlsSocket = reinterpret_cast<TLSSocket *>(manager_->GetData());
    if (tlsSocket == nullptr) {
        NETSTACK_LOGE("tlsSocket is null");
        return NapiUtils::GetUndefined(env);
    }

    const std::string event = NapiUtils::GetStringFromValueUtf8(env, params[0]);
    auto itor = monitors_.find(event);
    if (itor != monitors_.end()) {
        NETSTACK_LOGE("monitor is exits %{public}s", event.c_str());
        return NapiUtils::GetUndefined(env);
    }
    manager_->AddListener(env, event, params[1], false, false);
    if (event == EVENT_MESSAGE) {
        monitors_.insert(EVENT_MESSAGE);
        tlsSocket->OnMessage([this](auto data, auto remoteInfo) {
            this->data_ = data;
            this->remoteInfo_ = remoteInfo;
            manager_->EmitByUv(std::string(EVENT_MESSAGE), static_cast<void *>(this), EventMessageCallback);
        });
    }
    if (event == EVENT_CLOSE) {
        monitors_.insert(EVENT_CLOSE);
        tlsSocket->OnClose(
            [this]() { manager_->EmitByUv(std::string(EVENT_CLOSE), nullptr, EventConnectCloseCallback); });
    }
    if (event == EVENT_CONNECT) {
        monitors_.insert(EVENT_CONNECT);
        tlsSocket->OnConnect(
            [this]() { manager_->EmitByUv(std::string(EVENT_CONNECT), nullptr, EventConnectCloseCallback); });
    }
    if (event == EVENT_ERROR) {
        monitors_.insert(EVENT_ERROR);
        tlsSocket->OnError([this](auto errorNumber, auto errorString) {
            this->errorNumber_ = errorNumber;
            this->errorString_ = errorString;
            manager_->EmitByUv(std::string(EVENT_ERROR), static_cast<void *>(this), EventErrorCallback);
        });
    }
    return NapiUtils::GetUndefined(env);
}

napi_value Monitor::Off(napi_env env, napi_callback_info info)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));
    if (paramsCount == PARAM_OPTION) {
        return NapiUtils::GetUndefined(env);
    }

    if (paramsCount != PARAM_OPTION_CALLBACK) {
        if (NapiUtils::GetValueType(env, params[0]) != napi_string) {
            napi_throw_error(env, std::to_string(PARSE_ERROR_CODE).c_str(), PARSE_ERROR_MSG);
        }
        if (NapiUtils::GetValueType(env, params[1]) != napi_function) {
            return NapiUtils::GetUndefined(env);
        }
    }

    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager_));
    if (manager_ == nullptr) {
        NETSTACK_LOGE("manager is nullptr");
        return NapiUtils::GetUndefined(env);
    }
    auto tlsSocket = reinterpret_cast<TLSSocket *>(manager_->GetData());
    if (tlsSocket == nullptr) {
        NETSTACK_LOGE("tlsSocket is null");
        return NapiUtils::GetUndefined(env);
    }

    const std::string event = NapiUtils::GetStringFromValueUtf8(env, params[0]);
    auto itor = monitors_.find(event);
    if (itor == monitors_.end()) {
        NETSTACK_LOGE("monitor is off %{public}s", event.c_str());
        return NapiUtils::GetUndefined(env);
    }
    manager_->DeleteListener(event);
    if (event == EVENT_MESSAGE) {
        monitors_.erase(EVENT_MESSAGE);
        tlsSocket->OffMessage();
    }
    if (event == EVENT_CLOSE) {
        monitors_.erase(EVENT_CLOSE);
        tlsSocket->OffClose();
    }
    if (event == EVENT_CONNECT) {
        monitors_.erase(EVENT_CONNECT);
        tlsSocket->OffConnect();
    }
    if (event == EVENT_ERROR) {
        monitors_.erase(EVENT_ERROR);
        tlsSocket->OffError();
    }
    return NapiUtils::GetUndefined(env);
}
} // namespace NetStack
} // namespace OHOS
