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

#include "statistics_callback_observer.h"

#include "constant.h"
#include "napi_constant.h"
#include "netmanager_base_log.h"
#include "statistics_observer_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t StatisticsCallbackObserver::NetIfaceStatsChanged(const std::string &iface)
{
    if (!DelayedSingleton<StatisticsObserverWrapper>::GetInstance()->GetEventManager()->HasEventListener(
        EVENT_STATS_CHANGE)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_STATS_CHANGE);
        return 0;
    }
    auto pair = new std::pair<std::string, uint32_t>;
    pair->first = iface;
    pair->second = KEY_UID_ARG;
    DelayedSingleton<StatisticsObserverWrapper>::GetInstance()->GetEventManager()->EmitByUv(
        EVENT_STATS_CHANGE, pair, NetIfaceStatsChangedCallback);
    return 0;
}

int32_t StatisticsCallbackObserver::NetUidStatsChanged(const std::string &iface, uint32_t uid)
{
    if (!DelayedSingleton<StatisticsObserverWrapper>::GetInstance()->GetEventManager()->HasEventListener(
        EVENT_STATS_CHANGE)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_STATS_CHANGE);
        return 0;
    }
    auto pair = new std::pair<std::string, uint32_t>;
    pair->first = iface;
    pair->second = uid;
    DelayedSingleton<StatisticsObserverWrapper>::GetInstance()->GetEventManager()->EmitByUv(EVENT_STATS_CHANGE, pair,
                                                                                            NetUidStatsChangedCallback);
    return 0;
}

napi_value StatisticsCallbackObserver::CreateNetIfaceStatsChangedParam(napi_env env, void *data)
{
    auto pair(reinterpret_cast<std::pair<std::string, uint32_t> *>(data));
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_IFACE, pair->first);
    delete pair;
    return obj;
}

napi_value StatisticsCallbackObserver::CreateNetUidStatsChangedParam(napi_env env, void *data)
{
    auto pair(reinterpret_cast<std::pair<std::string, uint32_t> *>(data));
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_IFACE, pair->first);
    NapiUtils::SetUint32Property(env, obj, KEY_UID, pair->second);
    delete pair;
    return obj;
}

void StatisticsCallbackObserver::NetIfaceStatsChangedCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetIfaceStatsChangedParam>(work, status);
}

void StatisticsCallbackObserver::NetUidStatsChangedCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetUidStatsChangedParam>(work, status);
}
} // namespace NetManagerStandard
} // namespace OHOS
