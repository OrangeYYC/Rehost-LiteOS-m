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

#include "tlssocket_async_work.h"

#include "base_async_work.h"
#include "tls_bind_context.h"
#include "common_context.h"
#include "netstack_log.h"
#include "tls_send_context.h"
#include "tls_extra_context.h"
#include "tls_napi_context.h"
#include "tls_connect_context.h"
#include "tlssocket_exec.h"

namespace OHOS {
namespace NetStack {
void TLSSocketAsyncWork::ExecGetCertificate(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetCertificateContext, TLSSocketExec::ExecGetCertificate>(env, data);
}

void TLSSocketAsyncWork::ExecConnect(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSConnectContext, TLSSocketExec::ExecConnect>(env, data);
}

void TLSSocketAsyncWork::ExecGetCipherSuites(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetCipherSuitesContext, TLSSocketExec::ExecGetCipherSuites>(env, data);
}

void TLSSocketAsyncWork::ExecGetRemoteCertificate(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetRemoteCertificateContext, TLSSocketExec::ExecGetRemoteCertificate>(env, data);
}

void TLSSocketAsyncWork::ExecGetProtocol(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetProtocolContext, TLSSocketExec::ExecGetProtocol>(env, data);
}

void TLSSocketAsyncWork::ExecGetSignatureAlgorithms(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSignatureAlgorithmsContext, TLSSocketExec::ExecGetSignatureAlgorithms>(env, data);
}

void TLSSocketAsyncWork::ExecSend(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSSendContext, TLSSocketExec::ExecSend>(env, data);
}

void TLSSocketAsyncWork::ExecClose(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSNapiContext, TLSSocketExec::ExecClose>(env, data);
}

void TLSSocketAsyncWork::ExecBind(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSBindContext, TLSSocketExec::ExecBind>(env, data);
}

void TLSSocketAsyncWork::ExecGetState(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSGetStateContext, TLSSocketExec::ExecGetState>(env, data);
}

void TLSSocketAsyncWork::ExecGetRemoteAddress(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSGetRemoteAddressContext, TLSSocketExec::ExecGetRemoteAddress>(env, data);
}

void TLSSocketAsyncWork::ExecSetExtraOptions(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TLSSetExtraOptionsContext, TLSSocketExec::ExecSetExtraOptions>(env, data);
}

void TLSSocketAsyncWork::GetCertificateCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetCertificateContext, TLSSocketExec::GetCertificateCallback>(env, status, data);
}

void TLSSocketAsyncWork::ConnectCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSConnectContext, TLSSocketExec::ConnectCallback>(env, status, data);
}

void TLSSocketAsyncWork::GetCipherSuitesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetCipherSuitesContext, TLSSocketExec::GetCipherSuitesCallback>(env, status, data);
}

void TLSSocketAsyncWork::GetRemoteCertificateCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetRemoteCertificateContext, TLSSocketExec::GetRemoteCertificateCallback>(
        env, status, data);
}

void TLSSocketAsyncWork::GetProtocolCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetProtocolContext, TLSSocketExec::GetProtocolCallback>(env, status, data);
}

void TLSSocketAsyncWork::GetSignatureAlgorithmsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSignatureAlgorithmsContext, TLSSocketExec::GetSignatureAlgorithmsCallback>(
        env, status, data);
}

void TLSSocketAsyncWork::SendCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSSendContext, TLSSocketExec::SendCallback>(env, status, data);
}

void TLSSocketAsyncWork::CloseCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSNapiContext, TLSSocketExec::CloseCallback>(env, status, data);
}

void TLSSocketAsyncWork::BindCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSBindContext, TLSSocketExec::BindCallback>(env, status, data);
}

void TLSSocketAsyncWork::GetStateCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSGetStateContext, TLSSocketExec::GetStateCallback>(env, status, data);
}

void TLSSocketAsyncWork::GetRemoteAddressCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSGetRemoteAddressContext, TLSSocketExec::GetRemoteAddressCallback>(env, status,
                                                                                                          data);
}

void TLSSocketAsyncWork::SetExtraOptionsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TLSSetExtraOptionsContext, TLSSocketExec::SetExtraOptionsCallback>(env, status,
                                                                                                        data);
}
} // namespace NetStack
} // namespace OHOS
