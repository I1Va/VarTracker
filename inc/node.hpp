#pragma once
#include <cstdint>
#include <string>
class GraphBuilder;

class Node { 
    const GraphBuilder *environment_;
    std::string type_;
    uint64_t id_;
    std::string_view name_;  
    const void* addr_; 
    std::string value_;
    size_t scope_id_ = 0;

public:
    Node
    (
        const GraphBuilder *environment,
        const std::string &type, const uint64_t id, 
        const std::string_view name, const void* addr, const std::string value
    );

    void print(std::ostream &stream) const;

    void set_scope(const size_t scope_id) { scope_id_ = scope_id; }
    size_t get_scope() const { return scope_id_; }
    uint64_t get_id() const { return id_; }
    void set_value(const std::string &value) { value_ = value; }
    void set_value(std::string &&value) { value_ = std::move(value); }
};