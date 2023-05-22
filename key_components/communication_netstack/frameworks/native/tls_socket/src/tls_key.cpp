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

#include "tls_key.h"

#include "netstack_log.h"
#include "tls_utils.h"

namespace OHOS {
namespace NetStack {
namespace {
constexpr int FILE_READ_KEY_LEN = 4096;
constexpr const char *FILE_OPEN_FLAG = "rb";
} // namespace

TLSKey::TLSKey(const std::string &fileName, KeyAlgorithm algorithm, const SecureData &passPhrase,
               EncodingFormat encoding, KeyType type)
{
    if (encoding == DER) {
        DecodeDer(type, algorithm, fileName, passPhrase);
    } else {
        DecodePem(type, algorithm, fileName, passPhrase);
    }
}

TLSKey::TLSKey(const SecureData &data, KeyAlgorithm algorithm, const SecureData &passPhrase)
{
    if (data.Length() == 0) {
        NETSTACK_LOGD("data is empty");
    } else {
        DecodeData(data, algorithm, passPhrase);
    }
}

TLSKey::TLSKey(const TLSKey &other)
{
    *this = other;
}

TLSKey &TLSKey::operator=(const TLSKey &other)
{
    if (other.rsa_ != nullptr) {
        rsa_ = RSA_new();
        rsa_ = other.rsa_;
    }
    if (other.dsa_ != nullptr) {
        dsa_ = DSA_new();
        dsa_ = other.dsa_;
    }
    if (other.dh_ != nullptr) {
        dh_ = DH_new();
        dh_ = other.dh_;
    }
    if (other.genericKey_ != nullptr) {
        genericKey_ = EVP_PKEY_new();
        genericKey_ = other.genericKey_;
    }
    keyIsNull_ = other.keyIsNull_;
    keyType_ = other.keyType_;
    keyAlgorithm_ = other.keyAlgorithm_;
    keyPass_ = other.keyPass_;
    keyData_ = other.keyData_;
    return *this;
}

void TLSKey::DecodeData(const SecureData &data, KeyAlgorithm algorithm, const SecureData &passPhrase)
{
    if (data.Length() == 0) {
        NETSTACK_LOGE("The parameter data is empty");
        return;
    }
    keyAlgorithm_ = algorithm;
    keyData_ = data;
    keyPass_ = passPhrase;
    BIO *bio = BIO_new_mem_buf(data.Data(), -1);
    if (!bio) {
        NETSTACK_LOGE("Failed to create bio buffer");
        return;
    }
    rsa_ = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
    if (rsa_) {
        keyIsNull_ = false;
    }
}

void TLSKey::DecodeDer(KeyType type, KeyAlgorithm algorithm, const std::string &fileName, const SecureData &passPhrase)
{
    if (fileName.empty()) {
        NETSTACK_LOGI("The parameter filename is empty");
        return;
    }
    keyType_ = type;
    keyAlgorithm_ = algorithm;
    keyPass_ = passPhrase;
    std::string realPath;
    if (!CheckFilePath(fileName, realPath)) {
        NETSTACK_LOGE("file name is error");
        return;
    }

    FILE *fp = fopen(realPath.c_str(), FILE_OPEN_FLAG);
    if (!fp) {
        NETSTACK_LOGE("open file false");
        return;
    }
    char keyDer[FILE_READ_KEY_LEN] = {0};
    size_t keyLen = fread(keyDer, 1, FILE_READ_KEY_LEN, fp);
    (void)fclose(fp);
    if (!keyLen) {
        NETSTACK_LOGE("Insufficient size bytes were read");
        return;
    }

    const auto *key_data = reinterpret_cast<const unsigned char *>(keyDer);
    if (type == PUBLIC_KEY) {
        rsa_ = d2i_RSA_PUBKEY(nullptr, &key_data, static_cast<long>(keyLen));
    } else {
        rsa_ = d2i_RSAPrivateKey(nullptr, &key_data, static_cast<long>(keyLen));
    }
    if (!rsa_) {
        NETSTACK_LOGE("rsa is null");
        return;
    }
    keyIsNull_ = false;
}

void TLSKey::SwitchAlgorithm(KeyType type, KeyAlgorithm algorithm, BIO *bio)
{
    switch (algorithm) {
        case ALGORITHM_RSA:
            rsa_ = (type == PUBLIC_KEY) ? PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr)
                                        : PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
            if (rsa_) {
                keyIsNull_ = false;
            }
            break;
        case ALGORITHM_DSA:
            dsa_ = (type == PUBLIC_KEY) ? PEM_read_bio_DSA_PUBKEY(bio, nullptr, nullptr, nullptr)
                                        : PEM_read_bio_DSAPrivateKey(bio, nullptr, nullptr, nullptr);
            if (dsa_) {
                keyIsNull_ = false;
            }
            break;
        case ALGORITHM_DH: {
            EVP_PKEY *result = (type == PUBLIC_KEY) ? PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr)
                                                    : PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
            if (result) {
                dh_ = EVP_PKEY_get1_DH(result);
            }
            if (dh_) {
                keyIsNull_ = false;
            }
            EVP_PKEY_free(result);
            break;
        }
        case ALGORITHM_EC:
            ec_ = (type == PUBLIC_KEY) ? PEM_read_bio_EC_PUBKEY(bio, nullptr, nullptr, nullptr)
                                       : PEM_read_bio_ECPrivateKey(bio, nullptr, nullptr, nullptr);
            if (ec_) {
                keyIsNull_ = false;
            }
            break;
        default:
            NETSTACK_LOGE("algorithm = %{public}d is error", algorithm);
    }
}

void TLSKey::DecodePem(KeyType type, KeyAlgorithm algorithm, const std::string &fileName, const SecureData &passPhrase)
{
    if (fileName.empty()) {
        NETSTACK_LOGE("filename is empty");
        return;
    }
    keyType_ = type;
    keyAlgorithm_ = algorithm;
    std::string realPath;
    if (!CheckFilePath(fileName, realPath)) {
        NETSTACK_LOGE("file name is error");
        return;
    }

    FILE *fp = fopen(realPath.c_str(), FILE_OPEN_FLAG);
    if (!fp) {
        NETSTACK_LOGE("open file failed");
        return;
    }
    char privateKey[FILE_READ_KEY_LEN] = {0};
    if (!fread(privateKey, 1, FILE_READ_KEY_LEN, fp)) {
        NETSTACK_LOGE("read file failed");
        (void)fclose(fp);
        return;
    }
    (void)fclose(fp);
    const char *privateKeyData = static_cast<const char *>(privateKey);
    BIO *bio = BIO_new_mem_buf(privateKeyData, -1);
    if (!bio) {
        NETSTACK_LOGE("bio is null");
        return;
    }
    keyPass_ = passPhrase;
    SwitchAlgorithm(type, algorithm, bio);
    BIO_free(bio);
}

void TLSKey::Clear(bool deep)
{
    keyIsNull_ = true;
    const auto algo = Algorithm();
    if (algo == ALGORITHM_RSA && rsa_) {
        if (deep) {
            RSA_free(rsa_);
        }
        rsa_ = nullptr;
    }
    if (algo == ALGORITHM_DSA && dsa_) {
        if (deep) {
            DSA_free(dsa_);
        }
        dsa_ = nullptr;
    }
    if (algo == ALGORITHM_DH && dh_) {
        if (deep) {
            DH_free(dh_);
        }
        dh_ = nullptr;
    }
    if (algo == ALGORITHM_EC && ec_) {
        if (deep) {
            EC_KEY_free(ec_);
        }
        ec_ = nullptr;
    }
    if (algo == OPAQUE && opaque_) {
        if (deep) {
            EVP_PKEY_free(opaque_);
        }
        opaque_ = nullptr;
    }
}

KeyAlgorithm TLSKey::Algorithm() const
{
    return keyAlgorithm_;
}

Handle TLSKey::handle() const
{
    switch (keyAlgorithm_) {
        case OPAQUE:
            return Handle(OPAQUE);
        case ALGORITHM_RSA:
            return Handle(rsa_);
        case ALGORITHM_DSA:
            return Handle(dsa_);
        case ALGORITHM_DH:
            return Handle(dh_);
        case ALGORITHM_EC:
            return Handle(ec_);
        default:
            return Handle(nullptr);
    }
}

const SecureData &TLSKey::GetKeyPass() const
{
    return keyPass_;
}

const SecureData &TLSKey::GetKeyData() const
{
    return keyData_;
}
} // namespace NetStack
} // namespace OHOS
