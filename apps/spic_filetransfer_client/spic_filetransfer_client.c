#include "spic_filetransfer_interface.h"

#include <spic/spic.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s FILENAME\n", argv[0]);
        return -1;
    }

    bool success = false;

    spic_node_id_t src_id = SPIC_FILETRANSFER_CLIENT_ID;
    spic_node_id_t dst_id = SPIC_FILETRANSFER_SERVER_ID;
    (void)spic_create_node(src_id);

    (void)spic_start_node(src_id);

    start_filetransfer_req_t start_req = {START_FILETRANSFER_REQ_ID};
    (void)strncpy(start_req.filename, argv[1], sizeof(start_req.filename));
    FILE* fp = fopen(start_req.filename, "r");
    if(fp != NULL)
    {
        fseek(fp, 0L, SEEK_END);
        start_req.file_size = ftell(fp);
        size_t buffer_size = 512;
        rewind(fp);

        unsigned message_count = 0;
        ssize_t byte_count;

        unsigned char buffer[buffer_size];
        if(spic_send(src_id, dst_id, &start_req, sizeof(start_req)) == SPIC_OK)
        {
            ssize_t nr_of_bytes = 1;
            while(nr_of_bytes > 0)
            {
                nr_of_bytes = fread(&buffer, sizeof(unsigned char), sizeof(buffer), fp);
                if(nr_of_bytes > 0)
                {
                    if(spic_send(src_id, dst_id, &buffer, nr_of_bytes) != SPIC_OK)
                    {
                        printf("Send failed\n");
                        break;
                    }
                    byte_count += nr_of_bytes;
                    message_count++;
                }
                else if(feof(fp) == 0)
                {
                    printf("Read failed: %s\n", strerror(errno));
                    break;
                }
                else
                {
                    printf("Transfer complete, sent %u messages, %lu bytes in total\n", message_count, byte_count);
                    success = true;
                    break;
                }
            }
            fclose(fp);
        }
        else
        {
            printf("Failed to send start transfer request\n");
        }
    }
    else
    {
        printf("Failed to open file");
    }

    (void)spic_destroy_node(src_id);
    return success ? 0 : -1;
}
