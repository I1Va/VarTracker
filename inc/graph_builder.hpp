#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

struct Node { 
    uint64_t id; 
    std::string_view name; 
    bool named; 
    bool alive = true; 

    void print(std::ostream &stream, bool show_named_only=false) const {
        if (show_named_only && !named) return;

        stream << "  n" << id << " [label=\"";
        if (named) stream << name << " (id" << id << ")";
        else stream << "#" << id;
        if (!alive) stream << " (dead)";
        stream << "\"";
        stream << " shape=rect style=filled fillcolor=" << (alive ? "lightgreen" : "lightcoral");
        stream << "];\n";
    }
};

struct Event {
    enum Kind { 
        INIT_CONSTRUCT,
        COPY_CONSTRUCT, 
        MOVE_CONSTRUCT, 
        COPY_ASSIGN, 
        MOVE_ASSIGN, 
        DESTRUCT
    } kind;

    uint64_t src_id; 
    uint64_t dst_id;

    void print(std::ostream &stream) const {
        if (src_id == 0 && dst_id == 0) return;

        stream << "  n" << src_id << " -> n" << dst_id
               << " [label=\"";
        switch (kind) {
            case INIT_CONSTRUCT:   stream << "init-ctor"; break;
            case COPY_CONSTRUCT:   stream << "copy-ctor"; break;
            case MOVE_CONSTRUCT:   stream << "move-ctor"; break;
            case COPY_ASSIGN:      stream << "copy-assign"; break;
            case MOVE_ASSIGN:      stream << "move-assign"; break;
            case DESTRUCT:         stream << "destr"; break;
            default:               stream << "ctor/destr";
        }
        stream << "\"";

        bool copy_state = (kind == COPY_CONSTRUCT || kind == COPY_ASSIGN);
        stream << " color=" << (copy_state ? "red" : "green");     
        stream << " penwidth=" << (copy_state ? "3" : "2");         
        stream << " arrowhead=normal"; 
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

    uint64_t make_node(const std::string_view name) {
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

    std::string to_dot(bool show_named_only=false) const {
        std::ostringstream ostream;
        ostream << "digraph G {\n";
        ostream << "  rankdir=LR;\n";
        ostream << "  node [shape=rect style=filled fontname=\"Courier\"];\n";

        ostream << "  splines=polyline;\n";  
        ostream << "  nodesep=1.0;\n";       
        ostream << "  ranksep=1.5;\n";      
    
        for (auto &[id, node] : nodes_) node.print(ostream, show_named_only);
        for (auto &edge : events_) edge.print(ostream);

        ostream << "}\n";
        return ostream.str();
    }

    void to_image(std::string_view image_name, bool show_named_only=false) {
        std::string temp_dot_filename = std::string(image_name) + std::string(".dot");
        {
            std::ofstream image{temp_dot_filename};
            if (!image) {
                std::cerr << "Error creating image!\n";
                return;
            }

            image << to_dot(show_named_only);
        }
       
        std::string command = std::string("dot -Tpng ") + std::string(image_name) + std::string(".dot") + std::string(" -o ") + std::string(image_name);
        int ret = system(command.c_str());
        if (ret != 0) {
            std::cerr << "Failed to build graph. Return code : " << ret << "\n";
        }
    
        std::remove(temp_dot_filename.c_str());
    }

private:
    GraphBuilder() = default;
};
