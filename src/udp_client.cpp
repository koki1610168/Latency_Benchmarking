#include "socket_wrapper.hpp"
#include <iostream>
#include <chrono>
#include <cstring>

int main() {
    try {
        SocketWrapper client(Protocol::UDP);
        client.connectToServer("127.0.0.1", 12345);

        const char* msg = "Hello from UDP client";
        client.send(msg, std::strlen(msg));

        char buffer[128];
        ssize_t received = client.receive(buffer, sizeof(buffer));
        if (received > 0) {
            std::cout << "Received from server: " << std::string(buffer, received) << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "UDP client error: " << ex.what() << std::endl;
    }
    return 0;
}