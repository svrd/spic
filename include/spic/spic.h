#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef unsigned spic_node_id_t;
typedef int spic_result_t;

#define SPIC_OK 0
#define SPIC_NOK -1

spic_result_t spic_create_node(spic_node_id_t node_id);
bool spic_node_exists(spic_node_id_t node_id);
void spic_destroy_all_nodes();
void spic_destroy_node(spic_node_id_t node_id);
spic_result_t spic_start_node(spic_node_id_t node_id);
void spic_stop_node(spic_node_id_t node_id);
bool spic_node_is_running(spic_node_id_t node_id);
spic_result_t spic_send(spic_node_id_t src_node_id, spic_node_id_t dst_node_id,
    void* data, size_t size);
ssize_t spic_get_no_of_messages(spic_node_id_t node_id);
spic_result_t spic_receive(spic_node_id_t node_id, void* data, size_t size);

#ifdef __cplusplus
}
#endif