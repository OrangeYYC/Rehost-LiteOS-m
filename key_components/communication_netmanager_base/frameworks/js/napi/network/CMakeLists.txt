include_directories(network_module/include)
include_directories(async_work/include)
include_directories(async_context/include)
include_directories(options/include)
include_directories(observer/include)
include_directories(network_exec/include)
include_directories(constant/include)
include_directories(../../../../../../../base/telephony/core_service/interfaces/innerkits/include)

add_library(network SHARED
    network_module/src/network_module.cpp
    options/src/netconnection.cpp
    observer/src/net_conn_callback_observer.cpp
    async_context/src/gettype_context.cpp
    async_context/src/subscribe_context.cpp
    async_context/src/unsubscribe_context.cpp
    async_work/src/network_async_work.cpp
    network_exec/src/network_exec.cpp
    )