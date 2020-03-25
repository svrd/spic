#include "spic_fifo_node.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>
#include <string>

using namespace ::testing;

class TestSpicMessage : public ::testing::Test {
protected:
    void SetUp() override {
        ::system("rm -f /tmp/spic.*.fifo");
    }

    void TearDown() override {}
};

/*
 * boost::serialize is recommended for more poweful serialization.
 * However, this should work for most.
 * This is an example of how to bit more complex class structure can
 * be serialized. This is class that holds a list of objects of
 * another class.
 * NOTE: It is not recommended to use the << and >> operators.
 * They are used here for show and test purposes.
 * Use save on the object directly and push/popPayload methods
 * on Message.
 */
class Route {
public:

    class Coordinate {
    public:
        Coordinate(unsigned x, unsigned y) : m_x(x), m_y(y) {}
        Coordinate() : Coordinate(0, 0) {}
        ~Coordinate() {}
        void save(Spic::Message& msg) const {

            // Same as msg->pushPayload(m_x)
            msg << m_x;
            msg << m_y;
        }

        void load(Spic::Message& msg) {

            // Equivalent of msg->pushPayload(m_x)
            msg >> m_x;
            msg >> m_y;
        }

    public:
        unsigned m_x;
        unsigned m_y;
    };

    Route(unsigned id, const std::string& name) : m_id(id), m_name(name) {}
    Route() : Route(0, "") {}
    ~Route() {}

    void save(Spic::Message& msg) const {

        // Equivalent to msg->pushPayload(m_name);
        msg << m_name;
        msg << m_id;

        // Save list of objects of type Coordinate
        msg << m_coordinates.size();
        for(auto& coord : m_coordinates) {
            coord.save(msg);
        }
    }

    void load(Spic::Message& msg) {

        // Equivalent to msg->popPayload(m_a);
        msg >> m_name;
        msg >> m_id;

        // Load list
        size_t listSize;
        msg >> listSize;
        m_coordinates.resize(listSize);
        for(auto& coord : m_coordinates) {
            coord.load(msg);
        }
    }

public:
    unsigned m_id;
    std::string m_name;
    std::list<Coordinate> m_coordinates;
};

bool operator==(const Route::Coordinate& first,
                const Route::Coordinate& second) {
    return first.m_x == second.m_x && first.m_y == second.m_y;
}

TEST_F(TestSpicMessage, createMessage) {

    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    auto msg1 = fooNode->createMessage();
    auto msg2 = fooNode->createMessage(1000u);
    auto msg3 = fooNode->createMessage();
    std::vector<uint8_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    msg3->pushPayload(data.data(), data.size());
    auto msg4 = fooNode->createMessage();
    std::string strMsg = "Add this string to stream";
    *msg4 << strMsg;
    std::string extractedStrMsg;
    *msg4 >> extractedStrMsg;

    ASSERT_EQ(0u, msg1->payloadSize());
    ASSERT_EQ(0u, msg2->payloadSize());
    ASSERT_EQ(data.size(), msg3->payloadSize());
    ASSERT_EQ(0, ::memcmp(data.data(), msg3->payload(), msg3->payloadSize()));
    ASSERT_EQ(strMsg, extractedStrMsg);
}

TEST_F(TestSpicMessage, serializableNestedClasses) {

    Spic::NodeId fooNodeId = 1u;
    auto fooNode = Spic::CreateNode(fooNodeId);
    auto msg1 = fooNode->createMessage();

    // Save route in message
    Route route1(99, "My route");
    route1.m_coordinates = { {1,2}, {2,3}, {3,4}, {4, 5}};
    //Spic::Serializable* s1 = (Spic::Serializable*)&route1;
    //msg1 << *(Spic::Serializable*)&route1; // Same as s1.save(msg1)
    route1.save(*msg1);// << *(Spic::Serializable*)&route1; // Same as s1.save(msg1)

    msg1->printData();

    // Load route from message
    Route route2;
    route2.load(*msg1); //*msg1 >> *(Spic::Serializable*)&route2; // Same as route2.load(msg1)

    //ASSERT_EQ(route1.m_name, route2.m_name);
    ASSERT_EQ(route1.m_id, route2.m_id);
    ASSERT_EQ(route1.m_coordinates.size(), route2.m_coordinates.size());
    ASSERT_EQ(route1.m_coordinates, route2.m_coordinates);
}