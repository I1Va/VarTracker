#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

struct Node { uint64_t id; std::string_view name; bool named; bool alive = true; };
struct Event {
    enum Kind { 
        INIT_CONSTRUCT,
        COPY_CONSTRUCT, 
        MOVE_CONSTRUCT, 
        COPY_ASSIGN, 
        MOVE_ASSIGN, 
        DESTRUCT 
    } kind;
    uint64_t src_id; // 0 if none
    uint64_t dst_id;
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
        std::ostringstream o;
        o << "digraph G {\n";
        for (auto &p : nodes_) {
            const Node &n = p.second;
            if (show_named_only && !n.named) continue;
            o << "  n" << n.id << " [label=\"";
            if (n.named) o << n.name << " (id" << n.id << ")";
            else o << "#" << n.id;
            if (!n.alive) o << " (dead)";
            o << "\"";
            if (!n.named) o << " style=filled fillcolor=gray95";
            o << "];\n";
        }
        for (auto &e : events_) {
            // if filtering, edges connecting filtered nodes will be omitted here
            o << "  n" << e.src_id << " -> n" << e.dst_id
              << " [label=\"";
            switch (e.kind) {
              case Event::COPY_CONSTRUCT: o << "copy-ctor"; break;
              case Event::INIT_CONSTRUCT: o << "init-ctor"; break;
              case Event::MOVE_CONSTRUCT: o << "move-ctor"; break;
              case Event::COPY_ASSIGN: o << "copy-assign"; break;
              case Event::MOVE_ASSIGN: o << "move-assign"; break;
              case Event::DESTRUCT: o << "destr"; break;
              default: o << "ctor/destr";
            }
            o << "\"];\n";
        }
        o << "}\n";
        return o.str();
    }

private:
    GraphBuilder() = default;

};
