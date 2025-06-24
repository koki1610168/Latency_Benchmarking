#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>
#include <chrono>

int main() {
    SocketWrapper client;
    // connect to the server 127.0.0.1::12345
    client.connectToServer("127.0.0.1", 12345);

    char send_buf[16] = "ping";
    char recv_buf[16];

    using Clock = std::chrono::high_resolution_clock;
    long long total_rtt_ns = 0;

    const int kIterations = 1000;

    for (int i = 0; i < kIterations; ++i) {
        auto t1 = Clock::now();
        client.send(send_buf, 4);
        client.receive(recv_buf, sizeof(recv_buf));
        auto t2 = Clock::now();

        auto rtt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        total_rtt_ns += rtt;
        std::cout << "RTT: " << rtt << " ns"<< std::endl;
    }

    double average_rtt_ns = total_rtt_ns / 1000.0;
    std::cout << "Average RTT over " << kIterations << " messages: " << average_rtt_ns << " ns" << std::endl;
    return 0;
}