#include <spic/spic_node.hpp>
#include "spic_fifo_node.hpp"
#include "spic_sys_if.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <arpa/inet.h>

using namespace ::testing;

class TestSpicFifoNodeMessage : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestSpicFifoNodeMessage, prepareHeaderEncodesSenderNodeId) {

    Spic::NodeId fooId = 2u;
    Spic::Impl::FifoMessage msg(0u);

    msg.encodeSenderId(fooId);

    ASSERT_EQ(fooId, msg.senderId());
    ASSERT_EQ(0u, msg.payloadSize());
}

TEST_F(TestSpicFifoNodeMessage, encodeAndDecodeStringPayload) {

    Spic::NodeId fooId = 2u;
    std::string sendMsg = "A message to bar";

    Spic::Impl::FifoMessage msg(sendMsg.size());
    msg.pushPayload(sendMsg);
    msg.encodeHeader(fooId);

    ASSERT_EQ(fooId, msg.senderId());
    std::string recvMsg;
    msg.popPayload(recvMsg);
    ASSERT_EQ(sendMsg, recvMsg);
}