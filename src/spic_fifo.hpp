#pragma once

#include <spic/spic_node.hpp>
#include "spic.hpp"
#include "spic_sys_if.hpp"
#include <iostream>

namespace Spic::Impl {

class Fifo {
public:
    Fifo(NodeId id, SysIf& sysIf) :
        m_sysIf(sysIf),
        m_id(id),
        m_name(""),
        m_fd(-1)
    {
        m_name = "/tmp/spic." + std::to_string(id) + ".fifo";
    }

    ~Fifo()
    {
        this->close();
    }

    void create()
    {
        this->destroy();
        int mode = 0666u;
        throwSystemExceptionIf(failed(m_sysIf.mkfifo(m_name, mode)));
    }

    void open()
    {
        this->close();
        m_fd = m_sysIf.open(m_name, O_RDWR);
        throwSystemExceptionIf(failed(m_fd));
    }

    void close()
    {
        if (m_fd > -1)
        {
            m_sysIf.close(m_fd);
            m_fd = -1;
        }
    }

    void destroy()
    {
        if (m_sysIf.access(m_name, 0) == F_OK)
        {
            (void)m_sysIf.unlink(m_name);
            (void)::system(std::string("rm -rf " + m_name).c_str());
        }
    }

    bool read(void *buf, size_t count)
    {
        size_t totalNoOfBytes = 0;
        ssize_t noOfBytes = 0;
        while (totalNoOfBytes < count)
        {
            if(m_fd < 0)
            {
                break;
            }
            noOfBytes = m_sysIf.read(m_fd, buf, count);
            throwSystemExceptionIf(failed(noOfBytes));
            if (noOfBytes == 0)
            {
                break;
            }
            totalNoOfBytes += noOfBytes;
        }
        return totalNoOfBytes == count;
    }

    bool write(const void *buf, size_t count)
    {
        if(m_fd < 0)
        {
            return false;
        }
        size_t totalNoOfBytes = 0;
        ssize_t noOfBytes = 0;
        while (totalNoOfBytes < count)
        {
            noOfBytes = m_sysIf.write(m_fd, ((uint8_t*)buf)+totalNoOfBytes, 
                count);
            throwSystemExceptionIf(failed(noOfBytes));
            if (noOfBytes == 0)
            {
                throwSystemExceptionIf(errno != 0);
                break;
            }
            totalNoOfBytes += noOfBytes;
        }
        return totalNoOfBytes == count;
    }

protected:
    SysIf& m_sysIf;
    NodeId m_id;
    std::string m_name;
    int m_fd;
};

} // namespace Spic::Impl