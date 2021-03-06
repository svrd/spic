#pragma once

#include <spic/spic.h>

#include <cstdint>
#include <memory>
#include <vector>
#include <system_error>

namespace Spic {

typedef spic_node_id_t NodeId;
typedef uint32_t MessageId;

class Message {
public:

    virtual NodeId senderId() = 0;
    virtual void* payload() = 0;
    virtual size_t payloadSize() = 0;
    virtual void reservePayloadSize(size_t size) = 0;
    virtual void pushPayload(const void* data, size_t size) = 0;
    virtual void popPayload(void* data, size_t size) = 0;
    virtual void pushPayload(const std::string& msg) = 0;
    virtual void popPayload(std::string& msg) = 0;

    template <typename T>
    T& get()
    {
        return (T&)*(payload()+4);
    }

    template <typename T>
    void pushPayload(T payload)
    {
        pushPayload((void*)&payload, sizeof(payload));
    }

    template <typename T>
    void popPayload(T& payload)
    {
        popPayload((void*)&payload, sizeof(payload));
    }

    template <typename T>
    Message& operator<<(T payload)
    {
        pushPayload(payload);
        return *this;
    }

    template <typename T>
    Message& operator>>(T& payload)
    {
        popPayload(payload);
        return *this;
    }
};

typedef std::shared_ptr<Message> MessagePtr;

class Node {
public:

    class Exception : public std::exception {
    public:
        virtual const char* what() const noexcept override = 0;
    };

    virtual ~Node() {}
    virtual MessagePtr createMessage(size_t size = 0) = 0;
    virtual MessagePtr createMessage(const void* payload, size_t size) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;
    virtual bool send(NodeId receiverId, const MessagePtr msg) = 0;
    virtual bool send(NodeId receiverId, const void* payload,
        size_t size) = 0;
    virtual size_t nrOfMessages() = 0;
    virtual MessagePtr receive() = 0;
};

typedef std::shared_ptr<Node> NodePtr;

NodePtr CreateNode(NodeId id);

} // namespace Spic