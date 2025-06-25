#include "socket_wrapper.hpp"
#include <iostream>
#include <chrono>
#include <cstring>

int main() {
    SocketWrapper client(Protocol::UDP);
    client.connectToServer("127.0.0.1", 12345);

    char send_buf[] = "ping";
    char recv_buf[128];

    using Clock = std::chrono::high_resolution_clock;

    // This makes the variable evaluated at compile-time
    constexpr int iterations = 1000;

    long long total_rtt_ns = 0;

    for (int i = 0; i < iterations; ++i) {
        auto t1 = Clock::now();

        client.send(send_buf, sizeof(send_buf));
        client.receive(recv_buf, sizeof(recv_buf));

        auto t2 = Clock::now();
        auto rtt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        total_rtt_ns += rtt;

        std::cout << "RTT: " << rtt << " ns"<< std::endl;
    }

    double average_rtt_ns = total_rtt_ns / 1000.0;
    std::cout << "Average RTT over " << iterations << " messages: " << average_rtt_ns << " ns" << std::endl;
    return 0;
}