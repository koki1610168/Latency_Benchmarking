#include <gtest/gtest.h>
#include "socket_wrapper.hpp"
#include <thread>

TEST(SocketWrapperTCPTest, TCPBindAndListenDoesNotThrow) {
    SocketWrapper server(Protocol::TCP);
    EXPECT_NO_THROW(server.bindAndListen(55555));
}

TEST(SocketWrapperTCPTest, CanConnectToServer) {
    SocketWrapper server(Protocol::TCP);
    ASSERT_NO_THROW(server.bindAndListen(55556));

    std::thread server_thread([&]() {
        server.acceptClient();
    });

    SocketWrapper client(Protocol::TCP);
    EXPECT_NO_THROW(client.connectToServer("127.0.0.1", 55556));

    server_thread.join();
}

TEST(SocketWrapperTCPTest, CanSendAndReceive) {
    SocketWrapper server(Protocol::TCP);
    ASSERT_NO_THROW(server.bindAndListen(55557));

    std::thread server_thread([&]() {
        server.acceptClient();

        char buffer[128] = {};
        server.receive(buffer, sizeof(buffer));
        server.send("received", strlen("received"));
    });

    SocketWrapper client(Protocol::TCP);
    ASSERT_NO_THROW(client.connectToServer("127.0.0.1", 55557));

    const char* msg = "sending message";
    client.send(msg, sizeof(msg));

    char response[128] = {};
    client.receive(response, sizeof(response));

    EXPECT_STREQ(response, "received");

    server_thread.join();

}