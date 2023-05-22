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

#include <vector>

#include "net_manager_constants.h"
#include "netmanager_ext_log.h"

#include "constant.h"
#include "mdns_callback_observer.h"
#include "mdns_instances.h"

namespace OHOS {
namespace NetManagerStandard {

void MDnsRegistrationObserver::HandleRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) {}

void MDnsRegistrationObserver::HandleUnRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) {}

void MDnsRegistrationObserver::HandleRegisterResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) {}

void MDnsDiscoveryObserver::HandleStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) {}

void MDnsDiscoveryObserver::HandleStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) {}

void MDnsDiscoveryObserver::EmitStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MDnsDiscoveryInstance *mdnsDisdicover = MDnsDiscoveryInstance::discoverInstanceMap_[this];
    if (mdnsDisdicover == nullptr) {
        NETMANAGER_EXT_LOGE("can not find MDnsDiscoveryInstance handle");
        return;
    }

    if (!mdnsDisdicover->GetEventManager()->HasEventListener(EVENT_SERVICESTART)) {
        NETMANAGER_EXT_LOGE("no event listener find %{public}s", EVENT_SERVICESTART);
        return;
    }

    auto pair = new std::pair<int32_t, MDnsServiceInfo>(retCode, serviceInfo);
    mdnsDisdicover->GetEventManager()->EmitByUv(EVENT_SERVICESTART, pair, StartDiscoveryServiceCallback);
}

void MDnsDiscoveryObserver::EmitStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MDnsDiscoveryInstance *mdnsDisdicover = MDnsDiscoveryInstance::discoverInstanceMap_[this];
    if (mdnsDisdicover == nullptr) {
        NETMANAGER_EXT_LOGE("can not find MDnsDiscoveryInstance handle");
        return;
    }

    if (!mdnsDisdicover->GetEventManager()->HasEventListener(EVENT_SERVICESTOP)) {
        NETMANAGER_EXT_LOGE("no event listener find %{public}s", EVENT_SERVICESTOP);
        return;
    }

    auto pair = new std::pair<int32_t, MDnsServiceInfo>(retCode, serviceInfo);
    mdnsDisdicover->GetEventManager()->EmitByUv(EVENT_SERVICESTOP, pair, StopDiscoveryServiceCallback);
}

void MDnsDiscoveryObserver::HandleServiceFound(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MDnsDiscoveryInstance *mdnsDisdicover = MDnsDiscoveryInstance::discoverInstanceMap_[this];
    if (mdnsDisdicover == nullptr) {
        NETMANAGER_EXT_LOGE("can not find MDnsDiscoveryInstance handle");
        return;
    }

    if (!mdnsDisdicover->GetEventManager()->HasEventListener(EVENT_SERVICEFOUND)) {
        NETMANAGER_EXT_LOGE("no event listener find %{public}s", EVENT_SERVICEFOUND);
        return;
    }

    mdnsDisdicover->GetEventManager()->EmitByUv(EVENT_SERVICEFOUND, new MDnsServiceInfo(serviceInfo),
                                                ServiceFoundCallback);
}

void MDnsDiscoveryObserver::HandleServiceLost(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    MDnsDiscoveryInstance *mdnsDisdicover = MDnsDiscoveryInstance::discoverInstanceMap_[this];
    if (mdnsDisdicover == nullptr) {
        NETMANAGER_EXT_LOGE("can not find MDnsDiscoveryInstance handle");
        return;
    }

    if (!mdnsDisdicover->GetEventManager()->HasEventListener(EVENT_SERVICELOST)) {
        NETMANAGER_EXT_LOGE("no event listener find %{public}s", EVENT_SERVICELOST);
        return;
    }

    mdnsDisdicover->GetEventManager()->EmitByUv(EVENT_SERVICELOST, new MDnsServiceInfo(serviceInfo),
                                                ServiceFoundCallback);
}

napi_value CreateCallbackParam(const MDnsServiceInfo &serviceInfo, napi_env env)
{
    napi_value object = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, object, SERVICEINFO_TYPE, serviceInfo.type);
    NapiUtils::SetStringPropertyUtf8(env, object, SERVICEINFO_NAME, serviceInfo.name);
    NapiUtils::SetInt32Property(env, object, SERVICEINFO_PORT, serviceInfo.port);

    napi_value eleObj = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, eleObj, SERVICEINFO_ADDRESS, serviceInfo.addr);
    NapiUtils::SetInt32Property(env, eleObj, SERVICEINFO_PORT, serviceInfo.port);
    int32_t family = serviceInfo.family == MDnsServiceInfo::IPV6 ? 0 : 1;
    NapiUtils::SetInt32Property(env, eleObj, SERVICEINFO_FAMILY, family);

    NapiUtils::SetNamedProperty(env, object, SERVICEINFO_HOST, eleObj);
    return object;
}

napi_value MDnsDiscoveryObserver::CreateStartDiscoveryService(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<int32_t, MDnsServiceInfo> *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetUint32Property(env, obj, ERRCODE, pair->first);
    napi_value infoObj = CreateCallbackParam(pair->second, env);
    NapiUtils::SetNamedProperty(env, obj, SERVICEINFO, infoObj);
    delete pair;
    return obj;
}

void MDnsDiscoveryObserver::StartDiscoveryServiceCallback(uv_work_t *work, int32_t status)
{
    CallbackTemplate<CreateStartDiscoveryService>(work, status);
}

napi_value MDnsDiscoveryObserver::CreateStopDiscoveryService(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<int32_t, MDnsServiceInfo> *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetUint32Property(env, obj, ERRCODE, pair->first);
    napi_value infoObj = CreateCallbackParam(pair->second, env);
    NapiUtils::SetNamedProperty(env, obj, SERVICEINFO, infoObj);
    delete pair;
    return obj;
}

void MDnsDiscoveryObserver::StopDiscoveryServiceCallback(uv_work_t *work, int32_t status)
{
    CallbackTemplate<CreateStopDiscoveryService>(work, status);
}

napi_value MDnsDiscoveryObserver::CreateServiceFound(napi_env env, void *data)
{
    auto serviceInfo = static_cast<MDnsServiceInfo *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    napi_value infoObj = CreateCallbackParam(*serviceInfo, env);
    NapiUtils::SetNamedProperty(env, obj, SERVICEINFO, infoObj);
    delete serviceInfo;
    return obj;
}

void MDnsDiscoveryObserver::ServiceFoundCallback(uv_work_t *work, int32_t status)
{
    CallbackTemplate<CreateServiceFound>(work, status);
}

napi_value MDnsDiscoveryObserver::CreateServiceLost(napi_env env, void *data)
{
    auto serviceInfo = static_cast<MDnsServiceInfo *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    napi_value infoObj = CreateCallbackParam(*serviceInfo, env);
    NapiUtils::SetNamedProperty(env, obj, SERVICEINFO, infoObj);
    delete serviceInfo;
    return obj;
}

void MDnsDiscoveryObserver::ServiceLostCallback(uv_work_t *work, int32_t status)
{
    CallbackTemplate<CreateServiceLost>(work, status);
}

void MDnsResolveObserver::HandleResolveResult(const MDnsServiceInfo &serviceInfo, int32_t retCode)
{
    NETMANAGER_EXT_LOGI("HandleResolveResult [%{public}s][%{public}s][%{public}d]", serviceInfo.name.c_str(),
                        serviceInfo.type.c_str(), serviceInfo.port);
    mutex_.lock();
    retCode_ = retCode;
    serviceInfo_ = serviceInfo;
    resolved_ = true;
    mutex_.unlock();
    cv_.notify_one();
}
} // namespace NetManagerStandard
} // namespace OHOS
