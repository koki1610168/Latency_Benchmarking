#include "socket_wrapper.hpp"
#include <iostream>
#include <cstring>

int main() {
    SocketWrapper server(Protocol::TCP);
    server.bindAndListen(12345);
    std::cout << "Server listening on port 12345" << std::endl;

    // std::cout << "Client connected" << std::endl;

    server.runEpollServerLoop();
    std::cout << "Server shutting down." << std::endl;
    return 0;

}