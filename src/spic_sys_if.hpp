#pragma once

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

namespace Spic {

class SysIf {
public:
    virtual int mkfifo(const std::string& pathname, mode_t mode)
    {
        return ::mkfifo(pathname.c_str(), mode);
    }

    virtual int access(const std::string& name, int type)
    {
        return ::access(name.c_str(), type);
    }

    virtual int unlink(const std::string& name)
    {
        return ::unlink(name.c_str());
    }

    virtual int open(const std::string& pathname, int flags, mode_t mode)
    {
        return ::open(pathname.c_str(), flags, mode);
    }

    virtual int open(const std::string& pathname, int flags)
    {
        return ::open(pathname.c_str(), flags);
    }

    virtual void close(int fd)
    {
        ::close(fd);
    }

    virtual ssize_t write(int fd, const void *buf, size_t count)
    {
        return ::write(fd, buf, count);
    }

    virtual ssize_t read(int fd, void *buf, size_t count)
    {
        return ::read(fd, buf, count);
    }
};

}