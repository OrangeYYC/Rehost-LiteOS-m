/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from "./basic";
import connection from "./@ohos.net.connection";

/**
 * Provides interfaces to manage network policy rules.
 *
 * @since 9
 * @syscap SystemCapability.Communication.NetManager.Core
 */
declare namespace policy {
  type NetBearType = connection.NetBearType;

  /**
   * Control if applications can use data on background.
   *
   * @param isAllowed Allow applications to use data on background.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function setBackgroundAllowed(isAllowed: boolean, callback: AsyncCallback<void>): void;
  function setBackgroundAllowed(isAllowed: boolean): Promise<void>;

  /**
   * Get the status if applications can use data on background.
   *
   * @param callback Returns that it's allowed or not to use data on background.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function isBackgroundAllowed(callback: AsyncCallback<boolean>): void;
  function isBackgroundAllowed(): Promise<boolean>;

  /**
   * Set the policy for the specified UID.
   *
   * @param uid the specified UID of application.
   * @param policy the policy of the current UID of application.
   *      For details, see {@link NetUidPolicy}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function setPolicyByUid(uid: number, policy: NetUidPolicy, callback: AsyncCallback<void>): void;
  function setPolicyByUid(uid: number, policy: NetUidPolicy): Promise<void>;

  /**
   * Query the policy of the specified UID.
   *
   * @param uid the specified UID of application.
   * @param callback Returns the policy of the current UID of application.
   *      For details, see {@link NetUidPolicy}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getPolicyByUid(uid: number, callback: AsyncCallback<NetUidPolicy>): void;
  function getPolicyByUid(uid: number): Promise<NetUidPolicy>;

  /**
   * Query the application UIDs of the specified policy.
   *
   * @param policy the policy of the current UID of application.
   *      For details, see {@link NetUidPolicy}.
   * @param callback Returns the UIDs of the specified policy.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getUidsByPolicy(policy: NetUidPolicy, callback: AsyncCallback<Array<number>>): void;
  function getUidsByPolicy(policy: NetUidPolicy): Promise<Array<number>>;

  /**
   * Get network policies.
   *
   * @return See {@link NetQuotaPolicy}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getNetQuotaPolicies(callback: AsyncCallback<Array<NetQuotaPolicy>>): void;
  function getNetQuotaPolicies(): Promise<Array<NetQuotaPolicy>>;

  /**
   * Set network policies.
   *
   * @param quotaPolicies Indicates {@link NetQuotaPolicy}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function setNetQuotaPolicies(quotaPolicies: Array<NetQuotaPolicy>, callback: AsyncCallback<void>): void;
  function setNetQuotaPolicies(quotaPolicies: Array<NetQuotaPolicy>): Promise<void>;

  /**
   * Reset the specified network management policy.
   *
   * @param iccid Indicates the specified sim that is valid when netType is cellular.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function restoreAllPolicies(iccid: string, callback: AsyncCallback<void>): void;
  function restoreAllPolicies(iccid: string): Promise<void>;

  /**
   * Get the status whether the specified uid app can access the metered network or non-metered network.
   *
   * @param uid The specified UID of application.
   * @param isMetered Indicates meterd network or non-metered network.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function isUidNetAllowed(uid: number, isMetered: boolean, callback: AsyncCallback<boolean>): void;
  function isUidNetAllowed(uid: number, isMetered: boolean): Promise<boolean>;

  /**
   * Get the status whether the specified uid app can access the specified iface network.
   *
   * @param uid The specified UID of application.
   * @param iface Iface name.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function isUidNetAllowed(uid: number, iface: string, callback: AsyncCallback<boolean>): void;
  function isUidNetAllowed(uid: number, iface: string): Promise<boolean>;

  /**
   * Set the UID into device idle allow list.
   *
   * @param uid The specified UID of application.
   * @param isAllowed The UID is into allow list or not.
   * @param callback
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function setDeviceIdleAllowList(uid: number, isAllowed: boolean, callback: AsyncCallback<void>): void;
  function setDeviceIdleAllowList(uid: number, isAllowed: boolean): Promise<void>;

  /**
   * Get the allow list of in device idle mode.
   *
   * @param callback Returns the list of UIDs
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getDeviceIdleAllowList(callback: AsyncCallback<Array<number>>): void;
  function getDeviceIdleAllowList(): Promise<Array<number>>;

  /**
   * Get the background network restriction policy for the specified uid.
   *
   * @param uid The specified UID of application.
   * @param callback {@link NetBackgroundPolicy}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getBackgroundPolicyByUid(uid: number, callback: AsyncCallback<NetBackgroundPolicy>): void;
  function getBackgroundPolicyByUid(uid: number): Promise<NetBackgroundPolicy>;

  /**
   * Reset network policies\rules\quota policies\firewall rules.
   *
   * @param iccid Specify the matched iccid of quota policy.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function resetPolicies(iccid: string, callback: AsyncCallback<void>): void;
  function resetPolicies(iccid: string): Promise<void>;

  /**
   * Update the limit or warning remind time of quota policy.
   *
   * @param iccid Specify the matched iccid of quota policy when netType is cellular.
   * @param netType {@link NetBearType}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function updateRemindPolicy(netType: NetBearType, iccid: string, remindType: RemindType, callback: AsyncCallback<void>): void;
  function updateRemindPolicy(netType: NetBearType, iccid: string, remindType: RemindType): Promise<void>;

  /**
   * Register uid policy change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: "netUidPolicyChange", callback: Callback<{ uid: number, policy: NetUidPolicy }>): void;

  /**
   * Unregister uid policy change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: "netUidPolicyChange", callback?: Callback<{ uid: number, policy: NetUidPolicy }>): void;

  /**
   * Register uid rule change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: "netUidRuleChange", callback: Callback<{ uid: number, rule: NetUidRule }>): void;

  /**
   * Unregister uid rule change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: "netUidRuleChange", callback?: Callback<{ uid: number, rule: NetUidRule }>): void;

  /**
   * Register metered ifaces change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: "netMeteredIfacesChange", callback: Callback<Array<string>>): void;

  /**
   * Unregister metered ifaces change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: "netMeteredIfacesChange", callback?: Callback<Array<string>>): void;

  /**
   * Register quota policies change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: "netQuotaPolicyChange", callback: Callback<Array<NetQuotaPolicy>>): void;

  /**
   * Unregister quota policies change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: "netQuotaPolicyChange", callback?: Callback<Array<NetQuotaPolicy>>): void;

  /**
   * Register network background policy change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: "netBackgroundPolicyChange", callback: Callback<boolean>): void;

  /**
   * Unregister network background policy change listener.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: "netBackgroundPolicyChange", callback?: Callback<boolean>): void;

  /**
   * Indicate whether the application can use metered networks in background.
   *
   * @systemapi Hide this for inner system use.
   */
  export enum NetBackgroundPolicy {
    /* Default value. */
    NET_BACKGROUND_POLICY_NONE = 0,
    /* Apps can use metered networks on background. */
    NET_BACKGROUND_POLICY_ENABLE = 1,
    /* Apps can't use metered networks on background. */
    NET_BACKGROUND_POLICY_DISABLE = 2,
    /* Only apps in allow list can use metered networks on background. */
    NET_BACKGROUND_POLICY_ALLOW_LIST = 3,
  }

  /**
   * Policy for net quota, includes usage period, limit and warning actions.
   *
   * @systemapi Hide this for inner system use.
   */
  export interface NetQuotaPolicy {
    /* netType see {@link NetBearType}. */
    netType: NetBearType;
    /* The ID of the target card, valid when netType is BEARER_CELLULAR. */
    iccid: string;
    /* To specify the identity of network, such as different WLAN. */
    ident: string;
    /* The period and the start time for quota policy, defalt: "M1". */
    periodDuration: string;
    /* The warning threshold of traffic, default:  DATA_USAGE_UNKNOWN. */
    warningBytes: number;
    /* The limit threshold of traffic, default: DATA_USAGE_UNKNOWN. */
    limitBytes: number;
    /* The updated wall time that last warning remind, default: REMIND_NEVER. */
    lastWarningRemind: number;
    /* The updated wall time that last limit remind, default: REMIND_NEVER. */
    lastLimitRemind: number;
    /* Is meterd network or not. */
    metered: boolean;
    /* The action while the used bytes reach the limit, see {@link LimitAction}. */
    limitAction: LimitAction;
  }

  /**
   * The action when quota policy hit the limit.
   *
   * @systemapi Hide this for inner system use.
   */
  export enum LimitAction {
    /* Default action, do nothing. */
    LIMIT_ACTION_NONE = -1,
    /* Access is disabled, when quota policy hit the limit. */
    LIMIT_ACTION_DISABLE = 0,
    /* The user is billed automatically, when quota policy hit the limit. */
    LIMIT_ACTION_AUTO_BILL = 1,
  }

  /**
   * Rules whether an uid can access to a metered or non-metered network.
   *
   * @systemapi Hide this for inner system use.
   */
  export enum NetUidRule {
    /* Default uid rule. */
    NET_RULE_NONE = 0,
    /* Allow traffic on metered networks while app is foreground. */
    NET_RULE_ALLOW_METERED_FOREGROUND = 1 << 0,
    /* Allow traffic on metered network. */
    NET_RULE_ALLOW_METERED = 1 << 1,
    /* Reject traffic on metered network. */
    NET_RULE_REJECT_METERED = 1 << 2,
    /* Allow traffic on all network (metered or non-metered). */
    NET_RULE_ALLOW_ALL = 1 << 5,
    /* Reject traffic on all network. */
    NET_RULE_REJECT_ALL = 1 << 6,
  }

  /**
   * Specify the remind type, see {@link updateRemindPolicy}.
   *
   * @systemapi Hide this for inner system use.
   */
  export enum RemindType {
    /* Warning remind. */
    REMIND_TYPE_WARNING = 1,
    /* Limit remind. */
    REMIND_TYPE_LIMIT = 2,
  }

  /**
   * Network policy for uid.
   *
   * @systemapi Hide this for inner system use.
   */
  export enum NetUidPolicy {
    /* Default net policy. */
    NET_POLICY_NONE = 0,
    /* Reject on metered networks when app in background. */
    NET_POLICY_ALLOW_METERED_BACKGROUND = 1 << 0,
    /* Allow on metered networks when app in background. */
    NET_POLICY_REJECT_METERED_BACKGROUND = 1 << 1,
  }
}

export default policy;