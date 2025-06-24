cmake_minimum_required(VERSION 3.14)
project(latency_benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

#[[
add_executable(client
    src/server.cpp
    src/socket_wrapper.cpp
)

add_executable(server
    src/server.cpp
    src/socket_wrapper.cpp
)
]]#

add_executable(main src/main.cpp)
