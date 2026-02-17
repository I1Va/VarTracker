#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <memory>
#include <functional>

class Edge {

public:
    enum Kind {
        CONSTRUCT,
        ASSIGN,
        ADD,
        SUB,
        MUL,
        DIV,
        GT,
        LT,
        GE,
        LE,
        EQ,
        NE,
    };

    static const char *get_kind_label(const Kind kind) {
        #define EDGE_KIND_DESCR_(code) case code : return #code;
        switch (kind) {
            EDGE_KIND_DESCR_(CONSTRUCT)
            EDGE_KIND_DESCR_(ASSIGN)
            EDGE_KIND_DESCR_(ADD)
            EDGE_KIND_DESCR_(SUB)
            EDGE_KIND_DESCR_(MUL)
            EDGE_KIND_DESCR_(DIV)
            EDGE_KIND_DESCR_(GT)
            EDGE_KIND_DESCR_(LT)
            EDGE_KIND_DESCR_(GE)
            EDGE_KIND_DESCR_(LE)
            EDGE_KIND_DESCR_(EQ)
            EDGE_KIND_DESCR_(NE)
        
            default: return "Unknown";
        }
        #undef EDGE_KIND_DESCR_
    }

    virtual ~Edge() = default;       
    virtual void print(std::ostream &stream) const = 0;
    Edge(const Kind kind, const uint64_t src_id, const uint64_t dst_id): 
        kind_(kind), src_id_(src_id), dst_id_(dst_id) {}

protected:
    Kind kind_;
    uint64_t src_id_; 
    uint64_t dst_id_;
};

class CopyEdge final : public Edge {
public:
    using Edge::Edge;

    void print(std::ostream &stream) const {
        if (src_id_ == 0 && dst_id_ == 0) return;
        const char color[] = "red";
        const size_t penwidth = 3;
        const char style[] = "solid";

        stream << "  n" << src_id_ << " -> n" << dst_id_;
        stream << " [label=\"" << get_kind_label(kind_) << "\"";
        stream << " color="    << color;
        stream << " penwidth=" << penwidth;
        stream << " style="    << style;
        stream << " arrowhead=normal";
        stream << "];\n";
    }
};

class OperatorEdge : public Edge {
public:
   using Edge::Edge;

    void print(std::ostream &stream) const {
        if (src_id_ == 0 && dst_id_ == 0) return;
        const char color[] = "gray";
        const size_t penwidth = 1;
        const char style[] = "dotted";

        stream << "  n" << src_id_ << " -> n" << dst_id_;
        stream << " [label=\"" << get_kind_label(kind_) << "\"";
        stream << " color="    << color;
        stream << " penwidth=" << penwidth;
        stream << " style="    << style;
        stream << " arrowhead=normal";
        stream << "];\n";
    }
};

class MoveEdge : public Edge {
public:
    using Edge::Edge;
    
    void print(std::ostream &stream) const {
        if (src_id_ == 0 && dst_id_ == 0) return;
        const char color[] = "green";
        const size_t penwidth = 2;
        const char style[] = "solid";

        stream << "  n" << src_id_ << " -> n" << dst_id_;
        stream << " [label=\"" << get_kind_label(kind_) << "\"";
        stream << " color="    << color;
        stream << " penwidth=" << penwidth;
        stream << " style="    << style;
        stream << " arrowhead=normal";
        stream << "];\n";
    }
};

class Node { 
    std::string type_;
    uint64_t id_;
    std::string_view name_;  
    const void* addr_; 
    std::string value_;

public:

    Node
    (
        const std::string &type, const uint64_t id, 
        const std::string_view name, const void* addr, const std::string value
    ):
        type_(type), id_(id), name_(name), addr_(addr), value_(value) {}
    
    void print(std::ostream &stream) const {
        stream << "  n" << id_;
        stream << " [label=\"" << type_<< " " << name_ << " #" << id_
               << " " << addr_ << " " << " val = " << value_ << "\"";
        stream << " shape=rect style=filled fillcolor=" << (name_ != "" ? "lightgreen" : "gray");
        stream << "];\n";
    }
};

class GraphBuilder {
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, Node> nodes_;
    std::vector<std::unique_ptr<Edge>> edges_;

public:
    static GraphBuilder& instance() {
        static GraphBuilder g;
        return g;
    }

    template <typename T>
    uint64_t make_node
    (
        const void* addr, const T& value, 
        const std::string type="", const std::string_view name="") 
    {
        uint64_t id = next_id_++;
        nodes_.emplace(id, Node(type, id, name, addr, std::to_string(value)));
        return id;
    }

    void add_copy_edge(Edge::Kind kind, uint64_t src, uint64_t dst) {
        auto copy_edge = std::make_unique<CopyEdge>(kind, src, dst);
        edges_.push_back(std::move(copy_edge));
    }

    void add_move_edge(Edge::Kind kind, uint64_t src, uint64_t dst) {
        auto copy_edge = std::make_unique<MoveEdge>(kind, src, dst);
        edges_.push_back(std::move(copy_edge));
    }

    void add_operator_edge(Edge::Kind kind, uint64_t src, uint64_t dst) {
        auto copy_edge = std::make_unique<OperatorEdge>(kind, src, dst);
        edges_.push_back(std::move(copy_edge));
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
        for (auto &edge : edges_) edge->print(ostream);

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
