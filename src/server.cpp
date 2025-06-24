#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>

int main() {
    try {
        SocketWrapper server;
        server.bindAndListen(12345);
        std::cout << "Server listening on port 12345" << std::endl;

        server.acceptClient();
        std::cout << "Client connected" << std::endl;

        char buffer[128] = {};
        server.receive(buffer, sizeof(buffer));

        std::cout << "Received from client: " << buffer << std::endl;

        const char* message = "I received your message, Koki";
        server.send(message, std::strlen(message));
        std::cout << "Echoed message back to client" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
    }
}