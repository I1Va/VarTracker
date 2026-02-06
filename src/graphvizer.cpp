#include "graphvizer.hpp"
#include <cassert>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace grz
{

std::ostream& operator<<(std::ostream& stream, const Color color)
{
    std::ios_base::fmtflags original_flags = stream.flags();

    stream << std::hex << std::setfill('0');

    stream << std::setw(2) << +color.r << std::setw(2) << +color.g << std::setw(2) << +color.b;

    stream.flags(original_flags);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, NodeStyle style)
{
    return enum_to_stream(stream, style);
}

std::ostream& operator<<(std::ostream& stream, NodeShape shape)
{
    return enum_to_stream(stream, shape);
}

std::ostream& operator<<(std::ostream& stream, GraphRankdir rankdir)
{
    return enum_to_stream(stream, rankdir);
}

std::ostream& operator<<(std::ostream& stream, const DotNode& node)
{
    stream << "NODE_" << node.identifier << "[";

    stream << "shape=\"" << node.attributes.shape << "\",";
    stream << "color=\"#" << node.attributes.color << "\",";
    stream << "style=\"" << node.attributes.style << "\",";
    stream << "fillcolor=\"#" << node.attributes.fill_color << "\",";

    stream << "label=\"{";
    stream << escape_string(node.type_label) << " | " << escape_string(node.lexeme_label);
    stream << "}\"]";

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const DotEdge& edge)
{
    stream << "NODE_" << edge.start_node_id;

    stream << " -> ";

    stream << "NODE_" << edge.end_node_id;

    stream << " [";
    stream << "color=\"#" << edge.attributes_.color << "\"";
    stream << ",penwidth=" << edge.attributes_.penwidth;
    stream << ",label=\"\"";
    stream << "]";

    return stream;
}

void DotGraph::convert_to_image(const std::string& image_path)
{
    std::ofstream dot_file(TEMP_DOT_FILEPATH, std::ios::out);
    if (!dot_file)
    {
        std::cerr << "failed to create dot_file ifstream\n";
        return;
    }

    dot_file << "digraph G {\n";
    dot_file << "    rankdir=" << attributes_.rankdir << ";\n";

    for (const auto& node : nodes_)
        dot_file << "    " << node << "\n";
    for (const auto& edge : edges_)
        dot_file << "    " << edge << "\n";

    dot_file << "}\n";

    dot_file.flush();
    dot_file.close();

    std::stringstream ss;
    std::string escaped_path;
    ss << std::quoted(image_path);
    ss >> escaped_path;

    const std::string image_gen_command = "dot -Tsvg " + TEMP_DOT_FILEPATH + " -o " + escaped_path;

    auto result = std::system(image_gen_command.c_str());

    if (result != 0)
        throw GraphvizException("dot image generation failed");

    std::remove(TEMP_DOT_FILEPATH.c_str());
}

// DotNode DotGraph::create_dot_node(const compiler::AstNode* ast_node, ColorTable& node_color_table)
// {
//     assert(ast_node);

//     const char* ast_node_type_name = typeid(*ast_node).name();

//     if (node_color_table.find(ast_node_type_name) == node_color_table.end())
//         node_color_table[ast_node_type_name] = generate_random_color();

//     DotNode dot_node{};
//     dot_node.attributes.shape = attributes_.default_node_shape;
//     dot_node.attributes.color = attributes_.default_node_color;
//     dot_node.attributes.fill_color = node_color_table[ast_node_type_name];
//     dot_node.attributes.style = attributes_.default_node_style;

//     dot_node.lexeme_label = ast_node->get_string_lexeme();
//     dot_node.type_label = strip_namespace(demangle(ast_node_type_name));
//     dot_node.identifier = reinterpret_cast<uintptr_t>(ast_node);

//     return dot_node;
// }

// DotEdge DotGraph::create_dot_edge(const compiler::AstNode* start_node,
//                                   const compiler::AstNode* end_node)
// {
//     DotEdge edge{};
//     edge.attributes_.color = attributes_.default_edge_color;
//     edge.attributes_.penwidth = attributes_.default_edge_pen_width;
//     edge.start_node_id = reinterpret_cast<uintptr_t>(start_node);
//     edge.end_node_id = reinterpret_cast<uintptr_t>(end_node);

//     return edge;
// }

} // namespace grz