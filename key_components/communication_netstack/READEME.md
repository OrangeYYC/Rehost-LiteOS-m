# Network Stack Component<a name="EN-US_TOPIC_0000001125689015"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Available APIs](#section1096322014288)
-   [Repositories Involved](#section11683135113011)

## Introduction<a name="section11660541593"></a>

The **network stack component** is an adaptation layer framework for developers to develop network protocol stacks such as HTTP, socket and websocket for OpenHarmony applications. At present, it mainly consists of the following two parts:

1. NAPI based JS adaptation layer on mini system and small system.


2. JSI based JS adaptation layer on standard system.

## Directory Structure<a name="section1464106163817"></a>

The source code of the network stack component is stored in **/foundation/communication/netstack**. The directory structure is as follows:

```
/foundation/communication/netstack
├── frameworks         # Framework code
│   ├── js             # JS adaptation
│       ├── builtin    # JSI based JS adaptation layer on mini system and small system
│       └── napi       # NAPI based JS adaptation layer on standard system
├── interfaces         # APIs exposed externally
│   └── kits           # OpenHarmony SDK API, including Java, JS and native. At present, there is only JS
│       └── js         # JS API
├── utils              # Common tools
│   └── log            # Log tool
```

## Available APIs<a name="section1096322014288"></a>

```
export interface FetchResponse {
  /**
   * Server status code.
   * @since 3
   */
  code: number;

  /**
   * Data returned by the success function.
   * @since 3
   */
  data: string | object;

  /**
   * All headers in the response from the server.
   * @since 3
   */
  headers: Object;
}

/**
 * @Syscap SysCap.ACE.UIEngine
 */
export default class Fetch {
  /**
   * Obtains data through the network.
   * @param options
   */
  static fetch(options: {
    /**
     * Resource URL.
     * @since 3
     */
    url: string;

    /**
     * Request parameter, which can be of the string type or a JSON object.
     * @since 3
     */
    data?: string | object;

    /**
     * Request header, which accommodates all attributes of the request.
     * @since 3
     */
    header?: Object;

    /**
     * Request methods available: OPTIONS, GET, HEAD, POST, PUT, DELETE and TRACE. The default value is GET.
     * @since 3
     */
    method?: string;

    /**
     * The return type can be text, or JSON. By default, the return type is determined based on Content-Type in the header returned by the server.
     * @since 3
     */
    responseType?: string;

    /**
     * Called when the network data is obtained successfully.
     * @since 3
     */
    success?: (data: FetchResponse) => void;

    /**
     * Called when the network data fails to be obtained.
     * @since 3
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     * @since 3
     */
    complete?: () => void;
  }): void;
}
```

## Repositories Involved<a name="section11683135113011"></a>

[ ace_engine_lite ](https://gitee.com/openharmony/ace_engine_lite)

[ third_party_curl ](https://gitee.com/openharmony/third_party_curl)

[ third_party_mbedtls ](https://gitee.com/openharmony/third_party_mbedtls)
