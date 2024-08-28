cmake_minimum_required (VERSION 3.1)
project(nngpp_demo CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
find_package(nng)

include_directories(/usr/local/include)

add_executable(async_server async/server.cpp)
target_link_libraries(async_server nng)
target_include_directories(async_server PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(async_client async/client.cpp)
target_link_libraries(async_client nng)
target_include_directories(async_client PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(http_client http_client/http_client.cpp)
target_link_libraries(http_client nng)
target_include_directories(http_client PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(raw raw/raw.cpp)
target_link_libraries(raw nng)
target_include_directories(raw PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(reqrep reqrep/reqrep.cpp)
target_link_libraries(reqrep nng)
target_include_directories(reqrep PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(rest rest/server.cpp)
target_link_libraries(rest nng)
target_include_directories(rest PUBLIC ${CMAKE_SOURCE_DIR}/include)

