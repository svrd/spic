#include <spic/spic_node.hpp>
#include "spic_fifo_node.hpp"
#include "spic_sys_if.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class TestSpicFifoNode : public ::testing::Test {
protected:
    void SetUp() override
    {
        RemoveFifo();
    }

    void TearDown() override
    {
        RemoveFifo();
    }

    void RemoveFifo()
    {
        ::system("rm -f /tmp/spic.*.fifo");
    }
};

class FifoMock : public Spic::SysIf {
public:
    MOCK_METHOD2(mkfifo, int(const std::string&, mode_t));
    MOCK_METHOD1(remove, int(const std::string&));
    MOCK_METHOD2(open, int(const std::string&, int));
    MOCK_METHOD3(write, ssize_t(int, const void*, size_t));
};

TEST_F(TestSpicFifoNode, startFifoNodeThrowsExceptionWhenMkfifoFails)
{
    NiceMock<FifoMock> fifoMock;

    EXPECT_CALL(fifoMock, mkfifo(_, _))
        .WillOnce(testing::InvokeWithoutArgs(
            []() {
                errno = EACCES;
                return -1;
            }));

    Spic::NodeId fooNodeId = 1;
    Spic::NodePtr fooNode = Spic::NodePtr(
        new Spic::Impl::FifoNode(fooNodeId, fifoMock));

    EXPECT_THROW(fooNode->start(), std::system_error);
}

TEST_F(TestSpicFifoNode, startFifoNodeThrowsExceptionIfFifoExists)
{
    NiceMock<FifoMock> fifoMock;

    EXPECT_CALL(fifoMock, mkfifo(_, _))
        .WillOnce(testing::InvokeWithoutArgs(
            []() {
                errno = EEXIST;
                return -1;
            }));

    Spic::NodeId fooNodeId = 1;
    Spic::NodePtr fooNode = Spic::NodePtr(
        new Spic::Impl::FifoNode(fooNodeId, fifoMock));
    EXPECT_THROW(fooNode->start(), std::system_error);
}

// TEST_F(SpicTest, sendEmptyMessage) {

//     NiceMock<FifoMock> fifoMock;

//     EXPECT_CALL(fifoMock, write(_, _, _))
//         .WillOnce(Return(0));

//     Spic::NodeId fooNodeId = 1u;
//     Spic::NodePtr fooNode = Spic::NodePtr(new Spic::FifoNode(fooNodeId, fifoMock));
//     fooNode->start();

//     Spic::NodeId barNodeId = 2u;

//     auto msg = fooNode->prepareMessage(barNodeId);

//     ASSERT_TRUE(fooNode->send(msg));
// }


// TEST_F(SpicTest, sendWhenNotStartedThrowsException) {

//     NiceMock<FifoMock> fifoMock;

//     EXPECT_CALL(fifoMock, open(_, _))
//         .Times(0);

//     Spic::NodeId fooNodeId = 1u;
//     Spic::NodePtr fooNode = Spic::NodePtr(new Spic::FifoNode(fooNodeId, fifoMock));
//     Spic::NodeId barNodeId = 2u;
//     Spic::Message m(barNodeId, "This is a message from foo when not started");
//     try {
//         fooNode->send(barNodeId, m);
//         FAIL();
//     } catch(Spic::Node::Exception& expected) {
//         ASSERT_STREQ("Node not started", expected.what());
//     }
// }

// TEST_F(SpicTest, sendToNonExistingNodeReturnsFalse) {

//     NiceMock<FifoMock> fifoMock;

//     EXPECT_CALL(fifoMock, mkfifo(_, _))
//         .WillOnce(Return(0));
//     EXPECT_CALL(fifoMock, open("/tmp/spic.1.fifo", _))
//         .WillOnce(Return(0));
//     EXPECT_CALL(fifoMock, open("/tmp/spic.2.fifo", _))
//         .WillOnce(testing::InvokeWithoutArgs(
//             []() {
//                 errno = ENOENT;
//                 return -1;
//             }));

//     Spic::NodeId fooNodeId = 1u;
//     Spic::NodePtr fooNode = Spic::NodePtr(new Spic::FifoNode(fooNodeId, fifoMock));
//     fooNode->start();
//     Spic::NodeId barNodeId = 2u;
//     Spic::Message m(barNodeId, "This is a message from foo to bar but bar does not exist");

//     ASSERT_FALSE(fooNode->send(barNodeId, m));
// }

// TEST_F(SpicTest, sendReturnsFalseWhenWriteFails) {

//     NiceMock<FifoMock> fifoMock;

//     EXPECT_CALL(fifoMock, write(_, _, _))
//         .WillOnce(testing::InvokeWithoutArgs(
//             []() {
//                 errno = EPIPE;
//                 return -1;
//             }));

//     Spic::NodeId fooNodeId = 1u;
//     Spic::NodePtr fooNode = Spic::NodePtr(new Spic::FifoNode(fooNodeId, fifoMock));
//     fooNode->start();
//     Spic::NodeId barNodeId = 2u;
//     Spic::Message m(barNodeId, "This is a message from foo that fails to be written");

//     ASSERT_FALSE(fooNode->send(barNodeId, m));
// }

// TEST_F(SpicTest, startTwoNodesWithSameIdFails) {

//     Spic::NodeId fooNodeId = 1u;
//     Spic::NodePtr fooNode = Spic::CreateNode(fooNodeId);
//     fooNode->start();

//     Spic::NodeId barNodeId = 1u;
//     Spic::NodePtr barNode = Spic::CreateNode(barNodeId);

//     try {
//         barNode->start();
//         FAIL();
//     } catch(std::system_error& expected) {
//         ASSERT_STREQ("File exists", expected.what());
//     }
// }
