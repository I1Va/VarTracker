#include <iostream>
#include "node.hpp"
#include "graph_builder.hpp"

Node::Node
    (
        const GraphBuilder *environment,
        const std::string &type, const uint64_t id, 
        const std::string_view name, const void* addr, 
        const std::string value
    ):
        environment_(environment), type_(type), id_(id), name_(name), addr_(addr), value_(value) {}

void Node::print(std::ostream &stream) const {
    stream << "  n" << id_;
    stream << " [label=\"" << type_<< " " << name_ << " #" << id_
            << " " << addr_ << " " << " val = " << value_ << "\"";
    stream << " shape=rect style=filled fillcolor=" << (name_ != "" ? "lightgreen" : "gray");
    stream << "];\n";
}
