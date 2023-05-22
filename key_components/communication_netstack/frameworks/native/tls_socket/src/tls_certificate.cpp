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

#include "tls_certificate.h"

#include <cerrno>
#include <cstdlib>
#include <memory>
#include <map>
#include <securec.h>

#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/pem.h>

#include "netstack_log.h"
#include "tls_utils.h"

namespace OHOS {
namespace NetStack {
namespace {
constexpr const char *BIO_FILE_FLAG = "r";
constexpr const char *FILE_OPEN_FLAG = "rb";
constexpr const char *CERT_VERSION_FLAG = "V";
constexpr int FILE_READ_CERT_LEN = 4096;
} // namespace

TLSCertificate::TLSCertificate(const std::string &data, EncodingFormat format, CertType certType)
{
    rawData_.encodingFormat = PEM;
    if (data.empty()) {
        NETSTACK_LOGE("The parameter data is empty");
        return;
    }
    switch (format) {
        case PEM:
            if (!CertificateFromPem(data, certType)) {
                NETSTACK_LOGE("return CertificateFromPem(data, certs) is false");
            }
            break;
        case DER:
            if (!CertificateFromDer(data, certType)) {
                NETSTACK_LOGE("return CertificateFromDer(data, certs) is false");
            }
            break;
        default:
            NETSTACK_LOGE("%{public}u is not supported, only support PEM =0 and DER = 1", format);
    }
}

TLSCertificate::TLSCertificate(const std::string &data, CertType certType)
{
    rawData_.encodingFormat = PEM;
    if (data.empty()) {
        NETSTACK_LOGE("data is null in the TLSCertificate constructor");
        return;
    }
    if (!CertificateFromData(data, certType)) {
        NETSTACK_LOGE("return CertificatesFromPem(data, certs) is false");
    }
}

TLSCertificate::TLSCertificate(const TLSCertificate &other)
{
    *this = other;
}

TLSCertificate &TLSCertificate::operator=(const TLSCertificate &other)
{
    if (other.x509_ != nullptr) {
        x509_ = X509_new();
        x509_ = other.x509_;
    }
    version_ = other.version_;
    serialNumber_ = other.serialNumber_;
    signatureAlgorithm_ = other.signatureAlgorithm_;
    localCertString_ = other.localCertString_;
    caCertString_ = other.caCertString_;
    rawData_.data = other.rawData_.data;
    rawData_.encodingFormat = other.rawData_.encodingFormat;
    return *this;
}

bool TLSCertificate::CertificateFromData(const std::string &data, CertType certType)
{
    if (data.empty()) {
        NETSTACK_LOGE("The parameter data is empty");
        return false;
    }
    BIO *bio = BIO_new_mem_buf(data.c_str(), -1);
    if (!bio) {
        NETSTACK_LOGE("create BIO mem buf failed!");
        return false;
    }
    X509 *x509 = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }

    x509_ = X509_dup(x509);
    if (!AnalysisCertificate(certType, x509)) {
        NETSTACK_LOGE("Analysis certificate is false");
        X509_free(x509);
        return false;
    }
    X509_free(x509);
    return true;
}

bool TLSCertificate::CertificateFromPem(const std::string &data, CertType certType)
{
    BIO *bio = BIO_new_file((data.c_str()), BIO_FILE_FLAG);
    if (!bio) {
        NETSTACK_LOGE("bio new file fail errno");
        return false;
    }
    X509 *x509 = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }

    x509_ = X509_dup(x509);
    if (!AnalysisCertificate(certType, x509)) {
        NETSTACK_LOGE("Analysis certificate is false");
        X509_free(x509);
        return false;
    }
    X509_free(x509);
    return true;
}

bool TLSCertificate::CertificateFromDer(const std::string &data, CertType certType)
{
    if (data.empty()) {
        NETSTACK_LOGE("The certificate file to be converted is empty");
        return false;
    }
    std::string realPath;
    if (!CheckFilePath(data, realPath)) {
        NETSTACK_LOGE("file name is error");
        return false;
    }

    FILE *fp = fopen(realPath.c_str(), FILE_OPEN_FLAG);
    if (!fp) {
        NETSTACK_LOGE("Couldn't open file for reading, error string %{public}s", strerror(errno));
        return false;
    }
    unsigned char cert[FILE_READ_CERT_LEN] = {};
    size_t certLen = fread(cert, 1, FILE_READ_CERT_LEN, fp);
    fclose(fp);
    if (!certLen) {
        NETSTACK_LOGE("Insufficient size bytes were read");
        return false;
    }
    const auto *cert_data = reinterpret_cast<const unsigned char *>(cert);
    X509 *x509 = d2i_X509(nullptr, &cert_data, static_cast<long>(certLen));
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    x509_ = X509_dup(x509);
    if (!AnalysisCertificate(certType, x509)) {
        NETSTACK_LOGE("Analysis certificate is false");
        X509_free(x509);
        return false;
    }
    X509_free(x509);
    return true;
}

bool TLSCertificate::AnalysisCertificate(CertType certType, X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    if (certType == CA_CERT) {
        if (!CaCertToString(x509)) {
            NETSTACK_LOGE("Get CA cert as string failed");
            return false;
        }
    }
    if (certType == LOCAL_CERT) {
        if (!LocalCertToString(x509)) {
            NETSTACK_LOGE("Failed to convert certificate to string");
            return false;
        }
    }
    if (!SetLocalCertRawData(x509)) {
        NETSTACK_LOGE("Failed to set x509 certificata Serialization data");
        return false;
    }
    if (!SetX509Version(x509)) {
        NETSTACK_LOGE("Failed to set x509 version");
        return false;
    }
    if (!SetSerialNumber(x509)) {
        NETSTACK_LOGE("Failed to set serial number");
        return false;
    }
    if (!SetNotValidTime(x509)) {
        NETSTACK_LOGE("Failed to set not valid time");
        return false;
    }
    if (!SetSignatureAlgorithm(x509)) {
        NETSTACK_LOGE("Failed to set signature algorithm");
        return false;
    }
    return true;
}

bool TLSCertificate::CaCertToString(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    X509_print(bio, x509);
    char data[FILE_READ_CERT_LEN] = {};
    if (!BIO_read(bio, data, FILE_READ_CERT_LEN)) {
        NETSTACK_LOGE("Ca certificate to string BIO_read is false");
        BIO_free(bio);
        return false;
    }
    caCertString_ = std::string(data);
    BIO_free(bio);
    return true;
}

bool TLSCertificate::LocalCertToString(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        NETSTACK_LOGE("bio is null");
        return false;
    }
    X509_print(bio, x509);
    char data[FILE_READ_CERT_LEN] = {};
    if (!BIO_read(bio, data, FILE_READ_CERT_LEN)) {
        NETSTACK_LOGE("Local certificate to string BIO_read is false");
        BIO_free(bio);
        return false;
    }
    localCertString_ = std::string(data);
    BIO_free(bio);
    return true;
}

bool TLSCertificate::SetX509Version(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    auto ver = X509_get_version(x509) + 1;
    version_ = CERT_VERSION_FLAG + std::to_string(ver);
    NETSTACK_LOGI("tls version_ %{public}s", version_.c_str());
    return true;
}

bool TLSCertificate::SetSerialNumber(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    ASN1_INTEGER *serial = X509_get_serialNumber(x509);
    if (!serial) {
        NETSTACK_LOGE("Failed to get serial number");
        return false;
    }
    if (!serial->length) {
        NETSTACK_LOGE("Serial length error");
        return false;
    }
    BIGNUM *bn = ASN1_INTEGER_to_BN(serial, nullptr);
    if (!bn) {
        NETSTACK_LOGE("Unable to convert ASN1INTEGER to BN");
        return false;
    }
    serialNumber_ = BN_bn2hex(bn);
    BN_free(bn);
    return true;
}

bool TLSCertificate::SetNotValidTime(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    ASN1_TIME *before = X509_get_notBefore(x509);
    if (!before) {
        NETSTACK_LOGE("before is null");
        return false;
    }
    tm tmBefore = {0};
    if (!ASN1_TIME_to_tm(before, &tmBefore)) {
        NETSTACK_LOGE("ASN1_TIME_to_tm before is false");
        return false;
    }

    ASN1_TIME *after = X509_get_notAfter(x509);
    if (!after) {
        NETSTACK_LOGE("after is null");
        return false;
    }
    tm tmAfter;
    if (!ASN1_TIME_to_tm(after, &tmAfter)) {
        NETSTACK_LOGE("ASN1_TIME_to_tm before is false");
        return false;
    }
    return true;
}

bool TLSCertificate::SetSignatureAlgorithm(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    int signNid = X509_get_signature_nid(x509);
    const char *sign = OBJ_nid2sn(signNid);
    if (!sign) {
        NETSTACK_LOGE("OBJ_nid2sn is null");
        return false;
    }
    signatureAlgorithm_ = sign;
    return true;
}

bool TLSCertificate::SetLocalCertRawData(X509 *x509)
{
    if (!x509) {
        NETSTACK_LOGE("x509 is null");
        return false;
    }
    int32_t length = i2d_X509(x509, nullptr);
    if (length <= 0) {
        NETSTACK_LOGE("Failed to convert x509 to der format, length is %{public}d", length);
        return false;
    }
    unsigned char *der = nullptr;
    (void)i2d_X509(x509, &der);
    SecureData data(der, length);
    rawData_.data = data;
    OPENSSL_free(der);
    rawData_.encodingFormat = DER;
    return true;
}

std::string TLSCertificate::GetSignatureAlgorithm() const
{
    return signatureAlgorithm_;
}
std::string TLSCertificate::GetLocalCertString() const
{
    return localCertString_;
}

Handle TLSCertificate::handle() const
{
    return Handle(x509_);
}

const X509CertRawData &TLSCertificate::GetLocalCertRawData() const
{
    return rawData_;
}
} // namespace NetStack
} // namespace OHOS
