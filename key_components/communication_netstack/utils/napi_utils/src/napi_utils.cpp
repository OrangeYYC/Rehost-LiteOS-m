/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <algorithm>
#include <new>
#include <string>
#include <vector>

#include "securec.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "node_api.h"
#include "base_context.h"

namespace OHOS::NetStack::NapiUtils {
static constexpr const char *GLOBAL_JSON = "JSON";

static constexpr const char *GLOBAL_JSON_STRINGIFY = "stringify";

static constexpr const char *GLOBAL_JSON_PARSE = "parse";

static constexpr const char *CODE = "code";

static constexpr const char *MSG = "message";

napi_valuetype GetValueType(napi_env env, napi_value value)
{
    if (value == nullptr) {
        return napi_undefined;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, value, &valueType), napi_undefined);
    return valueType;
}

/* named property */
bool HasNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    if (GetValueType(env, object) != napi_object) {
        return false;
    }

    bool hasProperty = false;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, propertyName.c_str(), &hasProperty), false);
    return hasProperty;
}

napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    if (GetValueType(env, object) != napi_object) {
        return GetUndefined(env);
    }

    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, object, propertyName.c_str(), &value));
    return value;
}

void SetNamedProperty(napi_env env, napi_value object, const std::string &name, napi_value value)
{
    if (GetValueType(env, object) != napi_object) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), value);
}

std::vector<std::string> GetPropertyNames(napi_env env, napi_value object)
{
    if (GetValueType(env, object) != napi_object) {
        return {};
    }

    std::vector<std::string> ret;
    napi_value names = nullptr;
    NAPI_CALL_BASE(env, napi_get_property_names(env, object, &names), ret);
    uint32_t length = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, names, &length), ret);
    for (uint32_t index = 0; index < length; ++index) {
        napi_value name = nullptr;
        if (napi_get_element(env, names, index, &name) != napi_ok) {
            continue;
        }
        if (GetValueType(env, name) != napi_string) {
            continue;
        }
        ret.emplace_back(GetStringFromValueUtf8(env, name));
    }
    return ret;
}

/* UINT32 */
napi_value CreateUint32(napi_env env, uint32_t code)
{
    napi_value value = nullptr;
    if (napi_create_uint32(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

/* UINT64 */
napi_value CreateUint64(napi_env env, uint64_t code)
{
    napi_value value = nullptr;
    if (napi_create_bigint_uint64(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

uint32_t GetUint32FromValue(napi_env env, napi_value value)
{
    if (GetValueType(env, value) != napi_number) {
        return 0;
    }

    uint32_t ret = 0;
    NAPI_CALL_BASE(env, napi_get_value_uint32(env, value, &ret), 0);
    return ret;
}

uint32_t GetUint32Property(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return 0;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetUint32FromValue(env, value);
}

void SetUint32Property(napi_env env, napi_value object, const std::string &name, uint32_t value)
{
    napi_value jsValue = CreateUint32(env, value);
    if (GetValueType(env, jsValue) != napi_number) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

void SetUint64Property(napi_env env, napi_value object, const std::string &name, uint64_t value)
{
    napi_value jsValue = CreateUint64(env, value);
    if (GetValueType(env, jsValue) != napi_bigint) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* INT32 */
napi_value CreateInt32(napi_env env, int32_t code)
{
    napi_value value = nullptr;
    if (napi_create_int32(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

int32_t GetInt32FromValue(napi_env env, napi_value value)
{
    if (GetValueType(env, value) != napi_number) {
        return 0;
    }

    int32_t ret = 0;
    NAPI_CALL_BASE(env, napi_get_value_int32(env, value, &ret), 0);
    return ret;
}

int32_t GetInt32Property(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return 0;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetInt32FromValue(env, value);
}

void SetInt32Property(napi_env env, napi_value object, const std::string &name, int32_t value)
{
    napi_value jsValue = CreateInt32(env, value);
    if (GetValueType(env, jsValue) != napi_number) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* String UTF8 */
napi_value CreateStringUtf8(napi_env env, const std::string &str)
{
    napi_value value = nullptr;
    if (napi_create_string_utf8(env, str.c_str(), strlen(str.c_str()), &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

std::string GetStringFromValueUtf8(napi_env env, napi_value value)
{
    if (GetValueType(env, value) != napi_string) {
        return {};
    }

    std::string result;
    size_t stringLength = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, nullptr, 0, &stringLength), result);
    if (stringLength == 0) {
        return result;
    }

    auto deleter = [](char *s) { free(reinterpret_cast<void *>(s)); };
    std::unique_ptr<char, decltype(deleter)> str(static_cast<char *>(malloc(stringLength + 1)), deleter);
    if (memset_s(str.get(), stringLength + 1, 0, stringLength + 1) != EOK) {
        return result;
    }
    size_t length = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, str.get(), stringLength + 1, &length), result);
    if (length > 0) {
        result.append(str.get(), length);
    }
    return result;
}

std::string GetStringPropertyUtf8(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return "";
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetStringFromValueUtf8(env, value);
}

void SetStringPropertyUtf8(napi_env env, napi_value object, const std::string &name, const std::string &value)
{
    napi_value jsValue = CreateStringUtf8(env, value);
    if (GetValueType(env, jsValue) != napi_string) {
        return;
    }
    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* array buffer */
bool ValueIsArrayBuffer(napi_env env, napi_value value)
{
    if (value == nullptr) {
        return false;
    }
    bool isArrayBuffer = false;
    NAPI_CALL_BASE(env, napi_is_arraybuffer(env, value, &isArrayBuffer), false);
    return isArrayBuffer;
}

void *GetInfoFromArrayBufferValue(napi_env env, napi_value value, size_t *length)
{
    if (length == nullptr) {
        return nullptr;
    }

    void *data = nullptr;
    NAPI_CALL(env, napi_get_arraybuffer_info(env, value, &data, length));
    return data;
}

napi_value CreateArrayBuffer(napi_env env, size_t length, void **data)
{
    if (length == 0) {
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_arraybuffer(env, length, data, &result));
    return result;
}

/* object */
napi_value CreateObject(napi_env env)
{
    napi_value object = nullptr;
    NAPI_CALL(env, napi_create_object(env, &object));
    return object;
}

/* undefined */
napi_value GetUndefined(napi_env env)
{
    napi_value undefined = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &undefined));
    return undefined;
}

/* function */
napi_value CallFunction(napi_env env, napi_value recv, napi_value func, size_t argc, const napi_value *argv)
{
    napi_value res = nullptr;
    NAPI_CALL(env, napi_call_function(env, recv, func, argc, argv, &res));
    return res;
}

napi_value CreateFunction(napi_env env, const std::string &name, napi_callback func, void *arg)
{
    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_function(env, name.c_str(), strlen(name.c_str()), func, arg, &res));
    return res;
}

/* reference */
napi_ref CreateReference(napi_env env, napi_value callback)
{
    napi_ref callbackRef = nullptr;
    NAPI_CALL(env, napi_create_reference(env, callback, 1, &callbackRef));
    return callbackRef;
}

napi_value GetReference(napi_env env, napi_ref callbackRef)
{
    napi_value callback = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, callbackRef, &callback));
    return callback;
}

void DeleteReference(napi_env env, napi_ref callbackRef)
{
    (void)napi_delete_reference(env, callbackRef);
}

/* boolean */
bool GetBooleanProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return false;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    bool ret = false;
    NAPI_CALL_BASE(env, napi_get_value_bool(env, value, &ret), false);
    return ret;
}

void SetBooleanProperty(napi_env env, napi_value object, const std::string &name, bool value)
{
    napi_value jsValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, value, &jsValue));
    if (GetValueType(env, jsValue) != napi_boolean) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

napi_value GetBoolean(napi_env env, bool value)
{
    napi_value jsValue = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, value, &jsValue));
    return jsValue;
}

bool GetBooleanFromValue(napi_env env, napi_value value)
{
    if (GetValueType(env, value) != napi_boolean) {
        return GetUndefined(env);
    }

    bool ret = false;
    NAPI_CALL_BASE(env, napi_get_value_bool(env, value, &ret), false);
    return ret;
}

/* define properties */
void DefineProperties(napi_env env, napi_value object,
                      const std::initializer_list<napi_property_descriptor> &properties)
{
    napi_property_descriptor descriptors[properties.size()];
    std::copy(properties.begin(), properties.end(), descriptors);

    (void)napi_define_properties(env, object, properties.size(), descriptors);
}

/* array */
napi_value CreateArray(napi_env env, size_t length)
{
    if (length == 0) {
        napi_value res = nullptr;
        NAPI_CALL(env, napi_create_array(env, &res));
        return res;
    }
    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, length, &res));
    return res;
}

void SetArrayElement(napi_env env, napi_value array, uint32_t index, napi_value value)
{
    (void)napi_set_element(env, array, index, value);
}

bool IsArray(napi_env env, napi_value value)
{
    bool result = false;
    NAPI_CALL_BASE(env, napi_is_array(env, value, &result), false);
    return result;
}

uint32_t GetArrayLength(napi_env env, napi_value arr)
{
    uint32_t arrayLength = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, arr, &arrayLength), 0);
    return arrayLength;
}

napi_value GetArrayElement(napi_env env, napi_value arr, uint32_t index)
{
    napi_value elementValue = nullptr;
    NAPI_CALL(env, napi_get_element(env, arr, index, &elementValue));
    return elementValue;
}

/* JSON */
napi_value JsonStringify(napi_env env, napi_value object)
{
    napi_value undefined = GetUndefined(env);

    if (GetValueType(env, object) != napi_object) {
        return undefined;
    }

    napi_value global = nullptr;
    NAPI_CALL_BASE(env, napi_get_global(env, &global), undefined);
    napi_value json = nullptr;
    NAPI_CALL_BASE(env, napi_get_named_property(env, global, GLOBAL_JSON, &json), undefined);
    napi_value stringify = nullptr;
    NAPI_CALL_BASE(env, napi_get_named_property(env, json, GLOBAL_JSON_STRINGIFY, &stringify), undefined);
    if (GetValueType(env, stringify) != napi_function) {
        return undefined;
    }

    napi_value res = nullptr;
    napi_value argv[1] = {object};
    NAPI_CALL_BASE(env, napi_call_function(env, json, stringify, 1, argv, &res), undefined);
    return res;
}

napi_value JsonParse(napi_env env, napi_value str)
{
    napi_value undefined = GetUndefined(env);

    if (GetValueType(env, str) != napi_string) {
        return undefined;
    }

    napi_value global = nullptr;
    NAPI_CALL_BASE(env, napi_get_global(env, &global), undefined);
    napi_value json = nullptr;
    NAPI_CALL_BASE(env, napi_get_named_property(env, global, GLOBAL_JSON, &json), undefined);
    napi_value parse = nullptr;
    NAPI_CALL_BASE(env, napi_get_named_property(env, json, GLOBAL_JSON_PARSE, &parse), undefined);
    if (GetValueType(env, parse) != napi_function) {
        return undefined;
    }

    napi_value res = nullptr;
    napi_value argv[1] = {str};
    NAPI_CALL_BASE(env, napi_call_function(env, json, parse, 1, argv, &res), undefined);
    return res;
}

/* libuv */
void CreateUvQueueWork(napi_env env, void *data, void(handler)(uv_work_t *, int status))
{
    uv_loop_s *loop = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_uv_event_loop(env, &loop));

    auto work = new uv_work_t;
    work->data = data;

    (void)uv_queue_work(
        loop, work, [](uv_work_t *) {}, handler);
}

/* scope */
napi_handle_scope OpenScope(napi_env env)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL(env, napi_open_handle_scope(env, &scope));
    return scope;
}

void CloseScope(napi_env env, napi_handle_scope scope)
{
    (void)napi_close_handle_scope(env, scope);
}

void CreateUvQueueWorkEnhanced(napi_env env, void *data, void (*handler)(napi_env env, napi_status status, void *data))
{
    uv_loop_s *loop = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_uv_event_loop(env, &loop));

    class WorkData {
    public:
        WorkData() = delete;

        WorkData(napi_env env, void *data, void (*handler)(napi_env env, napi_status status, void *data))
            : env_(env), data_(data), handler_(handler)
        {
        }

        napi_env env_;
        void *data_;
        void (*handler_)(napi_env env, napi_status status, void *data);
    };

    auto workData = new WorkData(env, data, handler);

    auto work = new uv_work_t;
    work->data = reinterpret_cast<void *>(workData);

    auto callback = [](uv_work_t *work, int status) {
        auto workData = static_cast<WorkData *>(work->data);
        if (!workData) {
            delete work;
            return;
        }

        if (!workData->env_ || !workData->data_ || !workData->handler_) {
            delete workData;
            delete work;
            return;
        }

        napi_env env = workData->env_;
        auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);

        workData->handler_(workData->env_, static_cast<napi_status>(status), workData->data_);

        delete workData;
        delete work;
    };

    (void)uv_queue_work(
        loop, work, [](uv_work_t *) {}, callback);
}

/* error */
napi_value CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage)
{
    napi_value result = nullptr;
    result = CreateObject(env);
    SetNamedProperty(env, result, CODE, CreateInt32(env, errorCode));
    SetNamedProperty(env, result, MSG, CreateStringUtf8(env, errorMessage));
    return result;
}
} // namespace OHOS::NetStack::NapiUtils
