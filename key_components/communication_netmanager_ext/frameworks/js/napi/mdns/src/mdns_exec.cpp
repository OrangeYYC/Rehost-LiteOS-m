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

#include "napi_utils.h"
#include "net_manager_constants.h"
#include "netmanager_ext_log.h"

#include "constant.h"
#include "mdns_client.h"
#include "mdns_exec.h"
#include "mdns_instances.h"

namespace OHOS {
namespace NetManagerStandard {

bool MDnsExec::ExecAddLocalService(MDnsAddLocalServiceContext *context)
{
    auto ret =
        DelayedSingleton<MDnsClient>::GetInstance()->RegisterService(context->GetServiceInfo(), context->GetObserver());
    if (ret != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(ret);
        NETMANAGER_EXT_LOGE("RegisterService error, errorCode: %{public}d", ret);
        return false;
    }
    return ret == NETMANAGER_EXT_SUCCESS;
}

bool MDnsExec::ExecRemoveLocalService(MDnsRemoveLocalServiceContext *context)
{
    sptr<IRegistrationCallback> callback = context->GetObserver();
    auto ret = DelayedSingleton<MDnsClient>::GetInstance()->UnRegisterService(callback);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(ret);
        NETMANAGER_EXT_LOGE("UnRegisterService error, errorCode: %{public}d", ret);
        return false;
    }
    return ret == NETMANAGER_EXT_SUCCESS;
}

bool MDnsExec::ExecResolveLocalService(MDnsResolveLocalServiceContext *context)
{
    auto ret =
        DelayedSingleton<MDnsClient>::GetInstance()->ResolveService(context->GetServiceInfo(), context->GetObserver());
    if (ret != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(ret);
        NETMANAGER_EXT_LOGE("ResolveService error, errorCode: %{public}d", ret);
        return false;
    }

    sptr<IResolveCallback> callback = context->GetObserver();
    MDnsResolveObserver *observer = static_cast<MDnsResolveObserver *>(callback.GetRefPtr());

    if (observer == nullptr) {
        context->SetErrorCode(NET_MDNS_ERR_UNKNOWN);
        return false;
    }

    std::unique_lock<std::mutex> lk(observer->mutex_);
    if (!observer->cv_.wait_for(lk, std::chrono::seconds(SYNC_TIMEOUT), [=]() { return observer->resolved_; })) {
        context->SetErrorCode(NET_MDNS_ERR_TIMEOUT);
        return false;
    }

    context->SetServiceInfo(observer->serviceInfo_);
    if (observer->retCode_ != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(observer->retCode_);
        NETMANAGER_EXT_LOGE("HandleResolveResult error, errorCode: %{public}d", ret);
        return false;
    }
    return observer->retCode_ == NETMANAGER_EXT_SUCCESS;
}

bool MDnsExec::ExecStartSearchingMDNS(MDnsStartSearchingContext *context)
{
    EventManager *manager = context->GetManager();
    if (manager == nullptr) {
        NETMANAGER_EXT_LOGE("manager is nullptr");
        return false;
    }
    auto discover = static_cast<MDnsDiscoveryInstance *>(manager->GetData());
    if (discover == nullptr) {
        NETMANAGER_EXT_LOGE("discover is nullptr");
        return false;
    }
    auto ret =
        DelayedSingleton<MDnsClient>::GetInstance()->StartDiscoverService(discover->serviceType_, discover->observer_);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(ret);
        NETMANAGER_EXT_LOGE("StartDiscoverService error, errorCode: %{public}d", ret);
        return false;
    }
    MDnsServiceInfo info;
    info.type = discover->serviceType_;
    discover->GetObserver()->EmitStartDiscover(info, ret);
    return ret == NETMANAGER_EXT_SUCCESS;
}

bool MDnsExec::ExecStopSearchingMDNS(MDnsStopSearchingContext *context)
{
    EventManager *manager = context->GetManager();
    if (manager == nullptr) {
        NETMANAGER_EXT_LOGE("manager is nullptr");
        return false;
    }
    auto discover = static_cast<MDnsDiscoveryInstance *>(manager->GetData());
    if (discover == nullptr) {
        NETMANAGER_EXT_LOGE("discover is nullptr");
        return false;
    }
    auto ret = DelayedSingleton<MDnsClient>::GetInstance()->StopDiscoverService(discover->observer_);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        context->SetErrorCode(ret);
        NETMANAGER_EXT_LOGE("StopDiscoverService error, errorCode: %{public}d", ret);
        return false;
    }
    MDnsServiceInfo info;
    info.type = discover->serviceType_;
    discover->GetObserver()->EmitStopDiscover(info, ret);
    return ret == NETMANAGER_EXT_SUCCESS;
}

napi_value CreateAttributeObj(napi_env env, MDnsServiceInfo serviceInfo)
{
    TxtRecord attrMap = serviceInfo.GetAttrMap();
    auto attrArrSize = attrMap.size();
    size_t index = 0;
    auto iter = attrMap.begin();
    napi_value attrArr = NapiUtils::CreateArray(env, attrArrSize);
    for (; iter != attrMap.end(); iter++, index++) {
        napi_value attrItem = NapiUtils::CreateObject(env);
        NapiUtils::SetStringPropertyUtf8(env, attrItem, SERVICEINFO_ATTR_KEY, iter->first);
        auto valArrSize = iter->second.size();
        napi_value valArr = NapiUtils::CreateArray(env, valArrSize);
        auto setIter = iter->second.begin();
        size_t setIndex = 0;
        for (; setIter != iter->second.end(); setIter++, setIndex++) {
            NapiUtils::SetArrayElement(env, valArr, setIndex, NapiUtils::CreateUint32(env, *setIter));
        }
        NapiUtils::SetNamedProperty(env, attrItem, SERVICEINFO_ATTR_VALUE, valArr);
        NapiUtils::SetArrayElement(env, attrArr, index, attrItem);
    }
    return attrArr;
}

template <class T> napi_value CreateCallbackParam(T *context)
{
    napi_env env = context->GetEnv();
    MDnsServiceInfo serviceInfo = context->GetServiceInfo();
    NETMANAGER_EXT_LOGI("CreateCallbackParam [%{public}s][%{public}s][%{public}d]", serviceInfo.name.c_str(),
                        serviceInfo.type.c_str(), serviceInfo.port);

    napi_value object = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, object, SERVICEINFO_TYPE, serviceInfo.type);
    NapiUtils::SetStringPropertyUtf8(env, object, SERVICEINFO_NAME, serviceInfo.name);
    NapiUtils::SetInt32Property(env, object, SERVICEINFO_PORT, serviceInfo.port);

    napi_value eleObj = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, eleObj, SERVICEINFO_ADDRESS, serviceInfo.addr);
    NapiUtils::SetInt32Property(env, eleObj, SERVICEINFO_PORT, serviceInfo.port);
    int32_t family = serviceInfo.family == MDnsServiceInfo::IPV6 ? 0 : 1;
    NapiUtils::SetInt32Property(env, eleObj, SERVICEINFO_FAMILY, family);

    napi_value attrArrObj = CreateAttributeObj(env, serviceInfo);

    NapiUtils::SetNamedProperty(env, object, SERVICEINFO_HOST, eleObj);
    NapiUtils::SetNamedProperty(env, object, SERVICEINFO_ATTR, attrArrObj);
    return object;
}

napi_value MDnsExec::AddLocalServiceCallback(MDnsAddLocalServiceContext *context)
{
    return CreateCallbackParam<MDnsAddLocalServiceContext>(context);
}

napi_value MDnsExec::RemoveLocalServiceCallback(MDnsRemoveLocalServiceContext *context)
{
    return CreateCallbackParam<MDnsRemoveLocalServiceContext>(context);
}

napi_value MDnsExec::ResolveLocalServiceCallback(MDnsResolveLocalServiceContext *context)
{
    return CreateCallbackParam<MDnsResolveLocalServiceContext>(context);
}

napi_value MDnsExec::StartSearchingMDNSCallback(MDnsStartSearchingContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value MDnsExec::StopSearchingMDNSCallback(MDnsStopSearchingContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}
} // namespace NetManagerStandard
} // namespace OHOS
