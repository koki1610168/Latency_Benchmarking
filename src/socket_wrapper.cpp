#include "socket_wrapper.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

// max events that epoll will listen to
constexpr int MAX_EVENTS = 64;

SocketWrapper::SocketWrapper(Protocol protocol) : sockfd_(-1), connfd_(-1), is_server_(false), protocol_(protocol) {}

SocketWrapper::~SocketWrapper() {
    if (connfd_ != -1) close (connfd_);
    if (sockfd_ != -1) close (sockfd_);
    if (epoll_fd_ != -1) close (epoll_fd_);
}

void SocketWrapper::makeSocketNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to make socket non-blocking");
    }
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

    if (protocol_ == Protocol::TCP) {
        makeSocketNonBlocking(sockfd_);

        if (listen(sockfd_, SOMAXCONN) < 0) {
            throw std::runtime_error("listen failed");
        }

        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ == -1) {
            throw std::runtime_error("Failed to create epoll isntance");
        }

        epoll_event ev{};
        ev.data.fd = sockfd_;
        // This specifies that we want to know when the socket is ready to read
        ev.events = EPOLLIN;
        // Registers the listening socket fd with the epoll instance
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sockfd_, &ev) == -1) {
            throw std::runtime_error("Failed to add listen socket to epoll");
        }
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

void SocketWrapper::runEpollServerLoop() {
    if (protocol_ != Protocol::TCP || epoll_fd_ == -1) {
        throw std::runtime_error("Epoll is only supported for TCP");
    }

    epoll_event events[MAX_EVENTS];

    while (true) {
        int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == sockfd_) {
                // Accept new connection, create a client socket
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(sockfd_, (sockaddr*)&client_addr, &client_len);
                if (client_fd < 0) continue;

                // Print new client connection info
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
                std::cout << "New client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

                makeSocketNonBlocking(client_fd);
                
                // Add the newly created socket to epoll watchlist
                epoll_event client_ev{};
                client_ev.data.fd = client_fd;
                client_ev.events = EPOLLIN;
                epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &client_ev);
            } else {
                // Handle client data
                char buf[512];
                ssize_t count = recv(events[i].data.fd, buf, sizeof(buf), 0);
                if (count <= 0) {
                    close(events[i].data.fd);
                } else {
                    ::send(events[i].data.fd, buf, count, 0);
                }
            }
        }
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