#pragma once
#include <string>
#include <cstdint>
#include <ostream>

class Edge {

public:
    enum Kind {
        CONSTRUCT,
        ASSIGN,
        MOVE,
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
            EDGE_KIND_DESCR_(MOVE)
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
