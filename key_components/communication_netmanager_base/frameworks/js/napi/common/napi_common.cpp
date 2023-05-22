/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "napi_common.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *CODE = "code";
constexpr const char *MSG = "message";
} // namespace

bool NapiCommon::IsValidEvent(const std::string &eventInfo, int32_t &eventId)
{
    bool isEvent = true;
    if (!eventInfo.compare(NET_UID_POLICY_CHANGE)) {
        eventId = EVENT_NET_UID_POLICY_CHANGE;
    } else if (!eventInfo.compare(NET_CELLULAR_POLICY_CHANGE)) {
        eventId = EVENT_NET_CELLULAR_POLICY_CHANGE;
    } else if (!eventInfo.compare(NET_STRATEGY_SWITCH_CHANGE)) {
        eventId = EVENT_NET_STRATEGY_SWITCH_CHANGE;
    } else if (!eventInfo.compare(NET_AVAILABLE_CHANGE)) {
        eventId = EVENT_NET_AVAILABLE_CHANGE;
    } else if (!eventInfo.compare(NET_CAPABILITIES_CHANGE)) {
        eventId = EVENT_NET_CAPABILITIES_CHANGE;
    } else if (!eventInfo.compare(NET_CONNECTION_CHANGE)) {
        eventId = EVENT_NET_CONNECTION_CHANGE;
    } else if (!eventInfo.compare(NET_LOST_CHANGE)) {
        eventId = EVENT_NET_LOST_CHANGE;
    } else if (!eventInfo.compare(NET_STATS_CHANGE)) {
        eventId = EVENT_NET_STATS_CHANGE;
    } else if (!eventInfo.compare(NET_BLOCK_STATUS_CHANGE)) {
        eventId = EVENT_NET_BLOCK_STATUS_CHANGE;
    } else if (!eventInfo.compare(NET_UNAVAILABLE_CHANGE)) {
        eventId = EVENT_NET_UNAVAILABLE_CHANGE;
    } else {
        isEvent = false;
    }
    return isEvent;
}

napi_value NapiCommon::CreateCodeMessage(napi_env env, const std::string &msg, int32_t code)
{
    napi_value messageCodeInfo = nullptr;
    napi_value messageInfo = nullptr;
    napi_value codeInfo = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, msg.c_str(), msg.length(), &messageInfo));
    NAPI_CALL(
        env, napi_create_string_utf8(env, std::to_string(code).c_str(), std::to_string(code).length(), &codeInfo));
    NAPI_CALL(env, napi_create_error(env, codeInfo, messageInfo, &messageCodeInfo));
    return messageCodeInfo;
}

void NapiCommon::SetPropertyBool(
    napi_env env, napi_value object, const std::string &propertyName, bool property)
{
    napi_value propertyDest = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, property, &propertyDest));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, propertyName.c_str(), propertyDest));
}

void NapiCommon::SetPropertyInt32(
    napi_env env, napi_value object, const std::string &propertyName, int32_t property)
{
    napi_value propertyDest = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, property, &propertyDest));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, propertyName.c_str(), propertyDest));
}

void NapiCommon::SetPropertyInt64(
    napi_env env, napi_value object, const std::string &propertyName, int64_t property)
{
    napi_value propertyDest = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, property, &propertyDest));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, propertyName.c_str(), propertyDest));
}

void NapiCommon::SetPropertyUint32(
    napi_env env, napi_value object, const std::string &propertyName, uint32_t property)
{
    napi_value propertyDest = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, property, &propertyDest));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, propertyName.c_str(), propertyDest));
}

void NapiCommon::SetPropertyString(
    napi_env env, napi_value object, const std::string &propertyName, const std::string &property)
{
    napi_value propertyDest = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, property.c_str(), property.length(), &propertyDest));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, propertyName.c_str(), propertyDest));
}

void NapiCommon::GetPropertyString(
    napi_env env, napi_value object, const std::string &propertyName, std::string &property)
{
    napi_value value = nullptr;
    char propertyBuffer[PROPERTY_MAX_BYTE] = {0};
    size_t realByte = 0;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, object, propertyName.c_str(), &value));
    NAPI_CALL_RETURN_VOID(
        env, napi_get_value_string_utf8(env, value, propertyBuffer, PROPERTY_MAX_BYTE, &realByte));
    property = propertyBuffer;
}

void NapiCommon::GetPropertyInt32(
    napi_env env, napi_value object, const std::string &propertyName, int32_t &property)
{
    napi_value value = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, object, propertyName.c_str(), &value));
    NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, value, &property));
}

void NapiCommon::GetPropertyInt64(
    napi_env env, napi_value object, const std::string &propertyName, int64_t &property)
{
    napi_value value = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, object, propertyName.c_str(), &value));
    NAPI_CALL_RETURN_VOID(env, napi_get_value_int64(env, value, &property));
}

napi_value NapiCommon::NapiValueByInt32(napi_env env, int32_t property)
{
    napi_value value = nullptr;
    NAPI_CALL(env, napi_create_int32(env, property, &value));
    return value;
}

std::string NapiCommon::GetNapiStringValue(
    napi_env env, napi_value napiValue, const std::string &name, const std::string &defValue)
{
    napi_value value = GetNamedProperty(env, napiValue, name);
    if (value != nullptr) {
        return GetStringFromValue(env, value);
    } else {
        return defValue;
    }
}

std::string NapiCommon::GetStringFromValue(napi_env env, napi_value value)
{
    char msgChars[MAX_TEXT_LENGTH] = {0};
    size_t msgLength = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, msgChars, MAX_TEXT_LENGTH, &msgLength), "");
    if (msgLength > 0) {
        return std::string(msgChars, 0, msgLength);
    } else {
        return "";
    }
}

napi_value NapiCommon::GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, object, propertyName.data(), &hasProperty));
    if (hasProperty) {
        NAPI_CALL(env, napi_get_named_property(env, object, propertyName.data(), &value));
    }
    return value;
}

int32_t NapiCommon::GetNapiInt32Value(
    napi_env env, napi_value napiValue, const std::string &name, const int32_t &defValue)
{
    napi_value value = GetNamedProperty(env, napiValue, name);
    if (value != nullptr) {
        int32_t intValue = 0;
        napi_status getIntStatus = napi_get_value_int32(env, value, &intValue);
        if (getIntStatus == napi_ok) {
            return intValue;
        }
    }
    return defValue;
}

int64_t NapiCommon::GetNapiInt64Value(
    napi_env env, napi_value napiValue, const std::string &name, const int64_t &defValue)
{
    napi_value value = GetNamedProperty(env, napiValue, name);
    if (value != nullptr) {
        int64_t intValue = 0;
        napi_status getIntStatus = napi_get_value_int64(env, value, &intValue);
        if (getIntStatus == napi_ok) {
            return intValue;
        }
    }
    return defValue;
}

bool NapiCommon::MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, value, &valueType), false);
    return valueType == targetType;
}

bool NapiCommon::MatchParameters(
    napi_env env, const napi_value parameters[], std::initializer_list<napi_valuetype> valueTypes)
{
    if (parameters == nullptr) {
        return false;
    }
    int i = 0;
    for (auto beg = valueTypes.begin(); beg != valueTypes.end(); ++beg) {
        if (!MatchValueType(env, parameters[i], *beg)) {
            return false;
        }
        ++i;
    }
    return true;
}

napi_value NapiCommon::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiCommon::HandleAsyncWork(napi_env env, BaseContext *baseContext, const std::string &workName,
    napi_async_execute_callback execute, napi_async_complete_callback complete)
{
    NETMGR_LOG_I("NapiCommon HandleAsyncWork workName = %{public}s", workName.c_str());
    std::unique_ptr<BaseContext> context(baseContext);
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_invalid_arg);
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    napi_value result = nullptr;
    if (context->callbackRef == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &result));
    }
    napi_value resource = CreateUndefined(env);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, workName.data(), NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env,
        napi_create_async_work(
            env, resource, resourceName, execute, complete, static_cast<void *>(context.get()), &context->work));
    napi_status queueWorkStatus = napi_queue_async_work(env, context->work);
    if (queueWorkStatus == napi_ok) {
        context.release();
        NETMGR_LOG_I("NapiCommon HandleAsyncWork napi_queue_async_work ok");
    } else {
        std::string errorCode = std::to_string(queueWorkStatus);
        std::string errorMessage = "error at napi_queue_async_work";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
    }
    NETMGR_LOG_I("NapiCommon HandleAsyncWork end");
    return result;
}

void NapiCommon::Handle1ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue)
{
    NETMGR_LOG_I("Handle1ValueCallback start");
    if (baseContext == nullptr) {
        NETMGR_LOG_I("Handle1ValueCallback serious error baseContext nullptr");
        std::string errorCode = std::to_string(napi_invalid_arg);
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL_RETURN_VOID(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return;
    }
    if (baseContext->callbackRef != nullptr) {
        NETMGR_LOG_I("Handle1ValueCallback start normal callback");
        napi_value recv = CreateUndefined(env);
        napi_value callbackFunc = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, baseContext->callbackRef, &callbackFunc));
        napi_value callbackValues[] = {callbackValue};
        napi_value result = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result));
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, baseContext->callbackRef));
        NETMGR_LOG_I("Handle1ValueCallback normal callback end");
    } else if (baseContext->deferred != nullptr) {
        NETMGR_LOG_I("Handle1ValueCallback start promise callback");
        if (baseContext->resolved) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, baseContext->deferred, callbackValue));
            NETMGR_LOG_I("Handle1ValueCallback napi_resolve_deferred end");
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, baseContext->deferred, callbackValue));
            NETMGR_LOG_I("Handle1ValueCallback napi_reject_deferred end");
        }
        NETMGR_LOG_I("Handle1ValueCallback promise callback end");
    }
    napi_delete_async_work(env, baseContext->work);
    delete baseContext;
    NETMGR_LOG_I("Handle1ValueCallback end");
}

void NapiCommon::Handle2ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue)
{
    NETMGR_LOG_I("Handle2ValueCallback start");
    if (baseContext == nullptr) {
        NETMGR_LOG_I("Handle2ValueCallback serious error baseContext nullptr");
        std::string errorCode = std::to_string(napi_invalid_arg);
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL_RETURN_VOID(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return;
    }
    if (baseContext->callbackRef != nullptr) {
        NETMGR_LOG_I("Handle2ValueCallback start normal callback");
        napi_value recv = CreateUndefined(env);
        napi_value callbackFunc = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, baseContext->callbackRef, &callbackFunc));
        napi_value callbackValues[] = {nullptr, nullptr};
        callbackValues[0] = baseContext->resolved ? CreateUndefined(env) : callbackValue;
        callbackValues[1] = baseContext->resolved ? callbackValue : CreateUndefined(env);
        napi_value result = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result));
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, baseContext->callbackRef));
        NETMGR_LOG_I("Handle2ValueCallback normal callback end");
    } else if (baseContext->deferred != nullptr) {
        NETMGR_LOG_I("Handle2ValueCallback start promise callback");
        if (baseContext->resolved) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, baseContext->deferred, callbackValue));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, baseContext->deferred, callbackValue));
        }
        NETMGR_LOG_I("Handle2ValueCallback promise callback end");
    }
    napi_delete_async_work(env, baseContext->work);
    delete baseContext;
    NETMGR_LOG_I("Handle2ValueCallback end");
}

bool NapiCommon::HasNamedTypeProperty(
    napi_env env, napi_value object, napi_valuetype type, const std::string &propertyName)
{
    bool hasProperty = false;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, propertyName.data(), &hasProperty), false);
    if (hasProperty) {
        napi_value value = nullptr;
        NAPI_CALL_BASE(env, napi_get_named_property(env, object, propertyName.data(), &value), false);
        return NapiCommon::MatchValueType(env, value, type);
    }
    return false;
}

bool NapiCommon::HasNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    bool hasProperty = false;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, propertyName.data(), &hasProperty), false);
    return hasProperty;
}

bool NapiCommon::MatchObjectProperty(
    napi_env env, napi_value object, std::initializer_list<std::pair<std::string, napi_valuetype>> pairList)
{
    if (object == nullptr) {
        return false;
    }
    for (auto beg = pairList.begin(); beg != pairList.end(); ++beg) {
        if (!HasNamedTypeProperty(env, object, beg->second, beg->first)) {
            return false;
        }
    }
    return true;
}

napi_value NapiCommon::CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

napi_value NapiCommon::CreateObject(napi_env env)
{
    napi_value object = nullptr;
    NAPI_CALL(env, napi_create_object(env, &object));
    return object;
}

napi_value NapiCommon::CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage)
{
    napi_value result = CreateObject(env);
    SetPropertyInt32(env, result, CODE, errorCode);
    SetPropertyString(env, result, MSG, errorMessage);
    return result;
}
} // namespace NetManagerStandard
} // namespace OHOS
