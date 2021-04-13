#pragma once

#include <spic/spic_node.hpp>

#include <arpa/inet.h>

#include <cstring>

namespace Spic {
namespace Impl {

class FifoMessage : public Message {
public:

    typedef std::vector<uint8_t> Data;
    typedef uint32_t Position;

    FifoMessage(size_t size) : m_data(HEADER_SIZE), m_pos(HEADER_SIZE) {
        m_data.reserve(HEADER_SIZE+size);
    }
    FifoMessage() : FifoMessage(0u) {}
    ~FifoMessage() {}

    virtual NodeId senderId() override
    {
        return decode32(m_data.data());
    }

    virtual void* payload() override
    {
        return m_data.data()+HEADER_SIZE;
    }

    virtual size_t payloadSize() override
    {
        return decode32(m_data.data()+sizeof(NodeId));
    }

    virtual void reservePayloadSize(size_t size) override
    {
        m_data.reserve(HEADER_SIZE+size);
    }

    virtual void pushPayload(const void* data, size_t size) override
    {
        m_data.resize(m_data.size()+size);
        auto* pos = m_data.data() + m_data.size() - size;
        (void)std::memcpy(pos, data, size);
        encodePayloadSize();
    }

    virtual void popPayload(void* data, size_t size) override
    {
        (void)std::memcpy(data, &m_data.data()[m_pos], size);
        m_pos+=size;
    }

    virtual void pushPayload(const std::string& payload) override
    {
        auto size = payload.size();
        pushPayload(&size, sizeof(size));
        pushPayload(payload.data(), size);
    }

    virtual void popPayload(std::string& payload) override
    {
        size_t size;
        popPayload(&size, sizeof(size));
        payload.resize(size);
        popPayload(payload.data(), size);
    }

    virtual void* data()
    {
        return m_data.data();
    }

    virtual size_t dataSize()
    {
        return m_data.size();
    }

    virtual void encodeHeader(NodeId senderId)
    {
        encodeSenderId(senderId);
        encodePayloadSize();
    }

    virtual void encodeSenderId(NodeId senderId)
    {
        encode32(m_data.data(), senderId);
    }

    virtual void encodePayloadSize() {

        encode32(m_data.data()+sizeof(NodeId), m_data.size()-HEADER_SIZE);
    }

    static const uint32_t HEADER_SIZE = sizeof(NodeId) + sizeof(size_t);

protected:

    uint32_t decode32(const void* data) {
        uint32_t tmp;
        ::memcpy(&tmp, data, sizeof(tmp));
        return ntohl(tmp);
    }

    void encode32(void* data, uint32_t value) {
        uint32_t tmp = htonl(value);
        ::memcpy(data, &tmp, sizeof(tmp));
    }

    Data m_data;
    Position m_pos;
};

}} // namespaces
