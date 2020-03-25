extern "C"
{
#include <spic/spic.h>
}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class TestSpic : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestSpic, createNode)
{
    spic_node_id_t foo_node_id = 1u;
    int result = spic_create_node(foo_node_id);
    ASSERT_EQ(result, 0);
}