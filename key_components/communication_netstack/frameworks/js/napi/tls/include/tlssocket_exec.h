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

#ifndef TLS_TLSSOCKET_EXEC_H
#define TLS_TLSSOCKET_EXEC_H

#include <napi/native_api.h>

#include "common_context.h"
#include "tls_send_context.h"
#include "tls_bind_context.h"
#include "tls_extra_context.h"
#include "tls_napi_context.h"
#include "tls_connect_context.h"

namespace OHOS {
namespace NetStack {
class TLSSocketExec final {
public:
    TLSSocketExec() = delete;
    ~TLSSocketExec() = delete;

    static bool ExecGetCertificate(GetCertificateContext *context);
    static bool ExecConnect(TLSConnectContext *context);
    static bool ExecGetCipherSuites(GetCipherSuitesContext *context);
    static bool ExecGetRemoteCertificate(GetRemoteCertificateContext *context);
    static bool ExecGetProtocol(GetProtocolContext *context);
    static bool ExecGetSignatureAlgorithms(GetSignatureAlgorithmsContext *context);
    static bool ExecSend(TLSSendContext *context);
    static bool ExecClose(TLSNapiContext *context);
    static bool ExecBind(TLSBindContext *context);
    static bool ExecGetState(TLSGetStateContext *context);
    static bool ExecGetRemoteAddress(TLSGetRemoteAddressContext *context);
    static bool ExecSetExtraOptions(TLSSetExtraOptionsContext *context);

    static napi_value GetCertificateCallback(GetCertificateContext *context);
    static napi_value ConnectCallback(TLSConnectContext *context);
    static napi_value GetCipherSuitesCallback(GetCipherSuitesContext *context);
    static napi_value GetRemoteCertificateCallback(GetRemoteCertificateContext *context);
    static napi_value GetProtocolCallback(GetProtocolContext *context);
    static napi_value GetSignatureAlgorithmsCallback(GetSignatureAlgorithmsContext *context);
    static napi_value SendCallback(TLSSendContext *context);
    static napi_value CloseCallback(TLSNapiContext *context);
    static napi_value BindCallback(TLSBindContext *context);
    static napi_value GetStateCallback(TLSGetStateContext *context);
    static napi_value GetRemoteAddressCallback(TLSGetRemoteAddressContext *context);
    static napi_value SetExtraOptionsCallback(TLSSetExtraOptionsContext *context);
};
} // namespace NetStack
} // namespace OHOS
#endif // TLS_TLSSOCKET_EXEC_H
