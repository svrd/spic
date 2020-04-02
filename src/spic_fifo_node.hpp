#pragma once

#include <spic/spic_node.hpp>
#include "spic_sys_if.hpp"
#include "spic_message.hpp"
#include "spic_fifo.hpp"

#include <string>
#include <system_error>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

namespace Spic::Impl {

class FifoNode : public Node {
public:

    class Exception : public Node::Exception {
    public:
        Exception(const std::string& msg)
            : m_msg(msg) {}

        const char* what()
        {
            return m_msg.c_str();
        }
    protected:
        std::string m_msg;
    };

    FifoNode(NodeId id, SysIf& sysIf) :
        m_nodeId(id),
        m_sysIf(sysIf),
        m_ownFifo(id, sysIf),
        m_isRunning(false)
    {
    }

    virtual ~FifoNode()
    {
        stop();
    }

    virtual void start() override
    {
        m_ownFifo.create();
        m_ownFifo.open();

        m_isRunning = true;
        m_thread = std::thread(&Spic::Impl::FifoNode::run, this);
    }

    virtual void stop() override
    {
        m_isRunning = false;

        m_ownFifo.destroy();

        // constexpr int quit_signal = SIGINT;
        // ::pthread_kill(m_thread.native_handle(), quit_signal);

        if(m_thread.joinable())
        {
            m_thread.join();
        }

        m_ownFifo.destroy();
    }

    virtual bool isRunning() override
    {
        return m_isRunning;
    }

    virtual MessagePtr createMessage(size_t size = 0) override
    {
        return MessagePtr(new FifoMessage(size));
    }

    virtual MessagePtr createMessage(const uint8_t* payload, size_t size) override
    {
        auto msg = createMessage(size);
        msg->pushPayload(payload, size);
        return msg;
    }


    virtual bool send(NodeId receiverId, const MessagePtr msg) override
    {
        throwNodeExceptionIf(!m_isRunning, "Node not started");

        auto& fifoMsg = *dynamic_cast<FifoMessage*>(msg.get());

        Fifo fifo(receiverId, m_sysIf);
        fifo.open();

        fifoMsg.encodeHeader(m_nodeId);

        return fifo.write(fifoMsg.data(), fifoMsg.dataSize());
    }

    virtual bool send(NodeId receiverId, const uint8_t* payload,
        size_t size) override
    {
        return send(receiverId, createMessage(payload, size));
    }

    virtual size_t noOfMessages() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_messages.size();
    }

    virtual MessagePtr receive() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_messages.size() == 0)
        {
            m_messageReceived.wait(lock);
        }
        auto msg = m_messages.front();
        m_messages.pop();
        return msg;
    }

protected:

    void run() {
        try
        {
            auto msg = receiveImpl();
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_messages.push(msg);
                m_messageReceived.notify_one();
            }
        }
        catch(std::exception& e)
        {
            // TODO: Do some error counting instead?
            std::cerr << e.what() << std::endl;
        }
    }

    MessagePtr receiveImpl()
    {
        auto msg = createMessage();
        auto& fifoMsg = *dynamic_cast<FifoMessage*>(msg.get());

        if (!m_ownFifo.read(fifoMsg.data(), FifoMessage::HEADER_SIZE))
        {
            throwNodeException("Complete header not received");
        }

        msg->reservePayloadSize(fifoMsg.payloadSize());

        if (!m_ownFifo.read(fifoMsg.data()+FifoMessage::HEADER_SIZE,
                               fifoMsg.payloadSize()))
        {
            throwNodeException("Complete message not received");
        }
        return msg;
    }

    void throwNodeException(const std::string& msg)
    {
        throw Exception(msg);
    }

    void throwNodeExceptionIf(bool condition, const std::string& msg)
    {
        if(condition)
        {
            throwNodeException(msg);
        }
    }

    NodeId m_nodeId;
    SysIf& m_sysIf;
    Fifo m_ownFifo;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_messageReceived;

    std::atomic<bool> m_isRunning;
    std::queue<MessagePtr> m_messages;
};

} // namespace Spic::Impl
