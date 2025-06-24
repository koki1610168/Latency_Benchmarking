#pragma once
#include <string>
#include <unistd.h>


class SocketWrapper {
    public:
        SocketWrapper();
        ~SocketWrapper();

        // Server: binds to a port and starts listening for incoming connections
        void bindAndListen(int port);

        // Server: accepts an incoming client connection
        void acceptClient();

        // Client: connects to a remote server with IP and port
        void connectToServer(const std::string& ip, int port);

        // Send data
        ssize_t send(const void* buf, size_t len);

        // Receive data
        ssize_t receive(void *buf, size_t len);

    private:
        int sockfd_; // listening socket, it stays open
        int connfd_; // Connected socket, send/recv
        bool is_server_;

};