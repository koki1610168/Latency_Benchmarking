cmake_minimum_required(VERSION 3.14)
project(latency_benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(tcp_example_client
    src/tcp_example_client.cpp
    src/socket_wrapper.cpp
)

add_executable(tcp_example_server
    src/tcp_example_server.cpp
    src/socket_wrapper.cpp
)

add_executable(tcp_latency_client
    src/tcp_latency_client.cpp
    src/socket_wrapper.cpp
)

add_executable(tcp_latency_server
    src/tcp_latency_server.cpp
    src/socket_wrapper.cpp
)

add_executable(udp_latency_client
    src/udp_latency_client.cpp
    src/socket_wrapper.cpp
)

add_executable(udp_latency_server
    src/udp_latency_server.cpp
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
