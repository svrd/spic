#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

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

#ifdef __cplusplus
}
#endif