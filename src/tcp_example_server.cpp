#include "socket_wrapper.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

int main() {
    try {
        SocketWrapper server(Protocol::TCP);
        server.bindAndListen(12345);
        std::cout << "Server listening on port 12345" << std::endl;

        server.runEpollServerLoop();
        // std::cout << "Client connected" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;

}