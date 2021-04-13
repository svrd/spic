#include "spic_filetransfer_interface.h"

#include <spic/spic_node.hpp>

#include <iostream>
#include <fstream>

int main()
{
    auto node = Spic::CreateNode(SPIC_FILETRANSFER_SERVER_ID);
    node->start();

    std::cout << "Filetransfer server started..." << std::endl;

    auto msg = node->receive();
    start_filetransfer_req_t start_req;
    msg->popPayload(start_req);

    if(start_req.message_id == START_FILETRANSFER_REQ_ID)
    {
        std::cout << "Receiving file: " << start_req.filename << std::endl;
        std::cout << "Size: " << start_req.file_size << std::endl;
        auto bytesLeft = start_req.file_size;
        auto file = std::fstream(start_req.filename, std::ios::out | std::ios::binary);
        auto messageCount = 0;
        auto bytesReceived = 0;
        while (true)
        {
            auto msg = node->receive();
            char buffer[msg->payloadSize()];
            msg->popPayload(buffer, sizeof(buffer));
            file.write(buffer,sizeof(buffer));
            bytesLeft -= msg->payloadSize();
            messageCount++;
            bytesReceived += msg->payloadSize();
            if(bytesLeft == 0)
            {
                std::cout << "Transfer complete, received "
                          << messageCount << " messages, "
                          << bytesReceived << " bytes" << std::endl;
                break;
            }
        }
        file.close();
    }

    return 0;
}
