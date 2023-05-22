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

import {AsyncCallback, Callback} from "./basic";
import connection from "./@ohos.net.connection";
import Context from "./application/BaseContext";

/**
 * Provides interfaces to discover DNS based services on a local network over Multicast DNS.
 *
 * @since 9
 * @syscap SystemCapability.Communication.NetManager.MDNS
 */
declare namespace mdns {
  type NetAddress = connection.NetAddress;
   bundleName = Context.getBundleName()
  /**
   * Adds an mDNS service.
   *
   * @param context Indicates the context of application or capability.
   * @param serviceInfo Information about the mDNS service. {@link LocalServiceInfo}
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} 2100002 - Operation failed. Cannot connect to service.
   * @throws {BusinessError} 2100003 - System internal error.
   * @throws {BusinessError} 2204003 - Callback duplicated.
   * @throws {BusinessError} 2204008 - Service instance duplicated.
   * @throws {BusinessError} 2204010 - Send packet failed.
  */
  function addLocalService(context: bundleName, serviceInfo: LocalServiceInfo,
    callback: AsyncCallback<LocalServiceInfo>): void;
  function addLocalService(context: bundleName, serviceInfo: LocalServiceInfo): Promise<LocalServiceInfo>;

  /**
   * Removes an mDNS service.
   *
   * @param context Indicates the context of application or capability.
   * @param serviceInfo Information about the mDNS service. {@link LocalServiceInfo}
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} 2100002 - Operation failed. Cannot connect to service.
   * @throws {BusinessError} 2100003 - System internal error.
   * @throws {BusinessError} 2204002 - Callback not found.
   * @throws {BusinessError} 2204008 - Service instance not found.
   * @throws {BusinessError} 2204010 - Send packet failed.
  */
  function removeLocalService(context: bundleName, serviceInfo: LocalServiceInfo,
    callback: AsyncCallback<LocalServiceInfo>): void;
  function removeLocalService(context: bundleName, serviceInfo: LocalServiceInfo): Promise<LocalServiceInfo>;

  /**
   * Create an mDNS based discovery service with context and serviceType.
   *
   * @param serviceType The service type being discovered.
   * @throws {BusinessError} 401 - Parameter error.
  */
  function createDiscoveryService(context: bundleName, serviceType: string): DiscoveryService;

  /**
   * Resolves an mDNS service.
   *
   * @param context Indicates the context of application or capability.
   * @param serviceInfo Information about the mDNS service. {@link LocalServiceInfo}
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} 2100002 - Operation failed. Cannot connect to service.
   * @throws {BusinessError} 2100003 - System internal error.
   * @throws {BusinessError} 2204003 - Callback duplicated.
   * @throws {BusinessError} 2204006 - Request timeout.
   * @throws {BusinessError} 2204010 - Send packet failed.
  */
  function resolveLocalService(context: bundleName, serviceInfo: LocalServiceInfo,
    callback: AsyncCallback<LocalServiceInfo>): void;
  function resolveLocalService(context: bundleName, serviceInfo: LocalServiceInfo): Promise<LocalServiceInfo>;

  export interface DiscoveryService {
    /**
     * Enables listening for discoveryStart events of mDNS services.
     */
    on(type: 'discoveryStart',
      callback: Callback<{serviceInfo: LocalServiceInfo, errorCode?: MDNS_ERR}>): void;

    /**
     * Enables listening for discoveryStop events of mDNS services.
     */
    on(type: 'discoveryStop',
      callback: Callback<{serviceInfo: LocalServiceInfo, errorCode?: MDNS_ERR}>): void;

    /**
     * Enables listening for serviceFound events of mDNS services.
     */
    on(type: 'serviceFound', callback: Callback<LocalServiceInfo>): void;

    /**
     * Enables listening for serviceLost events of mDNS services.
     */
    on(type: 'serviceLost', callback: Callback<LocalServiceInfo>): void;

    /**
     * Starts searching for mDNS services on the LAN.
     */
    startSearchingMDNS(): void;

    /**
     * Stops searching for mDNS services on the LAN.
     */
    stopSearchingMDNS(): void;
  }

  export interface LocalServiceInfo {
    /**
     * Service type. Use an underscore (_) as the prefix, for example, _http._tcp.
     */
    serviceType: string;
    /**
     * Service name.
     */
    serviceName: string;
    /**
     * Port number.
     */
    port?: number;
    /**
     * IP address of the host.
     */
    host?: NetAddress;
    /**
     * DNS-SD TXT record pairs.
     */
    serviceAttribute?: Array<ServiceAttribute>;
  }

  export interface ServiceAttribute {
    /**
     * TXT record key.
     */
    key: string;
    /**
     * TXT record value.
     */
    value: Array<number>;
  }

  export enum MDNS_ERR {
    /**
     * Indicates that the operation failed due to   internal error.
     */
    INTERNAL_ERROR = 0,

    /**
     * Indicates that the operation failed because it is already active.
     */
    ALREADY_ACTIVE = 1,

    /**
     * Indicates that the operation failed because the maximum outstanding
     * requests from the applications have reached.
     */
    MAX_LIMIT = 2
  }
}

export default mdns;