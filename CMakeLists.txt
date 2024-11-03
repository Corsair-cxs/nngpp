cmake_minimum_required(VERSION 3.5)  # 更新版本以避免弃用警告
project(nngpp CXX)

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.1  # 替换为你需要的版本
)

FetchContent_MakeAvailable(Catch2)

option(NNGPP_BUILD_DEMOS "build nngpp demos" ON)
option(NNGPP_BUILD_TESTS "build nngpp tests" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nng REQUIRED)

# 查找所有的源文件
file(GLOB_RECURSE NNGPP_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 添加静态库目标
add_library(nngpp STATIC ${NNGPP_SOURCES})
target_include_directories(nngpp INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(nngpp PRIVATE ${CMAKE_SOURCE_DIR}/lib/libnng.a)

if (NNGPP_BUILD_DEMOS)
    add_subdirectory(demo)
endif()

if (NNGPP_BUILD_TESTS)
    option(NNGPP_BUILD_TLS_TEST "build nngpp tls tests (requires mbedtls)" ON)
    add_subdirectory(test)
endif()

install(TARGETS nngpp EXPORT nngpp-target)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT nngpp-target DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nngpp
        NAMESPACE nng:: FILE nngpp-config.cmake)