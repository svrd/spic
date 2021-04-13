#pragma once

#include <spic/spic.h>

#include <stdint.h>

static const spic_node_id_t SPIC_FILETRANSFER_SERVER_ID = 1u;
static const spic_node_id_t SPIC_FILETRANSFER_CLIENT_ID = 2u;

static const uint32_t START_FILETRANSFER_REQ_ID = 1;
typedef struct
{
    unsigned message_id;
    char filename[256];
    size_t file_size;
} start_filetransfer_req_t;
