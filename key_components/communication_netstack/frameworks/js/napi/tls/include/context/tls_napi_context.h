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

#ifndef TLS_NAPI_CONTEXT_H
#define TLS_NAPI_CONTEXT_H

#include <cstddef>
#include <string>
#include <vector>

#include <napi/native_api.h>

#include "base_context.h"
#include "event_manager.h"
#include "net_address.h"
#include "socket_state_base.h"
#include "tls.h"

namespace OHOS {
namespace NetStack {
class TLSNapiContext final : public BaseContext {
public:
    TLSNapiContext() = delete;
    explicit TLSNapiContext(napi_env env, EventManager *manager);

public:
    std::string protocol_;
    std::vector<std::string> cipherSuites_;
    X509CertRawData localCert_;
    X509CertRawData remoteCert_;
    std::vector<std::string> signatureAlgorithms_;
    int32_t errorNumber_ = 0;
    SocketStateBase state_;
    NetAddress address_;

public:
    void ParseParams(napi_value *params, size_t paramsCount);

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};

using GetCipherSuitesContext = TLSNapiContext;
using GetProtocolContext = TLSNapiContext;
using GetSignatureAlgorithmsContext = TLSNapiContext;
using GetRemoteCertificateContext = TLSNapiContext;
using GetCertificateContext = TLSNapiContext;
using TLSGetStateContext = TLSNapiContext;
using TLSGetRemoteAddressContext = TLSNapiContext;
} // namespace NetStack
} // namespace OHOS
#endif // TLS_NAPI_CONTEXT_H
