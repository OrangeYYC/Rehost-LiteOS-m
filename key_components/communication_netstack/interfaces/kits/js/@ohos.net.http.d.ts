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

import {AsyncCallback, Callback} from "./basic";
import connection from "./@ohos.net.connection";

/**
 * Provides http related APIs.
 *
 * @since 6
 * @syscap SystemCapability.Communication.NetStack
 */
declare namespace http {
  type HttpProxy = connection.HttpProxy;
  /**
   * Creates an HTTP request task.
   */
  function createHttp(): HttpRequest;

  export interface HttpRequestOptions {
    /**
     * Request method.
     */
    method?: RequestMethod; // default is GET

    /**
     * Additional data of the request.
     * extraData can be a string or an Object (API 6) or an ArrayBuffer(API 8).
     */
    extraData?: string | Object | ArrayBuffer;

    /**
     * Data type to be returned. If this parameter is set, the system preferentially returns the specified type.
     *
     * @since 9
     */
    expectDataType?: HttpDataType;

    /**
     * @since 9
     */
    usingCache?: boolean; // default is true

    /**
     * @since 9
     */
    priority?: number; // [1, 1000], default is 1.

    /**
     * HTTP request header.
     */
    header?: Object; // default is 'content-type': 'application/json'

    /**
     * Read timeout period. The default value is 60,000, in ms.
     */
    readTimeout?: number; // default is 60s

    /**
     * Connection timeout interval. The default value is 60,000, in ms.
     */
    connectTimeout?: number; // default is 60s.

    /**
     * @since 9
     */
    usingProtocol?: HttpProtocol; // default is automatically specified by the system.
    /**
     * If this parameter is set as type of boolean, the system will use default proxy or not use proxy.
     * If this parameter is set as type of HttpProxy, the system will use the specified HttpProxy.
     *
     * @since 10
     */
     usingProxy?: boolean | HttpProxy; // default is false.
  }

  export interface HttpRequest {
    /**
     * Initiates an HTTP request to a given URL.
     *
     * @param url URL for initiating an HTTP request.
     * @param options Optional parameters {@link HttpRequestOptions}.
     * @param callback Returns {@link HttpResponse}.
     * @permission ohos.permission.INTERNET
     */
    request(url: string, callback: AsyncCallback<HttpResponse>): void;
    request(url: string, options: HttpRequestOptions, callback: AsyncCallback<HttpResponse>): void;
    request(url: string, options?: HttpRequestOptions): Promise<HttpResponse>;

    /**
     * Destroys an HTTP request.
     */
    destroy(): void;

    /**
     * Registers an observer for HTTP Response Header events.
     *
     * @deprecated since 8
     * @useinstead on_headersReceive
     */
    on(type: "headerReceive", callback: AsyncCallback<Object>): void;

    /**
     * Unregisters the observer for HTTP Response Header events.
     *
     * @deprecated since 8
     * @useinstead off_headersReceive
     */
    off(type: "headerReceive", callback?: AsyncCallback<Object>): void;

    /**
     * Registers an observer for HTTP Response Header events.
     *
     * @since 8
     */
    on(type: "headersReceive", callback: Callback<Object>): void;

    /**
     * Unregisters the observer for HTTP Response Header events.
     *
     * @since 8
     */
    off(type: "headersReceive", callback?: Callback<Object>): void;

    /**
     * Registers a one-time observer for HTTP Response Header events.
     *
     * @since 8
     */
    once(type: "headersReceive", callback: Callback<Object>): void;
  }

  export enum RequestMethod {
    OPTIONS = "OPTIONS",
    GET = "GET",
    HEAD = "HEAD",
    POST = "POST",
    PUT = "PUT",
    DELETE = "DELETE",
    TRACE = "TRACE",
    CONNECT = "CONNECT"
  }

  export enum ResponseCode {
    OK = 200,
    CREATED,
    ACCEPTED,
    NOT_AUTHORITATIVE,
    NO_CONTENT,
    RESET,
    PARTIAL,
    MULT_CHOICE = 300,
    MOVED_PERM,
    MOVED_TEMP,
    SEE_OTHER,
    NOT_MODIFIED,
    USE_PROXY,
    BAD_REQUEST = 400,
    UNAUTHORIZED,
    PAYMENT_REQUIRED,
    FORBIDDEN,
    NOT_FOUND,
    BAD_METHOD,
    NOT_ACCEPTABLE,
    PROXY_AUTH,
    CLIENT_TIMEOUT,
    CONFLICT,
    GONE,
    LENGTH_REQUIRED,
    PRECON_FAILED,
    ENTITY_TOO_LARGE,
    REQ_TOO_LONG,
    UNSUPPORTED_TYPE,
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED,
    BAD_GATEWAY,
    UNAVAILABLE,
    GATEWAY_TIMEOUT,
    VERSION
  }

  /**
   * Supported protocols.
   *
   * @since 9
   */
  export enum HttpProtocol {
    HTTP1_1,
    HTTP2,
  }

  /**
   * Indicates the type of the returned data.
   *
   * @since 9
   */
  export enum HttpDataType {
    /**
     * The returned type is string.
     */
    STRING,
    /**
     * The returned type is Object.
     */
    OBJECT = 1,
    /**
     * The returned type is ArrayBuffer.
     */
    ARRAY_BUFFER = 2,
  }

  export interface HttpResponse {
    /**
     * result can be a string (API 6) or an ArrayBuffer(API 8). Object is deprecated from API 8.
     * If {@link HttpRequestOptions#expectDataType} is set, the system preferentially returns this parameter.
     */
    result: string | Object | ArrayBuffer;

    /**
     * If the resultType is string, you can get result directly.
     * If the resultType is Object, you can get result such as this: result['key'].
     * If the resultType is ArrayBuffer, you can use ArrayBuffer to create the binary objects.
     *
     * @since 9
     */
    resultType: HttpDataType;

    /**
     * Server status code.
     */
    responseCode: ResponseCode | number;

    /**
     * All headers in the response from the server.
     */
    header: Object;

    /**
     * @since 8
     */
    cookies: string;
  }

  /**
   * Creates a default {@code HttpResponseCache} object to store the responses of HTTP access requests.
   *
   * @param cacheSize the size of cache, default is 10*1024*1024(10MB).
   * @since 9
   */
  function createHttpResponseCache(cacheSize?: number): HttpResponseCache;

  /**
   * @since 9
   */
  export interface HttpResponseCache {
    /**
     * Writes data in the cache to the file system so that all the cached data can be accessed in the next HTTP request.
     */
    flush(callback: AsyncCallback<void>): void;
    flush(): Promise<void>;

    /**
     * Disables a cache and deletes the data in it.
     */
    delete(callback: AsyncCallback<void>): void;
    delete(): Promise<void>;
  }
}

export default http;