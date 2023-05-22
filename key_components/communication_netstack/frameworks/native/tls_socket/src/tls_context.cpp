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

#include "tls_context.h"

#include <cerrno>
#include <cinttypes>
#include <string>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "netstack_log.h"

namespace OHOS {
namespace NetStack {
VerifyMode TLSContext::verifyMode_ = TWO_WAY_MODE;
std::unique_ptr<TLSContext> TLSContext::CreateConfiguration(const TLSConfiguration &configuration)
{
    auto tlsContext = std::make_unique<TLSContext>();
    if (!InitTlsContext(tlsContext.get(), configuration)) {
        NETSTACK_LOGE("Failed to init tls context");
        return nullptr;
    }
    return tlsContext;
}

void InitEnv()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}

bool TLSContext::SetCipherList(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return false;
    }
    NETSTACK_LOGD("GetCipherSuite = %{public}s", configuration.GetCipherSuite().c_str());
    if (SSL_CTX_set_cipher_list(tlsContext->ctx_, configuration.GetCipherSuite().c_str()) <= 0) {
        NETSTACK_LOGE("Error setting the cipher list");
        return false;
    }
    return true;
}

void TLSContext::GetCiphers(TLSContext *tlsContext)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return;
    }
    std::vector<CipherSuite> cipherSuiteVec;
    STACK_OF(SSL_CIPHER) *sk = SSL_CTX_get_ciphers(tlsContext->ctx_);
    if (!sk) {
        NETSTACK_LOGE("sk is null");
        return;
    }
    CipherSuite cipherSuite;
    for (int i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(sk, i);
        cipherSuite.cipherId_ = SSL_CIPHER_get_id(cipher);
        cipherSuite.cipherName_ = SSL_CIPHER_get_name(cipher);
        cipherSuiteVec.push_back(cipherSuite);
    }
}

bool TLSContext::SetSignatureAlgorithms(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return false;
    }
    if (configuration.GetSignatureAlgorithms().empty()) {
        NETSTACK_LOGE("configuration get signature algorithms is empty");
        return false;
    }

    if (!SSL_CTX_set1_sigalgs_list(tlsContext->ctx_, configuration.GetSignatureAlgorithms().c_str())) {
        NETSTACK_LOGE("Error setting the Signature Algorithms");
        return false;
    }
    return true;
}

void TLSContext::UseRemoteCipher(TLSContext *tlsContext)
{
    if (!tlsContext) {
        NETSTACK_LOGE("TLSContext::UseRemoteCipher: tlsContext is null");
        return;
    }
    if (tlsContext->tlsConfiguration_.GetUseRemoteCipherPrefer()) {
        SSL_CTX_set_options(tlsContext->ctx_, SSL_OP_CIPHER_SERVER_PREFERENCE);
    }
    NETSTACK_LOGI("SSL_CTX_get_options = %{public}" PRIx64,
        static_cast<uint64_t>(SSL_CTX_get_options(tlsContext->ctx_)));
}

void TLSContext::SetMinAndMaxProtocol(TLSContext *tlsContext)
{
    if (!tlsContext) {
        NETSTACK_LOGE("TLSContext::SetMinAndMaxProtocol: tlsContext is null");
        return;
    }
    const long anyVersion = TLS_ANY_VERSION;
    long minVersion = anyVersion;
    long maxVersion = anyVersion;

    switch (tlsContext->tlsConfiguration_.GetMinProtocol()) {
        case TLS_V1_2:
            minVersion = TLS1_2_VERSION;
            break;
        case TLS_V1_3:
            minVersion = TLS1_3_VERSION;
            break;
        case UNKNOW_PROTOCOL:
            break;
        default:
            break;
    }

    switch (tlsContext->tlsConfiguration_.GetMaxProtocol()) {
        case TLS_V1_2:
            maxVersion = TLS1_2_VERSION;
            break;
        case TLS_V1_3:
            maxVersion = TLS1_3_VERSION;
            break;
        case UNKNOW_PROTOCOL:
            break;
        default:
            break;
    }

    if (minVersion != anyVersion && !SSL_CTX_set_min_proto_version(tlsContext->ctx_, minVersion)) {
        NETSTACK_LOGE("Error while setting the minimal protocol version");
        return;
    }

    if (maxVersion != anyVersion && !SSL_CTX_set_max_proto_version(tlsContext->ctx_, maxVersion)) {
        NETSTACK_LOGE("Error while setting the maximum protocol version");
        return;
    }

    NETSTACK_LOGD("minProtocol = %{public}lx, maxProtocol = %{public}lx",
                  SSL_CTX_get_min_proto_version(tlsContext->ctx_), SSL_CTX_get_max_proto_version(tlsContext->ctx_));
}

bool TLSContext::SetCaAndVerify(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return false;
    }
    for (const auto &cert : configuration.GetCaCertificate()) {
        TLSCertificate ca(cert, CA_CERT);
        if (!X509_STORE_add_cert(SSL_CTX_get_cert_store(tlsContext->ctx_), static_cast<X509 *>(ca.handle()))) {
            NETSTACK_LOGE("Failed to add x509 cert");
            return false;
        }
    }
    return true;
}

bool TLSContext::SetLocalCertificate(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return false;
    }
    if (!SSL_CTX_use_certificate(tlsContext->ctx_, static_cast<X509 *>(configuration.GetLocalCertificate().handle()))) {
        NETSTACK_LOGD("The local certificate is unavailable");
        return false;
    }
    return true;
}

bool TLSContext::SetKeyAndCheck(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("The parameter tlsContext is null");
        return false;
    }
    if (configuration.GetPrivateKey().Algorithm() == OPAQUE) {
        tlsContext->pkey_ = reinterpret_cast<EVP_PKEY *>(configuration.GetPrivateKey().handle());
    } else {
        tlsContext->pkey_ = EVP_PKEY_new();
        if (configuration.GetPrivateKey().Algorithm() == ALGORITHM_RSA) {
            EVP_PKEY_set1_RSA(tlsContext->pkey_, reinterpret_cast<RSA *>(configuration.GetPrivateKey().handle()));
        } else if (tlsContext->tlsConfiguration_.GetPrivateKey().Algorithm() == ALGORITHM_DSA) {
            EVP_PKEY_set1_DSA(tlsContext->pkey_, reinterpret_cast<DSA *>(configuration.GetPrivateKey().handle()));
        }
    }

    if (configuration.GetPrivateKey().Algorithm() == OPAQUE) {
        tlsContext->pkey_ = nullptr;
    }
    auto pkey_ = tlsContext->pkey_;
    if (!SSL_CTX_use_PrivateKey(tlsContext->ctx_, pkey_)) {
        NETSTACK_LOGE("SSL_CTX_use_PrivateKey is error");
        return false;
    }

    if (!configuration.GetPrivateKey().GetKeyPass().Length()) {
        SSL_CTX_set_default_passwd_cb_userdata(tlsContext->ctx_,
                                               reinterpret_cast<void *>(const_cast<char *>(
                                               tlsContext->tlsConfiguration_.GetPrivateKey().GetKeyPass().Data())));
    }
    // Check if the certificate matches the private key.
    if (!SSL_CTX_check_private_key(tlsContext->ctx_)) {
        NETSTACK_LOGE("Check if the certificate matches the private key is error");
        return false;
    }
    return true;
}

void TLSContext::SetVerify(TLSContext *tlsContext)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return;
    }

    if (tlsContext->tlsConfiguration_.GetCaCertificate().empty() ||
        !tlsContext->tlsConfiguration_.GetCertificate().data.Length() ||
        !tlsContext->tlsConfiguration_.GetPrivateKey().GetKeyData().Length()) {
        verifyMode_ = ONE_WAY_MODE;
        SSL_CTX_set_verify(tlsContext->ctx_, SSL_VERIFY_PEER, nullptr);
    } else {
        verifyMode_ = TWO_WAY_MODE;
        SSL_CTX_set_verify(tlsContext->ctx_, SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
    }
    NETSTACK_LOGD("Authentication mode is %{public}s", verifyMode_ ? "two-way authentication" : "one-way authentication");
}

bool TLSContext::InitTlsContext(TLSContext *tlsContext, const TLSConfiguration &configuration)
{
    if (!tlsContext) {
        NETSTACK_LOGE("tlsContext is null");
        return false;
    }
    InitEnv();
    tlsContext->tlsConfiguration_ = configuration;
    tlsContext->ctx_ = SSL_CTX_new(TLS_client_method());
    if (tlsContext->ctx_ == nullptr) {
        NETSTACK_LOGE("ctx is nullptr");
        return false;
    }
    if (!configuration.GetCipherSuite().empty()) {
        if (!SetCipherList(tlsContext, configuration)) {
            NETSTACK_LOGE("Failed to set cipher suite");
            return false;
        }
    }
    if (!configuration.GetSignatureAlgorithms().empty()) {
        if (!SetSignatureAlgorithms(tlsContext, configuration)) {
            NETSTACK_LOGE("Failed to set signature algorithms");
            return false;
        }
    }
    GetCiphers(tlsContext);
    UseRemoteCipher(tlsContext);
    SetMinAndMaxProtocol(tlsContext);
    SetVerify(tlsContext);
    if (!SetCaAndVerify(tlsContext, configuration)) {
        return false;
    }
    if (!verifyMode_) {
        NETSTACK_LOGD("one way authentication");
        return true;
    }
    if (!SetLocalCertificate(tlsContext, configuration)) {
        return false;
    }
    if (!SetKeyAndCheck(tlsContext, configuration)) {
        return false;
    }
    return true;
}
SSL *TLSContext::CreateSsl()
{
    ctxSsl_ = SSL_new(ctx_);
    return ctxSsl_;
}

void TLSContext::CloseCtx()
{
    SSL_CTX_free(ctx_);
}
} // namespace NetStack
} // namespace OHOS
