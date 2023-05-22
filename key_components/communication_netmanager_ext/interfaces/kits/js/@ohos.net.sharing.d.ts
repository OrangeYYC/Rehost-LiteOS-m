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

import {AsyncCallback, Callback} from "./basic";
import {NetHandle} from "./@ohos.net.connection";

/**
 * Provides network sharing related interfaces.
 *
 * @since 9
 * @syscap SystemCapability.Communication.NetManager.NetSharing
 */
declare namespace sharing {
  /**
   * Checks whether this device allows for network sharing.
   *
   * @param callback Returns {@code true} indicating network sharing is supported; returns {@code false} otherwise.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function isSharingSupported(callback: AsyncCallback<boolean>): void;
  function isSharingSupported(): Promise<boolean>;

  /**
   * Return the global network sharing state.
   *
   * @param callback Returns {@code true} indicating network sharing is running; returns {@code false} otherwise.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function isSharing(callback: AsyncCallback<boolean>): void;
  function isSharing(): Promise<boolean>;

  /**
   * Start network sharing for given type.
   *
   * @param type Enumeration of shareable interface types.
   * @param callback Returns the result.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function startSharing(type: SharingIfaceType, callback: AsyncCallback<void>): void;
  function startSharing(type: SharingIfaceType): Promise<void>;

  /**
   * Stop network sharing for given type.
   *
   * @param type Enumeration of shareable interface types.
   * @param callback Returns the result.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function stopSharing(type: SharingIfaceType, callback: AsyncCallback<void>): void;
  function stopSharing(type: SharingIfaceType): Promise<void>;

  /**
   * Obtains the number of downlink data bytes of the sharing network interfaces.
   *
   * @param callback Returns the number of downlink data bytes of the sharing network interfaces.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getStatsRxBytes(callback: AsyncCallback<number>): void;
  function getStatsRxBytes(): Promise<number>;

  /**
   * Obtains the number of uplink data bytes of the sharing network interfaces.
   *
   * @param callback Returns the number of uplink data bytes of the sharing network interfaces.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getStatsTxBytes(callback: AsyncCallback<number>): void;
  function getStatsTxBytes(): Promise<number>;

  /**
   * Obtains the number of total data bytes of the sharing network interfaces.
   *
   * @param callback Returns the number of total data bytes of the sharing network interfaces.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getStatsTotalBytes(callback: AsyncCallback<number>): void;
  function getStatsTotalBytes(): Promise<number>;

  /**
   * Obtains the names of interfaces in each sharing state.
   *
   * @param state Is the network sharing state.
   * @param callback Returns an array of interface names that meet this status.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getSharingIfaces(state: SharingIfaceState, callback: AsyncCallback<Array<string>>): void;
  function getSharingIfaces(state: SharingIfaceState): Promise<Array<string>>;

  /**
   * Obtains the network sharing state for given type.
   *
   * @param type Is the enumeration of shareable interface types.
   * @param callback Returns {@code SharingIfaceState}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getSharingState(type: SharingIfaceType, callback: AsyncCallback<SharingIfaceState>): void;
  function getSharingState(type: SharingIfaceType): Promise<SharingIfaceState>;

  /**
   * Get a list regular expression that defines any interface that can support network sharing.
   *
   * @param type Is the enumeration of shareable interface types.
   * @param callback Returns an array of regular expression strings that define which interfaces
   *        are considered to support network sharing.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getSharableRegexes(type: SharingIfaceType, callback: AsyncCallback<Array<string>>): void;
  function getSharableRegexes(type: SharingIfaceType): Promise<Array<string>>;

  /**
   * Register a callback for the global network sharing state change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: 'sharingStateChange', callback: Callback<boolean>): void;

  /**
   * Unregister a callback for the global network sharing state change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: 'sharingStateChange', callback?: Callback<boolean>): void;

  /**
   * Register a callback for the interface network sharing state change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: 'interfaceSharingStateChange', callback: Callback<{ type: SharingIfaceType, iface: string, state: SharingIfaceState }>): void;

  /**
   * Unregister a callback for the interface network sharing state change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: 'interfaceSharingStateChange', callback?: Callback<{ type: SharingIfaceType, iface: string, state: SharingIfaceState }>): void;

  /**
   * Register a callback for the sharing upstream network change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: 'sharingUpstreamChange', callback: Callback<NetHandle>): void;

  /**
   * Unregister a callback for the sharing upstream network change.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function off(type: 'sharingUpstreamChange', callback?: Callback<NetHandle>): void;

  /**
   * @systemapi Hide this for inner system use.
   */
  export enum SharingIfaceState {
    /**
     * Indicates the names of the NICs that are serving as network sharing.
     */
    SHARING_NIC_SERVING = 1,

    /**
     * Indicates the names of the NICs that can serve as network sharing.
     */
    SHARING_NIC_CAN_SERVER = 2,

    /**
     * Indicates the names of the NICs that serving error.
     */
    SHARING_NIC_ERROR = 3
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export enum SharingIfaceType {
    /**
     * Network sharing type for Wi-Fi.
     */
    SHARING_WIFI = 0,

    /**
     * This type is not supported temporarily
     */
    SHARING_USB = 1,

    /**
     * Network sharing type for BLUETOOTH.
     */
    SHARING_BLUETOOTH = 2
  }
}

export default sharing;