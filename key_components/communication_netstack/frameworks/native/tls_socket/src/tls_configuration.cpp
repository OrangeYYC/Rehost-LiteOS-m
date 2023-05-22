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

#include "tls_configuration.h"

#include <openssl/x509.h>

#include "secure_data.h"
#include "tls.h"
#include "tls_key.h"

namespace OHOS {
namespace NetStack {
TLSConfiguration::TLSConfiguration(const TLSConfiguration &other)
{
    privateKey_ = other.privateKey_;
    localCertificate_ = other.localCertificate_;
    caCertificate_ = other.caCertificate_;
    minProtocol_ = other.minProtocol_;
    maxProtocol_ = other.maxProtocol_;
    cipherSuite_ = other.cipherSuite_;
}

const TLSKey &TLSConfiguration::PrivateKey() const
{
    return privateKey_;
}

TLSConfiguration &TLSConfiguration::operator=(const TLSConfiguration &other)
{
    privateKey_ = other.privateKey_;
    localCertificate_ = other.localCertificate_;
    caCertificate_ = other.caCertificate_;
    minProtocol_ = other.minProtocol_;
    maxProtocol_ = other.maxProtocol_;
    cipherSuite_ = other.cipherSuite_;
    caCertificateChain_ = other.caCertificateChain_;
    signatureAlgorithms_ = other.signatureAlgorithms_;
    privateKey_ = other.privateKey_;
    return *this;
}

void TLSConfiguration::SetLocalCertificate(const TLSCertificate &certificate)
{
    localCertificate_ = certificate;
}

void TLSConfiguration::SetCaCertificate(const TLSCertificate &certificate)
{
    caCertificate_ = certificate;
}

void TLSConfiguration::SetPrivateKey(const TLSKey &key)
{
    privateKey_ = key;
}

void TLSConfiguration::SetPrivateKey(const SecureData &key, const SecureData &keyPass)
{
    TLSKey pkey(key, ALGORITHM_RSA, keyPass);
    privateKey_ = pkey;
}

void TLSConfiguration::SetLocalCertificate(const std::string &certificate)
{
    TLSCertificate local(certificate, LOCAL_CERT);
    localCertificate_ = local;
}

void TLSConfiguration::SetCaCertificate(const std::vector<std::string> &certificate)
{
    caCertificateChain_ = certificate;
}

void TLSConfiguration::SetProtocol(const std::vector<std::string> &Protocol)
{
    bool isTls1_3 = false;
    bool isTls1_2 = false;
    for (const auto &p : Protocol) {
        if (p == PROTOCOL_TLS_V13) {
            maxProtocol_ = TLS_V1_3;
            isTls1_3 = true;
        }
        if (p == PROTOCOL_TLS_V12) {
            minProtocol_ = TLS_V1_2;
            isTls1_2 = true;
        }
    }
    if (!isTls1_3) {
        maxProtocol_ = TLS_V1_2;
    }
    if (!isTls1_2) {
        minProtocol_ = TLS_V1_3;
    }
    protocol_ = maxProtocol_;
}

TLSProtocol TLSConfiguration::GetMinProtocol() const
{
    return minProtocol_;
}

TLSProtocol TLSConfiguration::GetMaxProtocol() const
{
    return maxProtocol_;
}

TLSProtocol TLSConfiguration::GetProtocol() const
{
    return protocol_;
}

std::string TLSConfiguration::GetCipherSuite() const
{
    return cipherSuite_;
}

std::vector<CipherSuite> TLSConfiguration::GetCipherSuiteVec() const
{
    return cipherSuiteVec_;
}

const X509CertRawData &TLSConfiguration::GetCertificate() const
{
    return localCertificate_.GetLocalCertRawData();
}

void TLSConfiguration::SetCipherSuite(const std::string &cipherSuite)
{
    cipherSuite_ = cipherSuite;
}

void TLSConfiguration::SetSignatureAlgorithms(const std::string &signatureAlgorithms)
{
    signatureAlgorithms_ = signatureAlgorithms;
}

const std::string &TLSConfiguration::GetSignatureAlgorithms() const
{
    return signatureAlgorithms_;
}

void TLSConfiguration::SetUseRemoteCipherPrefer(bool useRemoteCipherPrefer)
{
    useRemoteCipherPrefer_ = useRemoteCipherPrefer;
}

bool TLSConfiguration::GetUseRemoteCipherPrefer() const
{
    return useRemoteCipherPrefer_;
}

std::vector<std::string> TLSConfiguration::GetCaCertificate() const
{
    return caCertificateChain_;
}

TLSCertificate TLSConfiguration::GetLocalCertificate() const
{
    return localCertificate_;
}

TLSKey TLSConfiguration::GetPrivateKey() const
{
    return privateKey_;
}
} // namespace NetStack
} // namespace OHOS
