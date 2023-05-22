/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NET_SUPPLIER_CALLBACK_STUB_H
#define NET_SUPPLIER_CALLBACK_STUB_H

#include <map>
#include <set>

#include "iremote_stub.h"

#include "i_net_supplier_callback.h"
#include "net_supplier_callback_base.h"
#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
class NetSupplierCallbackStub : public IRemoteStub<INetSupplierCallback> {
public:
    NetSupplierCallbackStub();
    virtual ~NetSupplierCallbackStub();

    void RegisterSupplierCallbackImpl(const sptr<NetSupplierCallbackBase> &callback);

    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    int32_t RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps) override;
    int32_t ReleaseNetwork(const std::string &ident, const std::set<NetCap> &netCaps) override;

private:
    using NetSupplierCallbackFunc = int32_t (NetSupplierCallbackStub::*)(MessageParcel &, MessageParcel &);

private:
    int32_t OnRequestNetwork(MessageParcel &data, MessageParcel &reply);
    int32_t OnReleaseNetwork(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NetSupplierCallbackFunc> memberFuncMap_;
    sptr<NetSupplierCallbackBase> callback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_SUPPLIER_CALLBACK_STUB_H
