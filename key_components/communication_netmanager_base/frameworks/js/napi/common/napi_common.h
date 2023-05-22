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

#ifndef NAPI_COMMON_H
#define NAPI_COMMON_H

#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "base_context.h"
#include "event_context.h"

namespace OHOS {
namespace NetManagerStandard {
// define callback ref count
constexpr uint32_t CALLBACK_REF_CNT = 1;
// define property max bytes
constexpr uint32_t PROPERTY_MAX_BYTE = 128;
// define constant of 64 bytes
constexpr uint32_t BUFFER_BYTE = 64;
constexpr int32_t MAX_TEXT_LENGTH = 4096;

enum JS_CALLBACK_ARGV {
    CALLBACK_ARGV_INDEX_0 = 0,
    CALLBACK_ARGV_INDEX_1,
    CALLBACK_ARGV_CNT,
};

enum JS_ARGV_NUM {
    ARGV_NUM_0 = 0,
    ARGV_NUM_1,
    ARGV_NUM_2,
    ARGV_NUM_3,
    ARGV_NUM_4,
    ARGV_NUM_5,
};

enum JS_ARGV_INDEX {
    ARGV_INDEX_0 = 0,
    ARGV_INDEX_1,
    ARGV_INDEX_2,
    ARGV_INDEX_3,
    ARGV_INDEX_4,
};

class NapiCommon {
public:
    static bool IsValidEvent(const std::string &eventInfo, int32_t &eventId);
    static napi_value CreateCodeMessage(napi_env env, const std::string &msg, int32_t code);
    static napi_value NapiValueByInt32(napi_env env, int32_t property);
    static void SetPropertyBool(napi_env env, napi_value object, const std::string &propertyName, bool property);
    static void SetPropertyInt32(napi_env env, napi_value object, const std::string &propertyName, int32_t property);
    static void SetPropertyInt64(napi_env env, napi_value object, const std::string &propertyName, int64_t property);
    static void SetPropertyUint32(napi_env env, napi_value object, const std::string &propertyName, uint32_t property);
    static void SetPropertyString(napi_env env, napi_value object, const std::string &propertyName,
                                  const std::string &property);
    static void GetPropertyString(napi_env env, napi_value object, const std::string &propertyName,
                                  std::string &property);
    static void GetPropertyInt32(napi_env env, napi_value object, const std::string &propertyName, int32_t &property);
    static void GetPropertyInt64(napi_env env, napi_value object, const std::string &propertyName, int64_t &property);
    static std::string GetNapiStringValue(napi_env env, napi_value napiValue, const std::string &name,
                                          const std::string &defValue = "");
    static std::string GetStringFromValue(napi_env env, napi_value value);
    static napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
    static int32_t GetNapiInt32Value(napi_env env, napi_value napiValue, const std::string &name,
                                     const int32_t &defValue = 0);
    static int64_t GetNapiInt64Value(napi_env env, napi_value napiValue, const std::string &name,
                                     const int64_t &defValue = 0);
    static bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType);
    static bool MatchParameters(napi_env env, const napi_value parameters[],
                                std::initializer_list<napi_valuetype> valueTypes);
    static napi_value CreateUndefined(napi_env env);
    static napi_value HandleAsyncWork(napi_env env, BaseContext *baseContext, const std::string &workName,
                                      napi_async_execute_callback execute, napi_async_complete_callback complete);
    static void Handle1ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue);
    static void Handle2ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue);
    static bool HasNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
    static bool MatchObjectProperty(napi_env env, napi_value object,
                                    std::initializer_list<std::pair<std::string, napi_valuetype>> pairList);
    static bool HasNamedTypeProperty(napi_env env, napi_value object, napi_valuetype type,
                                     const std::string &propertyName);
    static napi_value CreateEnumConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateObject(napi_env env);
    static napi_value CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NAPI_COMMON_H
