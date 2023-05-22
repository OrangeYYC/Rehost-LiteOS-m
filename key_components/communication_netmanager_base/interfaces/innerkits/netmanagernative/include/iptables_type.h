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

#ifndef NETMANAGER_BASE_IPTABLES_TYPE_H
#define NETMANAGER_BASE_IPTABLES_TYPE_H

namespace OHOS {
namespace NetManagerStandard {
enum FirewallRule {
    RULE_ALLOW = 1,
    RULE_DENY = 2,
};

enum FirewallType {
    TYPE_ALLOWED_LIST = 1,
    TYPE_DENIDE_LIST = 2,
};

enum ChainType {
    CHAIN_NONE = 0,
    CHAIN_INPUT,
    CHAIN_OUTPUT,
    CHAIN_FORWARD,
    CHAIN_OHBW_INPUT,
    CHAIN_OHBW_OUTPUT,
    CHAIN_OHBW_FORWARD,
    CHAIN_OHFW_INPUT,
    CHAIN_OHFW_OUTPUT,
    CHAIN_OHFW_FORWARD,
    CHAIN_OHTC_FORWARD,
    CHAIN_OHBW_GLOBAL_ALERT,
    CHAIN_OHBW_COSTLY_SHARED,
    CHAIN_OHBW_DENIED_LIST_BOX,
    CHAIN_OHBW_ALLOWED_LIST_BOX,
    CHAIN_OHBW_DATA_SAVER,
    CHAIN_OHFW_DOZABLE,
    CHAIN_OHFW_UNDOZABLE,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_BASE_IPTABLES_TYPE_H */
