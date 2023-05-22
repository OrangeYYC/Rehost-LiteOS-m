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

#ifndef COMMUNICATION_NETSTACK_TLS_CERTIFICATE_H
#define COMMUNICATION_NETSTACK_TLS_CERTIFICATE_H

#include <list>
#include <memory>
#include <string>

#include <openssl/x509.h>

#include "tls.h"

namespace OHOS {
namespace NetStack {
class TLSCertificate {
public:
    TLSCertificate() = default;
    TLSCertificate(const std::string &data, EncodingFormat format = PEM, CertType certType = CA_CERT);
    TLSCertificate(const std::string &data, CertType certType);
    ~TLSCertificate() = default;

    TLSCertificate(const TLSCertificate &other);
    TLSCertificate &operator=(const TLSCertificate &other);

    bool CertificateFromData(const std::string &data, CertType certType);
    bool CertificateFromPem(const std::string &data, CertType certType);
    bool CertificateFromDer(const std::string &data, CertType certType);
    bool CaCertToString(X509 *x509);
    bool LocalCertToString(X509 *x509);
    std::string GetLocalCertString() const;
    std::string GetSignatureAlgorithm() const;
    const X509CertRawData &GetLocalCertRawData() const;

    Handle handle() const;

private:
    bool SetSerialNumber(X509 *x509);
    bool SetX509Version(X509 *x509);
    bool SetNotValidTime(X509 *x509);
    bool SetSignatureAlgorithm(X509 *x509);
    bool AnalysisCertificate(CertType certType, X509 *x509);
    bool SetLocalCertRawData(X509 *x509);

private:
    X509 *x509_ = nullptr;
    std::string version_;
    std::string serialNumber_;
    std::string signatureAlgorithm_;
    std::string localCertString_;
    std::string caCertString_;
    X509CertRawData rawData_;
};
} // namespace NetStack
} // namespace OHOS
#endif // COMMUNICATION_NETSTACK_TLS_CERTIFICATE_H
