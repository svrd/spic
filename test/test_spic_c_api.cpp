#include <spic/spic.h>
#include <spic/spic_node.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdlib.h>
#include <unistd.h>

using namespace ::testing;

class TestSpicCApi : public ::testing::Test {
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        spic_destroy_all_nodes();
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

TEST_F(TestSpicCApi, requestConfirm)
{
    typedef struct foo_req
    {
        const uint32_t id;
    } foo_req_t;

    typedef struct foo_cfm
    {
        const uint32_t id;
    } foo_cfm_t;

    spic_node_id_t foo_node_id = 1u;
    spic_node_id_t bar_node_id = 2u;
    (void)spic_create_node(foo_node_id);
    (void)spic_start_node(foo_node_id);

    (void)spic_create_node(bar_node_id);
    (void)spic_start_node(bar_node_id);

    foo_req_t foo_req {1u};
    foo_cfm_t foo_cfm {2u};

    ASSERT_EQ(spic_send(foo_node_id, bar_node_id, (void*)&foo_req, sizeof(foo_req)), SPIC_OK);
    unsigned retries = 1000;
    while(spic_get_no_of_messages(bar_node_id) != 1 && retries-- > 0)
    {
        useconds_t duration_1_ms = 1000u;
        ::usleep(duration_1_ms);
    }
    ASSERT_GT(retries, 0u);
    ASSERT_EQ(spic_get_no_of_messages(bar_node_id), 1);
    ASSERT_EQ(spic_receive(bar_node_id, &foo_cfm, sizeof(foo_cfm)), SPIC_OK);
    spic_destroy_node(foo_node_id);
    spic_destroy_node(bar_node_id);
}