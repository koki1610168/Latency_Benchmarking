#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>

int main() {
    SocketWrapper server;
    server.bindAndListen(12345);
    // std::cout << "Server listening on port 12345" << std::endl;

    server.acceptClient();
    // std::cout << "Client connected" << std::endl;

    char buffer[128];
    while (true) {
        // If client disconets or send EOF, it n becomes 0 or -1    
        ssize_t n = server.receive(buffer, sizeof(buffer));
        if (n <= 0) break;
        server.send(buffer, n);
    }
    std::cout << "Server shutting down." << std::endl;
    return 0;

}