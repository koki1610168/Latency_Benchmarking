#include <gtest/gtest.h>
#include "socket_wrapper.hpp"

TEST(SocketWrapperTest, TCPBindAndListenDoesNotThrow) {
    SocketWrapper server;
    EXPECT_NO_THROW(server.bindAndListen(55555));
}

TEST(SocketWrapperTest, CanConnectToServer) {
    SocketWrapper server;
    ASSERT_NO_THROW(server.bindAndListen(55556));

    std::thread server_thread([&]() {
        server.acceptClient();
    });

    SocketWrapper client;
    EXPECT_NO_THROW(client.connectToServer("127.0.0.1", 55556));

    server_thread.join();
}

TEST(SocketWrapperTest, CanSendAndReceive) {
    SocketWrapper server;
    ASSERT_NO_THROW(server.bindAndListen(55557));

    std::thread server_thread([&]() {
        server.acceptClient();

        char buffer[128] = {};
        server.receive(buffer, sizeof(buffer));
        server.send("received", strlen("received"));
    });

    SocketWrapper client;
    ASSERT_NO_THROW(client.connectToServer("127.0.0.1", 55557));

    const char* msg = "sending message";
    client.send(msg, sizeof(msg));

    char response[128] = {};
    client.receive(response, sizeof(response));

    EXPECT_STREQ(response, "received");

    server_thread.join();

}