cmake_minimum_required(VERSION 3.14)
project(latency_benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(tcp_client
    src/tcp_client.cpp
    src/socket_wrapper.cpp
)

add_executable(tcp_server
    src/tcp_server.cpp
    src/socket_wrapper.cpp
)

add_executable(udp_client
    src/udp_client.cpp
    src/socket_wrapper.cpp
)

add_executable(udp_server
    src/udp_server.cpp
    src/socket_wrapper.cpp
)

# Add Google Test
add_subdirectory(external/googletest)
enable_testing()

add_executable(tests
    tests/test_socket_wrapper.cpp
    src/socket_wrapper.cpp
)

target_link_libraries(tests
    gtest
    gtest_main
)

add_test(NAME socket_wrapper_tests COMMAND tests)
