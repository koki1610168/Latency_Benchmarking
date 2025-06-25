#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>

int main() {
    SocketWrapper server(Protocol::UDP);
    server.bindAndListen(12345);

    
    char buffer[128];
    while (true) {
        ssize_t received = server.receive(buffer, sizeof(buffer));
        if (received <= 0) break;

        std::cout << "Received: " << std::string(buffer, received) << std::endl;

        server.send(buffer, received);
    }

    std::cout << "UDP server shutting down." << std::endl;

    return 0;
}