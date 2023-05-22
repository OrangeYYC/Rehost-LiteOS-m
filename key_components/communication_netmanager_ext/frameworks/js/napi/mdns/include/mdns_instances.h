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

#ifndef NETMANAGER_EXT_MDNS_INSTANCES_H
#define NETMANAGER_EXT_MDNS_INSTANCES_H

#include <map>

#include "event_manager.h"

#include "mdns_callback_observer.h"

namespace OHOS::NetManagerStandard {
class MDnsDiscoveryInstance final {
public:
    [[nodiscard]] sptr<MDnsDiscoveryObserver> GetObserver() const;
    [[nodiscard]] EventManager *GetEventManager() const;

    static MDnsDiscoveryInstance *MakeMDnsDiscovery(EventManager *eventManager);
    static void DeleteMDnsDiscovery(MDnsDiscoveryInstance *mDnsDiscovery);

    std::string context_;
    std::string serviceType_;
    sptr<MDnsDiscoveryObserver> observer_;
    static std::map<MDnsDiscoveryObserver *, MDnsDiscoveryInstance *> discoverInstanceMap_;

private:
    EventManager *manager_;
    explicit MDnsDiscoveryInstance(EventManager *eventManager);
    ~MDnsDiscoveryInstance() = default;
};
} // namespace OHOS::NetManagerStandard
#endif /* NETMANAGER_EXT_MDNS_INSTANCES_H */
