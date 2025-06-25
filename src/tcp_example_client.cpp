#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>

int main() {
    try {
        SocketWrapper client(Protocol::TCP);
        client.connectToServer("127.0.0.1", 12345);

        const char* msg = "Hello from Koki client";
        client.send(msg, std::strlen(msg));

        char buffer[512] = {};
        ssize_t n = client.receive(buffer, sizeof(buffer));
        if (n > 0) {
            std::cout << "Received echo message from server: " << std::string(buffer, n) << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}