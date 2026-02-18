#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include "edge.hpp"
#include "node.hpp"



struct Scope {
    std::string signature;
    int parent_id = -1;

    Scope(const std::string &in_signature, const size_t in_parent_id): 
        signature(in_signature), parent_id(in_parent_id) {}
    Scope(std::string &&in_signature, const size_t in_parent_id): 
        signature(std::move(in_signature)), parent_id(in_parent_id) {}
};

class GraphBuilder {
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, Node> nodes_;
    std::vector<std::unique_ptr<Edge>> edges_;

    std::vector<Scope> scopes_storage{Scope("Global Scope", -1)};
    std::stack<size_t> scopes_stack;

public:
    static GraphBuilder& instance() {
        static GraphBuilder g;
        return g;
    }

    void new_scope(const std::string &signature) {
        scopes_storage.push_back(Scope(signature, scopes_stack.top()));
        scopes_stack.push(scopes_storage.size() - 1);
    }
    void new_scope(std::string &&signature) {
        scopes_storage.push_back(Scope(std::move(signature), scopes_stack.top()));
        scopes_stack.push(scopes_storage.size() - 1);
    }
    
    void close_scope() {
        scopes_stack.pop();
    }

    std::vector<Scope> &get_scopes_storage() { return scopes_storage; }

    template <typename T>
    void update_node_value(const uint64_t id, const T& new_value) {
        auto it = nodes_.find(id);
        if (it != nodes_.end()) {
            it->second.set_value(std::to_string(new_value));
        }
    }

    template <typename T>
    uint64_t make_node
    (
        const void* addr, const T& value, 
        const std::string type="", const std::string_view name="") 
    {
        uint64_t id = next_id_++;
        Node node = Node(this, type, id, name, addr, std::to_string(value));
        if (!scopes_stack.empty()) {
            node.set_scope(scopes_stack.top());
        } 
        nodes_.emplace(id, node);

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
        
        print_clusters(ostream);
        // for (auto &[id, node] : nodes_) node.print(ostream);
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
    
    void print_cluster
    (
        std::ostream &stream,
        const std::vector<std::vector<const Node *>> &cluster_nodes, 
        const size_t cluster_id, const size_t indent
    ) const {
        const std::string indent_string(indent, ' ');
        stream << indent_string << "subgraph cluster_" << cluster_id << " {\n";
        stream << indent_string << "label = \"" << scopes_storage[cluster_id].signature << "\";\n";
        stream << indent_string << "color = \"" << "blue" << "\";\n";
        stream << indent_string << "penwidth = \"" << "3" << "\";\n";
        stream << indent_string << "fontcolor= \"" << "red" << "\"\n";     
        stream << indent_string << "fontsize= " << 20 << "\n";

        
        for (const Node *node: cluster_nodes[cluster_id]) {
            stream << indent_string; node->print(stream);
        }
    } 

    void print_cluster_recursive
    (
        std::ostream &stream, 
        const std::vector<std::vector<size_t>> &graph, 
        const std::vector<std::vector<const Node *>> &cluster_nodes,
        const size_t id, const size_t indent
    ) const {
        const std::string indent_string(indent, ' ');
        for (size_t child_id : graph[id]) {
            print_cluster(stream, cluster_nodes, child_id, indent);
            print_cluster_recursive(stream, graph, cluster_nodes, child_id, 2);
            stream << indent_string << "}\n";
        }
    }

    void print_clusters(std::ostream &stream) const {
        std::vector<std::vector<const Node *>> cluster_nodes(scopes_storage.size());
        std::vector<std::vector<size_t>> clusters_graph(scopes_storage.size());

        for (auto &[node_id, node] : nodes_) {
            cluster_nodes[node.get_scope()].push_back(&node);
        }
    
        for (size_t scope_id = 0; scope_id < scopes_storage.size(); scope_id++) {
            size_t parent_id = scopes_storage[scope_id].parent_id;
            if (scope_id != 0) {
                clusters_graph[parent_id].push_back(scope_id);   
            }
        }

        const size_t indent = 2;
        const std::string indent_string(indent, ' ');
        
        print_cluster(stream, cluster_nodes, 0, 2);
        print_cluster_recursive(stream, clusters_graph, cluster_nodes, 0, 2);
        stream << indent_string << "}\n";
    }

    GraphBuilder() {
        scopes_stack.push(0);       
    }
};
