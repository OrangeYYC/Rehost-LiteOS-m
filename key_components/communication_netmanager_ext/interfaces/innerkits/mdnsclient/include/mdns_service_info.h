/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_MDNS_SERVICE_INFO_H
#define OHOS_MDNS_SERVICE_INFO_H

#include <string>

#include "parcel.h"

#include "mdns_common.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

struct MDnsServiceInfo : public Parcelable {
    enum {
        UNKNOWN = -1,
        IPV4 = 0,
        IPV6 = 1,
    };

    // read https://www.rfc-editor.org/rfc/rfc2782.html and https://www.rfc-editor.org/rfc/rfc6763#section-6
    std::string name;
    std::string type;
    int32_t family = UNKNOWN;
    std::string addr;
    int32_t port = -1;
    TxtRecordEncoded txtRecord;

    bool Marshalling(Parcel &parcel) const override;
    static bool Marshalling(Parcel &data, const sptr<MDnsServiceInfo> &obj);
    static sptr<MDnsServiceInfo> Unmarshalling(Parcel &parcel);

    bool IsKeyValueVaild(const ::std::string &key, const std::vector<uint8_t> &value);
    TxtRecord GetAttrMap();
    void SetAttrMap(const TxtRecord &map);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // OHOS_MDNS_SERVICE_INFO_H