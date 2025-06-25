#include "socket_wrapper.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>

SocketWrapper::SocketWrapper(Protocol protocol) : sockfd_(-1), connfd_(-1), is_server_(false), protocol_(protocol) {}

SocketWrapper::~SocketWrapper() {
    if (connfd_ != -1) close (connfd_);
    if (sockfd_ != -1) close (sockfd_);
}

void SocketWrapper::bindAndListen(int port) {
    is_server_ = true;
    //SOCK_STREAM for TCP, SOCK_DGRAM for UDP
    sockfd_ = socket(AF_INET, protocol_ == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (sockfd_ < 0) throw std::runtime_error("Socket creation failed");

    // bind and listen are not needed for UDP since the server does not
    // need to ensure connection with clients
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int opt = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("setstocket opt(SO_REUSEADDR) failed");
    }

    // Assigns a local address (IP + port) to a socket file descriptor
    if (bind(sockfd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error(std::string("Bind failed: ") + std::strerror(errno));
    }

    if (protocol_ == Protocol::TCP && listen(sockfd_, 1) < 0) {
        throw std::runtime_error("Listen failed");
    }
}

// wait for a client to connect
void SocketWrapper::acceptClient() {
    if (protocol_ == Protocol::UDP) return;

    sockaddr_in client_addr{};
    socklen_t len = sizeof(client_addr);
    connfd_ = accept(sockfd_, (sockaddr*)&client_addr, &len);
    if (connfd_ < 0) {
        throw std::runtime_error("Accept failed");
    }
}

void SocketWrapper::connectToServer(const std::string& ip, int port) {
    is_server_ = false;
    sockfd_ = socket(AF_INET, protocol_ == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(sockfd_, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error(std::string("Connection failed") + std::strerror(errno));
    }
}

ssize_t SocketWrapper::send(const void* buf, size_t len) {
    if (protocol_ == Protocol::UDP && is_server_) {
        return sendto(sockfd_, buf, len, 0, (sockaddr*)&last_sender_, sizeof(last_sender_));
    }
    return ::send((is_server_ && protocol_ == Protocol::TCP) ? connfd_ : sockfd_, buf, len, 0);
}

ssize_t SocketWrapper::receive(void* buf, size_t len) {
    if (protocol_ == Protocol::UDP && is_server_) {
        socklen_t addr_len = sizeof(last_sender_);
        return recvfrom(sockfd_, buf, len, 0, (sockaddr*)&last_sender_, &addr_len);
    }
    return ::recv((is_server_ && protocol_ == Protocol::TCP) ? connfd_ : sockfd_, buf, len, 0);
}