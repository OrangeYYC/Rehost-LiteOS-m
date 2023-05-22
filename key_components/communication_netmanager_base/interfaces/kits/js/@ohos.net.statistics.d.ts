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

/**
 * Obtains traffic statistics.
 *
 * @since 10
 * @syscap SystemCapability.Communication.NetManager.Core
 */
declare namespace statistics {
  /**
   * Queries the data traffic (including all TCP and UDP data packets) received through a specified NIC.
   *
   * @param nic Indicates the NIC name.
   * @param callback Returns the data traffic received through the specified NIC.
   */
  function getIfaceRxBytes(nic: string, callback: AsyncCallback<number>): void;
  function getIfaceRxBytes(nic: string): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) sent through a specified NIC.
   *
   * @param nic Indicates the NIC name.
   * @param callback Returns the data traffic sent through the specified NIC.
   */
  function getIfaceTxBytes(nic: string, callback: AsyncCallback<number>): void;
  function getIfaceTxBytes(nic: string): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) received through the cellular network.
   *
   * @param callback Returns the data traffic received through the cellular network.
   */
  function getCellularRxBytes(callback: AsyncCallback<number>): void;
  function getCellularRxBytes(): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) sent through the cellular network.
   *
   * @param callback Returns the data traffic sent through the cellular network.
   */
  function getCellularTxBytes(callback: AsyncCallback<number>): void;
  function getCellularTxBytes(): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) sent through all NICs.
   *
   * @param callback Returns the data traffic sent through all NICs.
   */
  function getAllTxBytes(callback: AsyncCallback<number>): void;
  function getAllTxBytes(): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) received through all NICs.
   *
   * @param callback Returns the data traffic received through all NICs.
   */
  function getAllRxBytes(callback: AsyncCallback<number>): void;
  function getAllRxBytes(): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) received by a specified application.
   * This method applies only to system applications and your own applications.
   *
   * @param uid Indicates the process ID of the application.
   * @param callback Returns the data traffic received by the specified application.
   */
  function getUidRxBytes(uid: number, callback: AsyncCallback<number>): void;
  function getUidRxBytes(uid: number): Promise<number>;

  /**
   * Queries the data traffic (including all TCP and UDP data packets) sent by a specified application.
   * This method applies only to system applications and your own applications.
   *
   * @param uid Indicates the process ID of the application.
   * @param callback Returns the data traffic sent by the specified application.
   */
  function getUidTxBytes(uid: number, callback: AsyncCallback<number>): void;
  function getUidTxBytes(uid: number): Promise<number>;

  /**
   * Register notifications of network traffic updates.
   *
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function on(type: 'netStatsChange', callback: Callback<{ iface: string, uid?: number }>): void;

  /**
   * @systemapi Hide this for inner system use.
   */
  function off(type: 'netStatsChange', callback?: Callback<{ iface: string, uid?: number }>): void;

  /**
   * Get the traffic usage details of the network interface in the specified time period.
   *
   * @param ifaceInfo Indicates the handle. See {@link IfaceInfo}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getIfaceStats(ifaceInfo: IfaceInfo, callback: AsyncCallback<NetStatsInfo>): void;
  function getIfaceStats(ifaceInfo: IfaceInfo): Promise<NetStatsInfo>;

  /**
   * Get the traffic usage details of the specified time period of the application.
   *
   * @param uidStatsInfo Indicates the handle. See {@link UidStatsInfo}.
   * @permission ohos.permission.CONNECTIVITY_INTERNAL
   * @systemapi Hide this for inner system use.
   */
  function getIfaceUidStats(uidStatsInfo: UidStatsInfo, callback: AsyncCallback<NetStatsInfo>): void;
  function getIfaceUidStats(uidStatsInfo: UidStatsInfo): Promise<NetStatsInfo>;

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface IfaceInfo {
    iface: string;
    startTime: number;
    endTime: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface UidStatsInfo {
    /*See {@link IfaceInfo}*/
    ifaceInfo: IfaceInfo;
    uid: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface NetStatsInfo {
    rxBytes: number;
    txBytes: number;
    rxPackets: number;
    txPackets: number;
  }
}

export default statistics;