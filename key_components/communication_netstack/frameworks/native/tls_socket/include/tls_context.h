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

#ifndef COMMUNICATION_NETSTACK_TLS_CONTEXT_H
#define COMMUNICATION_NETSTACK_TLS_CONTEXT_H

#include <memory>

#include "tls.h"
#include "tls_configuration.h"

namespace OHOS {
namespace NetStack {
class TLSContext {
public:
    TLSContext() = default;
    ~TLSContext() = default;
    static std::unique_ptr<TLSContext> CreateConfiguration(const TLSConfiguration &configuration);
    SSL *CreateSsl();
    void CloseCtx();

private:
    static bool InitTlsContext(TLSContext *sslContext, const TLSConfiguration &configuration);
    static bool SetCipherList(TLSContext *tlsContext, const TLSConfiguration &configuration);
    static bool SetSignatureAlgorithms(TLSContext *tlsContext, const TLSConfiguration &configuration);
    static void GetCiphers(TLSContext *tlsContext);
    static void UseRemoteCipher(TLSContext *tlsContext);
    static void SetMinAndMaxProtocol(TLSContext *tlsContext);
    static bool SetCaAndVerify(TLSContext *tlsContext, const TLSConfiguration &configuration);
    static bool SetLocalCertificate(TLSContext *tlsContext, const TLSConfiguration &configuration);
    static bool SetKeyAndCheck(TLSContext *tlsContext, const TLSConfiguration &configuration);
    static void SetVerify(TLSContext *tlsContext);

private:
    SSL_CTX *ctx_ = nullptr;
    EVP_PKEY *pkey_ = nullptr;
    SSL *ctxSsl_ = nullptr;
    TLSConfiguration tlsConfiguration_;
    static VerifyMode verifyMode_;
};
} // namespace NetStack
} // namespace OHOS
#endif // COMMUNICATION_NETSTACK_TLS_CONTEXT_H
