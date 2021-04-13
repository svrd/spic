#include <spic/spic.h>
#include <spic/spic_node.hpp>
#include "spic.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <map>

#define FIFO_NAME_LENGTH 19

static std::map<spic_node_id_t, Spic::NodePtr> node_map;

spic_result_t spic_create_node(spic_node_id_t node_id)
{
    if (spic_node_exists(node_id))
    {
        return SPIC_NOK;
    }
    auto node_ptr = Spic::CreateNode(node_id);
    node_map.insert(std::pair(node_id, node_ptr));

    return SPIC_OK;
}

bool spic_node_exists(spic_node_id_t node_id)
{
    if (node_map.find(node_id) != node_map.end())
    {
        return true;
    }
    return false;
}

void spic_destroy_all_nodes()
{
    node_map.clear();
}

void spic_destroy_node(spic_node_id_t node_id)
{
    node_map.erase(node_id);
}

int spic_start_node(spic_node_id_t node_id)
{
    if (!spic_node_exists(node_id))
    {
        return SPIC_NOK;
    }
    auto node_ptr = node_map[node_id];
    node_ptr->start();
    return SPIC_OK;
}

void spic_stop_node(spic_node_id_t node_id)
{
    if (!spic_node_exists(node_id))
    {
        return;
    }
    auto node_ptr = node_map[node_id];
    node_ptr->stop();
}

bool spic_node_is_running(spic_node_id_t node_id)
{
    if (!spic_node_exists(node_id))
    {
        return false;
    }
    return node_map[node_id]->isRunning();
}

spic_result_t spic_send(spic_node_id_t src_node_id, spic_node_id_t dst_node_id,
    void* data, size_t size)
{
    if (!spic_node_exists(src_node_id))
    {
        return SPIC_NOK;
    }
    auto node_ptr = node_map[src_node_id];

    bool send_result = false;

    try
    {
        send_result = node_ptr->send(dst_node_id, data, size);
    }
    catch(std::exception& e) {}

    if (!send_result)
    {
        return SPIC_NOK;
    }
    return SPIC_OK;
}

spic_result_t spic_receive(spic_node_id_t node_id, void* data, size_t size)
{
    if (!spic_node_exists(node_id))
    {
        return SPIC_NOK;
    }
    auto node_ptr = node_map[node_id];
    auto messagePtr = node_ptr->receive();
    messagePtr->popPayload(data, size);
    return SPIC_OK;
}

ssize_t spic_get_no_of_messages(spic_node_id_t node_id)
{
    if (!spic_node_exists(node_id))
    {
        return SPIC_NOK;
    }
    auto node_ptr = node_map[node_id];
    return node_ptr->nrOfMessages();
}

namespace Spic::Impl {

void throwSystemExceptionIf(bool condition) {

    if(condition) {

        throw std::system_error(errno, std::generic_category());
    }
}

bool failed(int returnCode) {

    return returnCode == -1;
}

bool fileDoesNotExistError() {

    return errno == ENOENT;
}

}