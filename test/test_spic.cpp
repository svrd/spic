#include <spic/spic_node.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

using namespace ::testing;

class TestSpic : public ::testing::Test {
protected:
    void SetUp() override
    {
        removeFifo();
    }

    void TearDown() override
    {
        removeFifo();
    }

    void removeFifo()
    {
        ::system("rm -f /tmp/spic.*.fifo");
    }
};

TEST_F(TestSpic, createNode)
{
    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    ASSERT_TRUE(fooNode.get());
}

TEST_F(TestSpic, createdNodeIsNotRunning)
{
    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    ASSERT_FALSE(fooNode->isRunning());
}

TEST_F(TestSpic, startedNodeIsRunningAndHasZeroMessages)
{
    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    fooNode->start();
    ASSERT_TRUE(fooNode->isRunning());
    ASSERT_EQ(0u, fooNode->noOfMessages());
}

TEST_F(TestSpic, sendMessageToSelf)
{
    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    fooNode->start();
    fooNode->send(fooNodeId, 0, NULL, 0);

    unsigned retries = 1000u;
    while(retries && fooNode->noOfMessages() == 0u)
    {
        std::this_thread::sleep_for(1ms);
        retries--;
    }
    ASSERT_GT(retries, 0u);
    ASSERT_EQ(fooNode->noOfMessages(), 1u);
}

TEST_F(TestSpic, requestConfirm)
{
    const Spic::MessageId FOO_REQ = 1;
    class FooRequest {
    public:
        FooRequest() : FooRequest(false) {}
        FooRequest(bool status) : barStatus(status) {}
        FooRequest(const FooRequest& copy)
        {
            this->barStatus = copy.barStatus;
        }
        bool barStatus;
    };

    const Spic::MessageId FOO_CFM = 2;
    class FooConfirm {
    public:
        bool barStatus;
    };

    Spic::NodeId fooNodeId = 1;
    Spic::NodePtr fooNode = Spic::CreateNode(fooNodeId);
    fooNode->start();

    Spic::NodeId barNodeId = 2;
    Spic::NodePtr barNode = Spic::CreateNode(barNodeId);
    barNode->start();

    FooRequest fooRequest = {true};
    fooNode->send(
        barNodeId, FOO_REQ, (uint8_t*)&fooRequest, sizeof(FooRequest));

    auto msg = barNode->receive();
    Spic::NodeId requesterId = msg->senderId();
    auto& fooRequestRecv = msg->get<FooRequest>();

    FooRequest fooReq2 = fooRequestRecv;

    FooConfirm fooConfirm = {fooRequestRecv.barStatus};
    barNode->send(
        requesterId, FOO_CFM, (uint8_t*)&fooConfirm, sizeof(FooConfirm));

    Spic::NodeId confirmerId;
    FooConfirm fooConfirmRecv;
    fooNode->receive(
        confirmerId, FOO_CFM, (uint8_t*)&fooConfirmRecv, sizeof(FooConfirm));

    ASSERT_EQ(fooNodeId, requesterId);
    ASSERT_EQ(barNodeId, confirmerId);
    ASSERT_EQ(fooRequest.barStatus, fooRequestRecv.barStatus);
    ASSERT_EQ(fooRequest.barStatus, fooConfirm.barStatus);
    ASSERT_EQ(fooRequest.barStatus, fooConfirmRecv.barStatus);
}

