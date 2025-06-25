#include <gtest/gtest.h>
#include "socket_wrapper.hpp"
#include <thread>
#include <vector>
#include <chrono>

TEST(SocketWrapperTCPTest, TCPBindAndListenDoesNotThrow) {
    SocketWrapper server(Protocol::TCP);
    EXPECT_NO_THROW(server.bindAndListen(55555));
}

TEST(SocketWrapperTCPTest, CanConnectToServer) {
    SocketWrapper server(Protocol::TCP);
    ASSERT_NO_THROW(server.bindAndListen(55556));

    std::thread server_thread([&]() {
        server.runEpollServerLoop();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SocketWrapper client(Protocol::TCP);
    EXPECT_NO_THROW(client.connectToServer("127.0.0.1", 55556));

    server_thread.detach();
}

TEST(SocketWrapperTCPTest, MultiClientConnections) {
    SocketWrapper server(Protocol::TCP);
    ASSERT_NO_THROW(server.bindAndListen(55557));

    std::thread server_thread([&]() {
        server.runEpollServerLoop();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    constexpr int client_count = 5;
    std::vector<std::thread> clients;

    for (int i = 0; i < client_count; ++i) {
        clients.emplace_back([i]() {
            try {
                SocketWrapper client(Protocol::TCP);
                ASSERT_NO_THROW(client.connectToServer("127.0.0.1", 55557));
                const char* msg = "hello";
                client.send(msg, 5);

                char buf[128];
                ssize_t received = -1;
                for (int i = 0; i < 10; ++i) {
                    received = client.receive(buf, sizeof(buf));
                    if (received > 0) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                EXPECT_GT(received, -1);
            } catch (...) {
                FAIL() << "Client " << i << "failed to connect or communicate" << std::endl;
            }
        });
    }

    for (auto& t : clients) {
        t.join();
    }

    server_thread.detach();

}