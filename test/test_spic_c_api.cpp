#include <spic/spic.h>
#include <spic/spic_node.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdlib.h>

using namespace ::testing;

class TestSpicCApi : public ::testing::Test {
protected:
    void SetUp() override
    {
        spic_destroy_all_nodes();
        RemoveFifos();
    }

    void TearDown() override
    {
        spic_destroy_all_nodes();
        RemoveFifos();
    }

    void RemoveFifos()
    {
        system("rm -f /tmp/spic*");
    }

};

TEST_F(TestSpicCApi, createNode)
{
    spic_node_id_t foo_node_id = 1u;
    ASSERT_EQ(spic_create_node(foo_node_id), SPIC_OK);
}

TEST_F(TestSpicCApi, createdNodeExists)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    ASSERT_TRUE(spic_node_exists(foo_node_id));
}

TEST_F(TestSpicCApi, notCreatedNodeIsNotRunning)
{
    spic_node_id_t foo_node_id = 1u;
    ASSERT_FALSE(spic_node_is_running(foo_node_id));
}

TEST_F(TestSpicCApi, createdNodeIsNotRunning)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    ASSERT_FALSE(spic_node_is_running(foo_node_id));
}

TEST_F(TestSpicCApi, notCreatedNodeDoesNotExists)
{
    spic_node_id_t foo_node_id = 1u;
    ASSERT_FALSE(spic_node_exists(foo_node_id));
}

TEST_F(TestSpicCApi, createNodeThatAlreadyExistFails)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    ASSERT_EQ(spic_create_node(foo_node_id), SPIC_NOK);
}

TEST_F(TestSpicCApi, createTwoNodesWithDifferentIds)
{
    spic_node_id_t foo_node = 1u;
    spic_node_id_t bar_node = 2u;
    ASSERT_EQ(spic_create_node(foo_node), SPIC_OK);
    ASSERT_EQ(spic_create_node(bar_node), SPIC_OK);
}

TEST_F(TestSpicCApi, destroyedNodeDoesNotExist)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    spic_destroy_node(foo_node_id);
    ASSERT_FALSE(spic_node_exists(foo_node_id));
}

TEST_F(TestSpicCApi, destroyedNodeCanBeRecreated)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    spic_destroy_node(foo_node_id);
    ASSERT_EQ(spic_create_node(foo_node_id), SPIC_OK);
}

TEST_F(TestSpicCApi, startNode)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    ASSERT_EQ(spic_start_node(foo_node_id), SPIC_OK);
}

TEST_F(TestSpicCApi, startedNodeIsRunning)
{
    spic_node_id_t foo_node_id = 1u;
    (void)spic_create_node(foo_node_id);
    (void)spic_start_node(foo_node_id);
    ASSERT_TRUE(spic_node_is_running(foo_node_id));
}

TEST_F(TestSpicCApi, startNodeThatHasNotBeenCreatedFails)
{
    spic_node_id_t foo_node_id = 1u;
    ASSERT_EQ(spic_start_node(foo_node_id), SPIC_NOK);
}