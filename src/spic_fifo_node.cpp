#include <spic/spic_node.hpp>
#include "spic_fifo_node.hpp"
#include "spic_sys_if.hpp"

#include <map>

namespace Spic {

Spic::MessagePtr& operator<<(Spic::MessagePtr& msg, const std::string& payload) {
    *msg.get() << payload;
    return msg;
}

Spic::MessagePtr& operator>>(Spic::MessagePtr& msg, std::string& payload) {
    *msg.get() >> payload;
    return msg;
}

SysIf sysIf;

NodePtr CreateNode(NodeId id) {
    return NodePtr(new Impl::FifoNode(id, sysIf));
}

} // namespace Spic