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

#include "network_exec.h"

#include "network_constant.h"
#include "net_conn_client.h"
#include "net_manager_constants.h"
#if HAS_TELEPHONY
#include "core_service_client.h"
#endif
#include "netmanager_base_log.h"
#include "napi_utils.h"
#include "securec.h"

namespace OHOS::NetManagerStandard {
static constexpr const int ERROR_PARAM_NUM = 2;
static constexpr const char *ERROR_MSG = "failed";
static constexpr const char *NETWORK_NONE = "none";
static constexpr const char *NETWORK_WIFI = "WiFi";

#if HAS_TELEPHONY
static std::string CellularTypeToString(Telephony::SignalInformation::NetworkType type)
{
    switch (type) {
        case Telephony::SignalInformation::NetworkType::GSM:
            return "2g";
        case Telephony::SignalInformation::NetworkType::CDMA:
        case Telephony::SignalInformation::NetworkType::WCDMA:
        case Telephony::SignalInformation::NetworkType::TDSCDMA:
            return "3g";
        case Telephony::SignalInformation::NetworkType::LTE:
            return "4g";
        default:
            break;
    }
    return "5g";
}
#endif

static napi_value MakeNetworkResponse(napi_env env, const std::set<NetBearType> &bearerTypes)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (bearerTypes.find(BEARER_WIFI) != bearerTypes.end()) {
        NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_WIFI);
        NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
        return obj;
    }

#if HAS_TELEPHONY
    if (bearerTypes.find(BEARER_CELLULAR) != bearerTypes.end()) {
        std::vector<sptr<Telephony::SignalInformation>> vec;
        DelayedRefSingleton<Telephony::CoreServiceClient>::GetInstance().GetSignalInfoList(0, vec);
        if (vec.empty()) {
            NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_NONE);
            NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
            return obj;
        }

        std::sort(vec.begin(), vec.end(),
                  [](const sptr<Telephony::SignalInformation> &info1, const sptr<Telephony::SignalInformation> &info2)
                      -> bool { return info1->GetSignalLevel() > info2->GetSignalLevel(); });
        NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, CellularTypeToString(vec[0]->GetNetworkType()));
        NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, true);
        return obj;
    }
#endif

    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_NONE);
    NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
    return obj;
}

bool NetworkExec::ExecGetType(GetTypeContext *context)
{
    NETMANAGER_BASE_LOGI("NetworkExec::ExecGetType");
    NetHandle handle;
    auto ret = DelayedSingleton<NetConnClient>::GetInstance()->GetDefaultNet(handle);
    if (ret != NETMANAGER_SUCCESS) {
        context->SetErrorCode(ret);
        return ret == NETMANAGER_SUCCESS;
    }

    if (handle.GetNetId() == 0) {
        context->SetErrorCode(NETMANAGER_ERR_INTERNAL);
        return false;
    }

    NetAllCapabilities cap;
    ret = DelayedSingleton<NetConnClient>::GetInstance()->GetNetCapabilities(handle, cap);
    if (ret == NETMANAGER_SUCCESS) {
        context->SetCap(cap);
    }

    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value NetworkExec::GetTypeCallback(GetTypeContext *context)
{
    if (!context->IsExecOK()) {
        napi_value fail = context->GetFailCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), fail) == napi_function) {
            napi_value argv[ERROR_PARAM_NUM] = {
                NapiUtils::CreateStringUtf8(context->GetEnv(), ERROR_MSG),
                NapiUtils::CreateInt32(context->GetEnv(), context->GetErrorCode()),
            };
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), fail,
                                    ERROR_PARAM_NUM, argv);
        }

        napi_value complete = context->GetCompleteCallback();
        // if ok complete will be called in observer
        if (NapiUtils::GetValueType(context->GetEnv(), complete) == napi_function) {
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), complete, 0,
                                    nullptr);
        }
    } else {
        napi_value success = context->GetSuccessCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), success) == napi_function) {
            auto cap = context->GetCap();
            auto obj = MakeNetworkResponse(context->GetEnv(), cap.bearerTypes_);
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), success, 1, &obj);
        }
    }

    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetworkExec::ExecSubscribe(SubscribeContext *context)
{
    NETMANAGER_BASE_LOGI("NetworkExec::ExecSubscribe");
    NetHandle handle;
    auto ret = DelayedSingleton<NetConnClient>::GetInstance()->GetDefaultNet(handle);
    if (ret != NETMANAGER_SUCCESS) {
        context->SetErrorCode(ret);
        return ret == NETMANAGER_SUCCESS;
    }

    if (handle.GetNetId() == 0) {
        context->SetErrorCode(NETMANAGER_ERR_INTERNAL);
        return false;
    }

    NetAllCapabilities cap;
    ret = DelayedSingleton<NetConnClient>::GetInstance()->GetNetCapabilities(handle, cap);
    if (ret == NETMANAGER_SUCCESS) {
        context->SetCap(cap);
    }

    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value NetworkExec::SubscribeCallback(SubscribeContext *context)
{
    if (!context->IsExecOK()) {
        napi_value fail = context->GetFailCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), fail) == napi_function) {
            napi_value argv[ERROR_PARAM_NUM] = {
                NapiUtils::CreateStringUtf8(context->GetEnv(), ERROR_MSG),
                NapiUtils::CreateInt32(context->GetEnv(), context->GetErrorCode()),
            };
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), fail,
                                    ERROR_PARAM_NUM, argv);
        }
    } else {
        napi_value success = context->GetSuccessCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), success) == napi_function) {
            auto cap = context->GetCap();
            auto obj = MakeNetworkResponse(context->GetEnv(), cap.bearerTypes_);
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), success, 1, &obj);
        }
    }

    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetworkExec::ExecUnsubscribe(UnsubscribeContext *context)
{
    return true;
}

napi_value NetworkExec::UnsubscribeCallback(UnsubscribeContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}
} // namespace OHOS::NetManagerStandard