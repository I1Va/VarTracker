#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class Edge {
protected:
    std::string_view label_;
    uint64_t src_id_; 
    uint64_t dst_id_;

    const char *color_ = "black";
    size_t penwidth_ = 1;
    const char *style_ = "solid";

public:
    virtual ~Edge() = default;   
    Edge(const std::string_view label, const uint64_t src_id, const uint64_t dst_id): 
        label_(label), src_id_(src_id), dst_id_(dst_id) {}
    
    void print(std::ostream &stream) const {
        if (src_id_ == 0 && dst_id_ == 0) return;

        stream << "  n" << src_id_ << " -> n" << dst_id_;
        stream << " [label=\"" << label_ << "\"";
        stream << " color=" << color_;
        stream << " penwidth=" << penwidth_;
        stream << " style=" << style_;
        stream << " arrowhead=normal";
        stream << "];\n";
    }
};

class CopyEdge final : public Edge {
    enum kind {
        CONSTRUCT,
        ASSIGN,
    };

private:
    CopyEdge(std::string_view label, const uint64_t src_id, const uint64_t dst_id): Edge(label, src_id, dst_id) {
        color_ = "red";
        penwidth_ = 3;
        style_ = "solid";
    }

      const char *stkerr_get_bit_descr(stk_err err) {
    #define BIT_DESCR_(code) case code : return #code;
    switch (err) {
        BIT_DESCR_(STK_ERR_OK)
        BIT_DESCR_(STK_ERR_UNKNOWN)
        BIT_DESCR_(STK_ERR_CALLOC)
        BIT_DESCR_(STK_ERR_NULLPTR)
        BIT_DESCR_(STK_ERR_STAT)
        BIT_DESCR_(STK_ERR_INPUT_DATA)
        BIT_DESCR_(STK_ERR_MEM)
        BIT_DESCR_(STK_ERR_FILE_CLOSE)
        BIT_DESCR_(STK_ERR_FILE_OPEN)
        BIT_DESCR_(STK_ERR_ARGS)
        BIT_DESCR_(STK_ERR_WRONG_COEF)
        BIT_DESCR_(STK_ERR_INIT)
        BIT_DESCR_(STK_ERR_STACK_NULLPTR)
        BIT_DESCR_(STK_ERR_STACK_CONT_NULLPTR)
        BIT_DESCR_(STK_ERR_STACK_OVERFLOW)
        BIT_DESCR_(STK_ERR_STACK_POP)
        BIT_DESCR_(STK_ERR_REALLOC)
        BIT_DESCR_(STK_ERR_CANARY_LEFT)
        BIT_DESCR_(STK_ERR_CANARY_MID)
        BIT_DESCR_(STK_ERR_CANARY_RIGHT)
        BIT_DESCR_(STK_ERR_CANARY_STK_RIGHT)
        BIT_DESCR_(STK_ERR_HASH_STACK_DATA_MISMATCH)
        BIT_DESCR_(STK_ERR_CANARY_STK_LEFT)
        BIT_DESCR_(STK_ERR_SYSTEM)
        BIT_DESCR_(STK_ERR_STACK_LAST_ELEM)
        BIT_DESCR_(STK_ERR_HASH_STACK_STRUCT_MISMATCH)
        BIT_DESCR_(STK_ERR_INVALID_INDEX)

        default: return "VERY STRANGE ERROR:(";
    }
    #undef BIT_DESCR_
}

};

class OperatorEdge : public Edge {
    OperatorEdge(std::string_view label, const uint64_t src_id, const uint64_t dst_id): Edge(label, src_id, dst_id) {
        color_ = "gray";
        penwidth_ = 1;
        style_ = "dotted";
    }
};

class MoveEdge : public Edge {
    MoveEdge(std::string_view label, const uint64_t src_id, const uint64_t dst_id): Edge(label, src_id, dst_id) {
        color_ = "green";
        penwidth_ = 2;
        style_ = "solid";
    }
};

class Node { 
protected:
    std::string_view type;
    uint64_t id;
    std::string_view name;  
    uintptr_t addr; 

public:
    void print(std::ostream &stream) const {
        stream << "  n" << id;
        stream << " [label=\"" << name << " (id" << id << ")";
        else stream << "#" << id;
        if (!alive) stream << " (dead)";
        stream << "\"";
        stream << " shape=rect style=filled fillcolor=" << (alive ? "lightgreen" : "lightcoral");
        stream << "];\n";
    }
};

class GraphBuilder {
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, Node> nodes_;
    std::vector<Event> events_;

public:
    static GraphBuilder& instance() {
        static GraphBuilder g;
        return g;
    }

    uint64_t make_node(const std::string_view name="") {
        uint64_t id = next_id_++;
        nodes_.emplace(id, Node{id, name, !name.empty(), true});
        return id;
    }

    void mark_dead(uint64_t id) {
        auto it = nodes_.find(id);
        if (it != nodes_.end()) it->second.alive = false;
    }

    void add_event(Event::Kind k, uint64_t src, uint64_t dst) {
        events_.push_back(Event{k, src, dst});
    }

    std::string to_dot() const {
        std::ostringstream ostream;
        ostream << "digraph G {\n";
        ostream << "  rankdir=LR;\n";
        ostream << "  node [shape=rect style=filled fontname=\"Courier\"];\n";

        ostream << "  splines=polyline;\n";  
        ostream << "  nodesep=1.0;\n";       
        ostream << "  ranksep=1.5;\n";      
    
        for (auto &[id, node] : nodes_) node.print(ostream);
        for (auto &edge : events_) edge.print(ostream);

        ostream << "}\n";
        return ostream.str();
    }

    void to_image(std::string_view image_name, bool remove_dotfile=true) {
        std::string temp_dot_filename = std::string(image_name) + std::string(".dot");
        {
            std::ofstream image{temp_dot_filename};
            if (!image) {
                std::cerr << "Error creating image!\n";
                return;
            }

            image << to_dot();
        }
       
        std::string command = std::string("dot -Tpng ") + std::string(image_name) + std::string(".dot") + std::string(" -o ") + std::string(image_name) + ".png";
        int ret = system(command.c_str());
        if (ret != 0) {
            std::cerr << "Failed to build graph. Return code : " << ret << "\n";
        }
        
        if (remove_dotfile) std::remove(temp_dot_filename.c_str());
    }

private:
    GraphBuilder() = default;
};
