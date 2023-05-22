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

#ifndef TLS_TLSSOCKET_ASYNC_WORK_H
#define TLS_TLSSOCKET_ASYNC_WORK_H

#include <napi/native_api.h>

namespace OHOS {
namespace NetStack {
class TLSSocketAsyncWork final {
public:
    TLSSocketAsyncWork() = delete;
    ~TLSSocketAsyncWork() = delete;

    static void ExecGetCertificate(napi_env env, void *data);
    static void ExecConnect(napi_env env, void *data);
    static void ExecGetCipherSuites(napi_env env, void *data);
    static void ExecGetRemoteCertificate(napi_env env, void *data);
    static void ExecGetProtocol(napi_env env, void *data);
    static void ExecGetSignatureAlgorithms(napi_env env, void *data);
    static void ExecSend(napi_env env, void *data);
    static void ExecClose(napi_env env, void *data);
    static void ExecBind(napi_env env, void *data);
    static void ExecGetState(napi_env env, void *data);
    static void ExecGetRemoteAddress(napi_env env, void *data);
    static void ExecSetExtraOptions(napi_env env, void *data);

    static void GetCertificateCallback(napi_env env, napi_status status, void *data);
    static void ConnectCallback(napi_env env, napi_status status, void *data);
    static void GetCipherSuitesCallback(napi_env env, napi_status status, void *data);
    static void GetRemoteCertificateCallback(napi_env env, napi_status status, void *data);
    static void GetProtocolCallback(napi_env env, napi_status status, void *data);
    static void GetSignatureAlgorithmsCallback(napi_env env, napi_status status, void *data);
    static void SendCallback(napi_env env, napi_status status, void *data);
    static void CloseCallback(napi_env env, napi_status status, void *data);
    static void BindCallback(napi_env env, napi_status status, void *data);
    static void GetStateCallback(napi_env env, napi_status status, void *data);
    static void GetRemoteAddressCallback(napi_env env, napi_status status, void *data);
    static void SetExtraOptionsCallback(napi_env env, napi_status status, void *data);
};
} // namespace NetStack
} // namespace OHOS
#endif // TLS_TLSSOCKET_ASYNC_WORK_H
