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

#ifndef COMMUNICATIONNETMANAGEREXT_MDNS_CONSTANT_H
#define COMMUNICATIONNETMANAGEREXT_MDNS_CONSTANT_H

#include <cstdint>

static constexpr int32_t SYSC_TIMEOUT = 5;

static constexpr int32_t PARAM_NONE = 0;
static constexpr int32_t PARAM_JUST_OPTIONS = 2;
static constexpr int32_t PARAM_OPTIONS_AND_CALLBACK = 3;

static constexpr const char *EVENT_SERVICESTART = "discoveryStart";
static constexpr const char *EVENT_SERVICESTOP = "discoveryStop";
static constexpr const char *EVENT_SERVICEFOUND = "serviceFound";
static constexpr const char *EVENT_SERVICELOST = "serviceLost";

static constexpr const char *ERRCODE = "errorCode";
static constexpr const char *RETMSG = "retMsg";
static constexpr const char *SERVICEINFO = "serviceInfo";
static constexpr const char *SERVICEINFO_ATTR = "serviceAttribute";
static constexpr const char *SERVICEINFO_ADDRESS = "address";
static constexpr const char *SERVICEINFO_TYPE = "serviceType";
static constexpr const char *SERVICEINFO_NAME = "serviceName";
static constexpr const char *SERVICEINFO_PORT = "port";
static constexpr const char *SERVICEINFO_HOST = "host";
static constexpr const char *SERVICEINFO_FAMILY = "family";

static constexpr const char *SERVICEINFO_ATTR_KEY = "key";
static constexpr const char *SERVICEINFO_ATTR_VALUE = "value";
static constexpr const char *CONTEXT_ATTR_APPINFO = "applicationInfo";
static constexpr const char *APPINFO_ATTR_NAME = "name";

static constexpr const char *MDNS_ERR = "MDNS_ERR";
static constexpr const char *INTERNAL_ERROR_NAME = "INTERNAL_ERROR";
static constexpr const char *ALREADY_ACTIVE_NAME = "ALREADY_ACTIVE";
static constexpr const char *MAX_LIMIT_NAME = "MAX_LIMIT";

enum {
    ARG_NUM_0,
    ARG_NUM_1,
    ARG_NUM_2,
};

enum MDnsErr {
    INTERNAL_ERROR,
    ALREADY_ACTIVE,
    MAX_LIMIT,
};
#endif /* COMMUNICATIONNETMANAGEREXT_MDNS_CONSTANT_H */
