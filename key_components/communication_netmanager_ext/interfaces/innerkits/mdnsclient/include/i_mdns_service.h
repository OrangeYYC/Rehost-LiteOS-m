/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef I_MDNS_SERVICE_H
#define I_MDNS_SERVICE_H

#include <string>
#include <vector>

#include "iremote_broker.h"
#include "iremote_object.h"

#include "i_mdns_event.h"
#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {

class IMDnsService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.IMDnsService");
    enum {
        CMD_REGISTER,
        CMD_STOP_REGISTER,
        CMD_DISCOVER,
        CMD_STOP_DISCOVER,
        CMD_RESOLVE
    };

    /**
     * Register mDNS service instance
     * read https://www.rfc-editor.org/rfc/rfc2782.html and http://www.dns-sd.org/ServiceTypes.html
     *
     * @param serviceInfo.name Service instance name
     * @param serviceInfo.type Service instance type
     * @param serviceInfo.port Service instance port
     * @param cb callback object
     * @return Return errorcode
     */
    virtual int32_t RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb) = 0;
    
    /**
     * UnRegister mDNS service instance
     *
     * @param cb callback object used in RegisterService
     * @return Return errorcode
     */
    virtual int32_t UnRegisterService(const sptr<IRegistrationCallback> &cb) = 0;

    /**
     * Browse mDNS service instance by service type
     * read http://www.dns-sd.org/ServiceTypes.html
     *
     * @param serviceType Service instance type
     * @param cb callback object
     * @return Return errorcode
     */
    virtual int32_t StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb) = 0;
    
    /**
     * Stop browse mDNS service instance by service type
     *
     * @param cb callback object used in StartDiscoverService
     * @return Return errorcode
     */
    virtual int32_t StopDiscoverService(const sptr<IDiscoveryCallback> &cb) = 0;

    /**
     * Resolve browse mDNS service instance by service type and name
     *
     * @param serviceInfo.name Service instance name
     * @param serviceInfo.type Service instance type
     * @param cb callback object
     * @return Return errorcode
     */
    virtual int32_t ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_MDNS_SERVICE_H
