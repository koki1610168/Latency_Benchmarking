#include <gtest/gtest.h>
#include "socket_wrapper.hpp"

TEST(SocketWrapperTest, TCPBindAndListenDoesNotThrow) {
    SocketWrapper server;
    EXPECT_NO_THROW(server.bindAndListen(55555));
}