#include "spic_filetransfer_interface.h"

#include <spic/spic.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s FILENAME\n", argv[0]);
        return -1;
    }
    spic_node_id_t node_id = 1;
    return 0;
}