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
    }

    void TearDown() override
    {
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

TEST_F(TestSpic, destroyedNodeCanBeRecreated)
{
    Spic::NodeId fooNodeId = 1u;
    {
        auto fooNode = Spic::CreateNode(fooNodeId);
        fooNode->start();
    }
    {
        auto fooNode = Spic::CreateNode(fooNodeId);
        fooNode->start();
    }
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
    fooNode->send(fooNodeId, NULL, 0);

    unsigned retries = 1000u;
    while(retries && fooNode->noOfMessages() == 0u)
    {
        std::this_thread::sleep_for(1ms);
        retries--;
    }
    ASSERT_GT(retries, 0u);
    ASSERT_EQ(fooNode->noOfMessages(), 1u);
}

TEST_F(TestSpic, receiveMessageFromSelf)
{
    Spic::NodeId fooNodeId = 1u;
    uint8_t sendData[] = {0u, 1u, 2u, 3u};
    auto fooNode = Spic::CreateNode(fooNodeId);
    fooNode->start();
    fooNode->send(fooNodeId, sendData, sizeof(sendData));

    unsigned retries = 1000u;
    while(retries && fooNode->noOfMessages() == 0u)
    {
        std::this_thread::sleep_for(1ms);
        retries--;
    }
    ASSERT_GT(retries, 0u);
    auto msgPtr = fooNode->receive();
    ASSERT_EQ(msgPtr->senderId(), fooNodeId);
    ASSERT_EQ(msgPtr->payloadSize(), sizeof(sendData));
    uint8_t recvData[sizeof(sendData)];
    (void)memset(recvData, 0u, sizeof(recvData));
    msgPtr->popPayload(recvData, sizeof(recvData));
    ASSERT_THAT(recvData, ElementsAreArray(sendData));
}

TEST_F(TestSpic, requestConfirm)
{
    const Spic::MessageId FOO_REQ = 1;
    class FooRequest {
    public:
        int fooStatus;
    };

    const Spic::MessageId FOO_CFM = 2;
    class FooConfirm {
    public:
        int barStatus;
    };

    /* Create and start the two nodes foo and bar */
    Spic::NodeId fooNodeId = 1;
    Spic::NodePtr fooNode = Spic::CreateNode(fooNodeId);
    fooNode->start();

    Spic::NodeId barNodeId = 2;
    Spic::NodePtr barNode = Spic::CreateNode(barNodeId);
    barNode->start();

    /* Send a foo request from foo too bar */
    FooRequest fooRequest = {100u};
    auto sentReqMsg = fooNode->createMessage(sizeof(FOO_REQ)+ sizeof(fooRequest));
    sentReqMsg->pushPayload(FOO_REQ);
    sentReqMsg->pushPayload(fooRequest);
    fooNode->send(barNodeId, sentReqMsg);

    /* Bar receives the request from foo */
    auto recvReqMsg = barNode->receive();
    auto requesterId = recvReqMsg->senderId();
    //auto recvReqMsgId = recvReqMsg->get<Spic::MessageId>();
    Spic::MessageId recvReqMsgId;
    recvReqMsg->popPayload(recvReqMsgId);
    auto& recvFooRequest = recvReqMsg->get<FooRequest>();

    /* Bar responds with a confirm back to foo */
    FooConfirm fooConfirm = {recvFooRequest.fooStatus + 1};
    auto sentCfmMsg = barNode->createMessage(sizeof(FOO_CFM) + sizeof(fooConfirm));
    sentCfmMsg->pushPayload(FOO_CFM);
    sentCfmMsg->pushPayload(fooConfirm);
    barNode->send(requesterId, sentCfmMsg);

    /* Foo receives the confirm from bar */
    auto recvCfmMsg = fooNode->receive();
    auto confirmerId = recvCfmMsg->senderId();
    //auto recvCfmMsgId = recvCfmMsg->get<Spic::MessageId>();
    Spic::MessageId recvCfmMsgId;
    recvCfmMsg->popPayload(recvCfmMsgId);
    auto& recvFooConfirm = recvCfmMsg->get<FooConfirm>();

    /* Assert that foo receives the correct barStatus in the confirm from bar */
    ASSERT_EQ(confirmerId, barNodeId);
    ASSERT_EQ(recvReqMsgId, FOO_REQ);
    ASSERT_EQ(recvCfmMsgId, FOO_CFM);
    ASSERT_EQ(recvFooConfirm.barStatus, fooRequest.fooStatus + 1);
}

