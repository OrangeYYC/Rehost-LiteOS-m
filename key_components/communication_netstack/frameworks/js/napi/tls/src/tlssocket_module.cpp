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

#include "tlssocket_module.h"

#include <initializer_list>
#include <napi/native_common.h>

#include "common_context.h"
#include "event_manager.h"
#include "module_template.h"
#include "monitor.h"
#include "napi_utils.h"
#include "netstack_log.h"
#include "tls_send_context.h"
#include "tls_bind_context.h"
#include "tls_extra_context.h"
#include "tls.h"
#include "tls_napi_context.h"
#include "tls_connect_context.h"
#include "tlssocket_async_work.h"

namespace OHOS {
namespace NetStack {
namespace {
static constexpr const char *PROTOCOL_TLSV13 = "TLSv13";
static constexpr const char *PROTOCOL_TLSV12 = "TLSv12";

void Finalize(napi_env, void *data, void *)
{
    auto manager = reinterpret_cast<EventManager *>(data);
    if (manager != nullptr) {
        manager->SetInvalid();
    }
}
} // namespace

napi_value TLSSocketModuleExports::TLSSocket::GetCertificate(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetCertificateContext>(env, info, FUNCTION_GET_CERTIFICATE, nullptr,
                                                            TLSSocketAsyncWork::ExecGetCertificate,
                                                            TLSSocketAsyncWork::GetCertificateCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetProtocol(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetCipherSuitesContext>(env, info, FUNCTION_GET_PROTOCOL, nullptr,
                                                             TLSSocketAsyncWork::ExecGetProtocol,
                                                             TLSSocketAsyncWork::GetProtocolCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::Connect(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSConnectContext>(
        env, info, FUNCTION_CONNECT, nullptr, TLSSocketAsyncWork::ExecConnect, TLSSocketAsyncWork::ConnectCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetCipherSuites(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetCipherSuitesContext>(env, info, FUNCTION_GET_CIPHER_SUITE, nullptr,
                                                             TLSSocketAsyncWork::ExecGetCipherSuites,
                                                             TLSSocketAsyncWork::GetCipherSuitesCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetRemoteCertificate(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetRemoteCertificateContext>(env, info, FUNCTION_GET_REMOTE_CERTIFICATE, nullptr,
                                                                  TLSSocketAsyncWork::ExecGetRemoteCertificate,
                                                                  TLSSocketAsyncWork::GetRemoteCertificateCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetSignatureAlgorithms(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetSignatureAlgorithmsContext>(
        env, info, FUNCTION_GET_SIGNATURE_ALGORITHMS, nullptr, TLSSocketAsyncWork::ExecGetSignatureAlgorithms,
        TLSSocketAsyncWork::GetSignatureAlgorithmsCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::Send(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSSendContext>(env, info, FUNCTION_SEND, nullptr, TLSSocketAsyncWork::ExecSend,
                                                     TLSSocketAsyncWork::SendCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::Close(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSNapiContext>(env, info, FUNCTION_CLOSE, nullptr, TLSSocketAsyncWork::ExecClose,
                                                     TLSSocketAsyncWork::CloseCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::Bind(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSBindContext>(env, info, FUNCTION_BIND, nullptr, TLSSocketAsyncWork::ExecBind,
                                                     TLSSocketAsyncWork::BindCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetState(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSGetStateContext>(
        env, info, FUNCTION_GET_STATE, nullptr, TLSSocketAsyncWork::ExecGetState, TLSSocketAsyncWork::GetStateCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::GetRemoteAddress(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSGetRemoteAddressContext>(env, info, FUNCTION_GET_REMOTE_ADDRESS, nullptr,
                                                              TLSSocketAsyncWork::ExecGetRemoteAddress,
                                                              TLSSocketAsyncWork::GetRemoteAddressCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::SetExtraOptions(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<TLSSetExtraOptionsContext>(env, info, FUNCTION_BIND, nullptr,
                                                                TLSSocketAsyncWork::ExecSetExtraOptions,
                                                                TLSSocketAsyncWork::SetExtraOptionsCallback);
}

napi_value TLSSocketModuleExports::TLSSocket::On(napi_env env, napi_callback_info info)
{
    return DelayedSingleton<Monitor>::GetInstance()->On(env, info);
}

napi_value TLSSocketModuleExports::TLSSocket::Off(napi_env env, napi_callback_info info)
{
    return DelayedSingleton<Monitor>::GetInstance()->Off(env, info);
}

void TLSSocketModuleExports::DefineTLSSocketClass(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> functions = {
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_CERTIFICATE, TLSSocket::GetCertificate),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_REMOTE_CERTIFICATE, TLSSocket::GetRemoteCertificate),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_SIGNATURE_ALGORITHMS, TLSSocket::GetSignatureAlgorithms),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_PROTOCOL, TLSSocket::GetProtocol),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_CONNECT, TLSSocket::Connect),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_CIPHER_SUITE, TLSSocket::GetCipherSuites),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_SEND, TLSSocket::Send),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_CLOSE, TLSSocket::Close),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_BIND, TLSSocket::Bind),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_STATE, TLSSocket::GetState),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_GET_REMOTE_ADDRESS, TLSSocket::GetRemoteAddress),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_SET_EXTRA_OPTIONS, TLSSocket::SetExtraOptions),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_ON, TLSSocket::On),
        DECLARE_NAPI_FUNCTION(TLSSocket::FUNCTION_OFF, TLSSocket::Off),
    };
    ModuleTemplate::DefineClass(env, exports, functions, INTERFACE_TLS_SOCKET);
}

void TLSSocketModuleExports::InitProtocol(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_STATIC_PROPERTY(PROTOCOL_TLSV12, NapiUtils::CreateStringUtf8(env, PROTOCOL_TLS_V12)),
        DECLARE_NAPI_STATIC_PROPERTY(PROTOCOL_TLSV13, NapiUtils::CreateStringUtf8(env, PROTOCOL_TLS_V13)),
    };

    napi_value protocol = NapiUtils::CreateObject(env);
    NapiUtils::DefineProperties(env, protocol, properties);
    NapiUtils::SetNamedProperty(env, exports, INTERFACE_PROTOCOL, protocol);
}

napi_value TLSSocketModuleExports::ConstructTLSSocketInstance(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::NewInstance(env, info, INTERFACE_TLS_SOCKET, Finalize);
}

void TLSSocketModuleExports::InitTLSSocketProperties(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(FUNCTION_CONSTRUCTOR_TLS_SOCKET_INSTANCE, ConstructTLSSocketInstance),
    };
    NapiUtils::DefineProperties(env, exports, properties);
}

napi_value TLSSocketModuleExports::InitTLSSocketModule(napi_env env, napi_value exports)
{
    DefineTLSSocketClass(env, exports);
    InitTLSSocketProperties(env, exports);
    InitProtocol(env, exports);
    return exports;
}
} // namespace NetStack
} // namespace OHOS
