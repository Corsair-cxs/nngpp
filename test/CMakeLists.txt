find_package(Catch2)

set(test_sources
    bus.cpp
    device.cpp
    hello_world.cpp
    http_client.cpp
    ipcsupp.cpp
    main.cpp
    message.cpp
    pipe.cpp
    reconnect.cpp
    respondpoll.cpp
    scalability.cpp
    synch.cpp
    tcpsupp.cpp
)
# 检查系统名称
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    message(STATUS "Compiling on macOS")
    # 检查处理器架构
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
        message(STATUS "Architecture: ARM64")
        # 针对 ARM64 架构的设置
        include_directories(/opt/homebrew/include)
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        message(STATUS "Architecture: x86_64")
        # 针对 x86_64 架构的设置
        include_directories(/usr/local/include)
    endif()
endif()

if (NNGPP_BUILD_TLS_TEST)
    list(APPEND test_sources tls.cpp)
endif()

add_executable(tests ${test_sources})
target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/include)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_include_directories(tests PRIVATE /opt/homebrew/include)
endif()

target_link_libraries(tests nngpp Catch2::Catch2)
