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

#ifndef COMMUNICATIONNETSTACK_TLS_SOCEKT_H
#define COMMUNICATIONNETSTACK_TLS_SOCEKT_H

#include <any>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <map>
#include <thread>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "extra_options_base.h"
#include "net_address.h"
#include "socket_error.h"
#include "socket_remote_info.h"
#include "socket_state_base.h"
#include "tcp_connect_options.h"
#include "tcp_extra_options.h"
#include "tcp_send_options.h"
#include "tls.h"
#include "tls_certificate.h"
#include "tls_configuration.h"
#include "tls_context.h"
#include "tls_key.h"

namespace OHOS {
namespace NetStack {

using BindCallback = std::function<void(int32_t errorNumber)>;
using ConnectCallback = std::function<void(int32_t errorNumber)>;
using SendCallback = std::function<void(int32_t errorNumber)>;
using CloseCallback = std::function<void(int32_t errorNumber)>;
using GetRemoteAddressCallback = std::function<void(int32_t errorNumber, const NetAddress &address)>;
using GetStateCallback = std::function<void(int32_t errorNumber, const SocketStateBase &state)>;
using SetExtraOptionsCallback = std::function<void(int32_t errorNumber)>;
using GetCertificateCallback = std::function<void(int32_t errorNumber, const X509CertRawData &cert)>;
using GetRemoteCertificateCallback = std::function<void(int32_t errorNumber, const X509CertRawData &cert)>;
using GetProtocolCallback = std::function<void(int32_t errorNumber, const std::string &protocol)>;
using GetCipherSuiteCallback = std::function<void(int32_t errorNumber, const std::vector<std::string> &suite)>;
using GetSignatureAlgorithmsCallback =
    std::function<void(int32_t errorNumber, const std::vector<std::string> &algorithms)>;

using OnMessageCallback = std::function<void(const std::string &data, const SocketRemoteInfo &remoteInfo)>;
using OnConnectCallback = std::function<void(void)>;
using OnCloseCallback = std::function<void(void)>;
using OnErrorCallback = std::function<void(int32_t errorNumber, const std::string &errorString)>;

using CheckServerIdentity =
    std::function<void(const std::string &hostName, const std::vector<std::string> &x509Certificates)>;

constexpr const char *ALPN_PROTOCOLS_HTTP_1_1 = "http1.1";
constexpr const char *ALPN_PROTOCOLS_HTTP_2 = "h2";

constexpr size_t MAX_ERR_LEN = 1024;

/**
 * Parameters required during communication
 */
class TLSSecureOptions {
public:
    TLSSecureOptions() = default;
    ~TLSSecureOptions() = default;

    TLSSecureOptions(const TLSSecureOptions &tlsSecureOptions);
    TLSSecureOptions &operator=(const TLSSecureOptions &tlsSecureOptions);
    /**
     * Set root CA Chain to verify the server cert
     * @param caChain root certificate chain used to validate server certificates
     */
    void SetCaChain(const std::vector<std::string> &caChain);

    /**
     * Set digital certificate for server verification
     * @param cert digital certificate sent to the server to verify validity
     */
    void SetCert(const std::string &cert);

    /**
     * Set key to decrypt server data
     * @param keyChain key used to decrypt server data
     */
    void SetKey(const SecureData &key);

    /**
     * Set the password to read the private key
     * @param keyPass read the password of the private key
     */
    void SetKeyPass(const SecureData &keyPass);

    /**
     * Set the protocol used in communication
     * @param protocolChain protocol version number used
     */
    void SetProtocolChain(const std::vector<std::string> &protocolChain);

    /**
     * Whether the peer cipher suite is preferred for communication
     * @param useRemoteCipherPrefer whether the peer cipher suite is preferred
     */
    void SetUseRemoteCipherPrefer(bool useRemoteCipherPrefer);

    /**
     * Encryption algorithm used in communication
     * @param signatureAlgorithms encryption algorithm e.g: rsa
     */
    void SetSignatureAlgorithms(const std::string &signatureAlgorithms);

    /**
     * Crypto suite used in communication
     * @param cipherSuite cipher suite e.g:AES256-SHA256
     */
    void SetCipherSuite(const std::string &cipherSuite);

    /**
     * Set a revoked certificate
     * @param crlChain certificate Revocation List
     */
    void SetCrlChain(const std::vector<std::string> &crlChain);

    /**
     * Get root CA Chain to verify the server cert
     * @return root CA chain
     */
    [[nodiscard]] const std::vector<std::string> &GetCaChain() const;

    /**
     * Obtain a certificate to send to the server for checking
     * @return digital certificate obtained
     */
    [[nodiscard]] const std::string &GetCert() const;

    /**
     * Obtain the private key in the communication process
     * @return private key during communication
     */
    [[nodiscard]] const SecureData &GetKey() const;

    /**
     * Get the password to read the private key
     * @return read the password of the private key
     */
    [[nodiscard]] const SecureData &GetKeyPass() const;

    /**
     * Get the protocol of the communication process
     * @return protocol of communication process
     */
    [[nodiscard]] const std::vector<std::string> &GetProtocolChain() const;

    /**
     * Is the remote cipher suite being used for communication
     * @return is use Remote Cipher Prefer
     */
    [[nodiscard]] bool UseRemoteCipherPrefer() const;

    /**
     * Obtain the encryption algorithm used in the communication process
     * @return encryption algorithm used in communication
     */
    [[nodiscard]] const std::string &GetSignatureAlgorithms() const;

    /**
     * Obtain the cipher suite used in communication
     * @return crypto suite used in communication
     */
    [[nodiscard]] const std::string &GetCipherSuite() const;

    /**
     * Get revoked certificate chain
     * @return revoked certificate chain
     */
    [[nodiscard]] const std::vector<std::string> &GetCrlChain() const;

private:
    std::vector<std::string> caChain_;
    std::string cert_;
    SecureData key_;
    SecureData keyPass_;
    std::vector<std::string> protocolChain_;
    bool useRemoteCipherPrefer_ = false;
    std::string signatureAlgorithms_;
    std::string cipherSuite_;
    std::vector<std::string> crlChain_;
};

/**
 * Some options required during tls connection
 */
class TLSConnectOptions {
public:
    /**
     * Communication parameters required for connection establishment
     * @param address communication parameters during connection
     */
    void SetNetAddress(const NetAddress &address);

    /**
     * Parameters required during communication
     * @param tlsSecureOptions certificate and other relevant parameters
     */
    void SetTlsSecureOptions(TLSSecureOptions &tlsSecureOptions);

    /**
     * Set the callback function to check the validity of the server
     * @param checkServerIdentity callback function passed in by API caller
     */
    void SetCheckServerIdentity(const CheckServerIdentity &checkServerIdentity);

    /**
     * Set application layer protocol negotiation
     * @param alpnProtocols application layer protocol negotiation
     */
    void SetAlpnProtocols(const std::vector<std::string> &alpnProtocols);

    /**
     * Obtain the network address of the communication process
     * @return network address
     */
    [[nodiscard]] NetAddress GetNetAddress() const;

    /**
     * Obtain the parameters required in the communication process
     * @return certificate and other relevant parameters
     */
    [[nodiscard]] TLSSecureOptions GetTlsSecureOptions() const;

    /**
     * Get the check server ID callback function passed in by the API caller
     * @return check the server identity callback function
     */
    [[nodiscard]] CheckServerIdentity GetCheckServerIdentity() const;

    /**
     * Obtain the application layer protocol negotiation in the communication process
     * @return application layer protocol negotiation
     */
    [[nodiscard]] const std::vector<std::string> &GetAlpnProtocols() const;

private:
    NetAddress address_;
    TLSSecureOptions tlsSecureOptions_;
    CheckServerIdentity checkServerIdentity_;
    std::vector<std::string> alpnProtocols_;
};

/**
 * TLS socket interface class
 */
class TLSSocket {
public:
    TLSSocket(const TLSSocket &) = delete;
    TLSSocket(TLSSocket &&) = delete;

    TLSSocket &operator=(const TLSSocket &) = delete;
    TLSSocket &operator=(TLSSocket &&) = delete;

    TLSSocket() = default;
    ~TLSSocket() = default;

    /**
     * Create a socket and bind to the address specified by address
     * @param address ip address
     * @param callback callback to the caller if bind ok or not
     */
    void Bind(const NetAddress &address, const BindCallback &callback);

    /**
     * Establish a secure connection based on the created socket
     * @param tlsConnectOptions some options required during tls connection
     * @param callback callback to the caller if connect ok or not
     */
    void Connect(TLSConnectOptions &tlsConnectOptions, const ConnectCallback &callback);

    /**
     * Send data based on the created socket
     * @param tcpSendOptions  some options required during tcp data transmission
     * @param callback callback to the caller if send ok or not
     */
    void Send(const TCPSendOptions &tcpSendOptions, const SendCallback &callback);

    /**
     * Disconnect by releasing the socket when communicating
     * @param callback callback to the caller
     */
    void Close(const CloseCallback &callback);

    /**
     * Get the peer network address
     * @param callback callback to the caller
     */
    void GetRemoteAddress(const GetRemoteAddressCallback &callback);

    /**
     * Get the status of the current socket
     * @param callback callback to the caller
     */
    void GetState(const GetStateCallback &callback);

    /**
     * Gets or sets the options associated with the current socket
     * @param tcpExtraOptions options associated with the current socket
     * @param callback callback to the caller
     */
    void SetExtraOptions(const TCPExtraOptions &tcpExtraOptions, const SetExtraOptionsCallback &callback);

    /**
     *  Get a local digital certificate
     * @param callback callback to the caller
     */
    void GetCertificate(const GetCertificateCallback &callback);

    /**
     * Get the peer digital certificate
     * @param needChain need chain
     * @param callback callback to the caller
     */
    void GetRemoteCertificate(const GetRemoteCertificateCallback &callback);

    /**
     * Obtain the protocol used in communication
     * @param callback callback to the caller
     */
    void GetProtocol(const GetProtocolCallback &callback);

    /**
     * Obtain the cipher suite used in communication
     * @param callback callback to the caller
     */
    void GetCipherSuite(const GetCipherSuiteCallback &callback);

    /**
     * Obtain the encryption algorithm used in the communication process
     * @param callback callback to the caller
     */
    void GetSignatureAlgorithms(const GetSignatureAlgorithmsCallback &callback);

    /**
     * Register a callback which is called when message is received
     * @param onMessageCallback callback which is called when message is received
     */
    void OnMessage(const OnMessageCallback &onMessageCallback);

    /**
     * Register the callback that is called when the connection is established
     * @param onConnectCallback callback invoked when connection is established
     */
    void OnConnect(const OnConnectCallback &onConnectCallback);

    /**
     * Register the callback that is called when the connection is disconnected
     * @param onCloseCallback callback invoked when disconnected
     */
    void OnClose(const OnCloseCallback &onCloseCallback);

    /**
     * Register the callback that is called when an error occurs
     * @param onErrorCallback callback invoked when an error occurs
     */
    void OnError(const OnErrorCallback &onErrorCallback);

    /**
     * Unregister the callback which is called when message is received
     */
    void OffMessage();

    /**
     * Off Connect
     */
    void OffConnect();

    /**
     * Off Close
     */
    void OffClose();

    /**
     * Off Error
     */
    void OffError();

private:
    class TLSSocketInternal final {
    public:
        TLSSocketInternal() = default;
        ~TLSSocketInternal() = default;

        /**
         * Establish an encrypted connection on the specified socket
         * @param sock socket for establishing encrypted connection
         * @param options some options required during tls connection
         * @return whether the encrypted connection is successfully established
         */
        bool TlsConnectToHost(int sock, const TLSConnectOptions &options);

        /**
         * Set the configuration items for establishing encrypted connections
         * @param config configuration item when establishing encrypted connection
         */
        void SetTlsConfiguration(const TLSConnectOptions &config);

        /**
         * Send data through an established encrypted connection
         * @param data data sent over an established encrypted connection
         * @return whether the data is successfully sent to the server
         */
        bool Send(const std::string &data);

        /**
         * Receive the data sent by the server through the established encrypted connection
         * @param buffer receive the data sent by the server
         * @param maxBufferSize the size of the data received from the server
         * @return whether the data sent by the server is successfully received
         */
        int Recv(char *buffer, int maxBufferSize);

        /**
         * Disconnect encrypted connection
         * @return whether the encrypted connection was successfully disconnected
         */
        bool Close();

        /**
         * Set the application layer negotiation protocol in the encrypted communication process
         * @param alpnProtocols application layer negotiation protocol
         * @return set whether the application layer negotiation protocol is successful during encrypted communication
         */
        bool SetAlpnProtocols(const std::vector<std::string> &alpnProtocols);

        /**
         * Storage of server communication related network information
         * @param remoteInfo communication related network information
         */
        void MakeRemoteInfo(SocketRemoteInfo &remoteInfo);

        /**
         * Get configuration options for encrypted communication process
         * @return configuration options for encrypted communication processes
         */
        [[nodiscard]] TLSConfiguration GetTlsConfiguration() const;

        /**
         * Obtain the cipher suite during encrypted communication
         * @return crypto suite used in encrypted communication
         */
        [[nodiscard]] std::vector<std::string> GetCipherSuite() const;

        /**
         * Obtain the peer certificate used in encrypted communication
         * @return peer certificate used in encrypted communication
         */
        [[nodiscard]] std::string GetRemoteCertificate() const;

        /**
         * Obtain the peer certificate used in encrypted communication
         * @return peer certificate serialization data used in encrypted communication
         */
        [[nodiscard]] const X509CertRawData &GetRemoteCertRawData() const;

        /**
         * Obtain the certificate used in encrypted communication
         * @return certificate serialization data used in encrypted communication
         */
        [[nodiscard]] const X509CertRawData &GetCertificate() const;

        /**
         * Get the encryption algorithm used in encrypted communication
         * @return encryption algorithm used in encrypted communication
         */
        [[nodiscard]] std::vector<std::string> GetSignatureAlgorithms() const;

        /**
         * Obtain the communication protocol used in encrypted communication
         * @return communication protocol used in encrypted communication
         */
        [[nodiscard]] std::string GetProtocol() const;

        /**
         * Set the information about the shared signature algorithm supported by peers during encrypted communication
         * @return information about peer supported shared signature algorithms
         */
        [[nodiscard]] bool SetSharedSigals();

        /**
         * Obtain the ssl used in encrypted communication
         * @return SSL used in encrypted communication
         */
        [[nodiscard]] ssl_st *GetSSL() const;

    private:
        bool StartTlsConnected(const TLSConnectOptions &options);
        bool CreatTlsContext();
        bool StartShakingHands(const TLSConnectOptions &options);
        bool GetRemoteCertificateFromPeer();
        bool SetRemoteCertRawData();
        std::string CheckServerIdentityLegal(const std::string &hostName, const X509 *x509Certificates);

    private:
        ssl_st *ssl_ = nullptr;
        X509 *peerX509_ = nullptr;
        uint16_t port_ = 0;
        sa_family_t family_ = 0;
        int32_t socketDescriptor_ = 0;

        TLSContext tlsContext_;
        TLSConfiguration configuration_;
        NetAddress address_;
        X509CertRawData remoteRawData_;

        std::string hostName_;
        std::string remoteCert_;
        std::string keyPass_;

        std::vector<std::string> signatureAlgorithms_;
        std::unique_ptr<TLSContext> tlsContextPointer_ = nullptr;
    };

private:
    TLSSocketInternal tlsSocketInternal_;

    static std::string MakeAddressString(sockaddr *addr);

    static void GetAddr(const NetAddress &address, sockaddr_in *addr4, sockaddr_in6 *addr6, sockaddr **addr,
                        socklen_t *len);

    void CallOnMessageCallback(const std::string &data, const SocketRemoteInfo &remoteInfo);
    void CallOnConnectCallback();
    void CallOnCloseCallback();
    void CallOnErrorCallback(int32_t err, const std::string &errString);

    void CallBindCallback(int32_t err, BindCallback callback);
    void CallConnectCallback(int32_t err, ConnectCallback callback);
    void CallSendCallback(int32_t err, SendCallback callback);
    void CallCloseCallback(int32_t err, CloseCallback callback);
    void CallGetRemoteAddressCallback(int32_t err, const NetAddress &address, GetRemoteAddressCallback callback);
    void CallGetStateCallback(int32_t err, const SocketStateBase &state, GetStateCallback callback);
    void CallSetExtraOptionsCallback(int32_t err, SetExtraOptionsCallback callback);
    void CallGetCertificateCallback(int32_t err, const X509CertRawData &cert, GetCertificateCallback callback);
    void CallGetRemoteCertificateCallback(int32_t err, const X509CertRawData &cert,
                                          GetRemoteCertificateCallback callback);
    void CallGetProtocolCallback(int32_t err, const std::string &protocol, GetProtocolCallback callback);
    void CallGetCipherSuiteCallback(int32_t err, const std::vector<std::string> &suite,
                                    GetCipherSuiteCallback callback);
    void CallGetSignatureAlgorithmsCallback(int32_t err, const std::vector<std::string> &algorithms,
                                            GetSignatureAlgorithmsCallback callback);

    void StartReadMessage();

    void GetIp4RemoteAddress(const GetRemoteAddressCallback &callback);
    void GetIp6RemoteAddress(const GetRemoteAddressCallback &callback);

    [[nodiscard]] bool SetBaseOptions(const ExtraOptionsBase &option) const;
    [[nodiscard]] bool SetExtraOptions(const TCPExtraOptions &option) const;

    void MakeIpSocket(sa_family_t family);

private:
    static constexpr const size_t MAX_ERROR_LEN = 128;
    static constexpr const size_t MAX_BUFFER_SIZE = 8192;

    OnMessageCallback onMessageCallback_;
    OnConnectCallback onConnectCallback_;
    OnCloseCallback onCloseCallback_;
    OnErrorCallback onErrorCallback_;

    std::mutex mutex_;
    bool isRunning_ = false;
    bool isRunOver_ = true;

    int sockFd_ = -1;
};
} // namespace NetStack
} // namespace OHOS

#endif // COMMUNICATIONNETSTACK_TLS_SOCEKT_H
