#pragma once

#include <string_view>
#include <iostream>
#include <utility>
#include <cstdint>
#include <typeinfo>
#include <cxxabi.h>

#include "graph_builder.hpp"

class ScopeGuard {

public:
    ScopeGuard(const std::string &signature) {
        GraphBuilder::instance().new_scope(signature);
    }
    ScopeGuard(std::string &&signature) {
        GraphBuilder::instance().new_scope(std::move(signature));
    }

    ~ScopeGuard() {
        GraphBuilder::instance().close_scope();
    }
};

#define TRACK_VAR(T, name, ...) Tracked<T> name(#name, __VA_ARGS__);
#define INIT_FUNC() ScopeGuard scope(__PRETTY_FUNCTION__);




template <typename T>
std::string full_type_name() {
    const char* mangled = typeid(T).name();
    int status = 0;

    std::unique_ptr<char, void(*)(void*)> demangled(
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
        std::free
    );

    return (status == 0) ? demangled.get() : mangled;
}

template <typename T>
struct Tracked {
    uint64_t graph_id_;
    std::string_view name_{};
    std::string type_{};
    T value_;
public:
    Tracked() = delete;

    Tracked(std::string_view name, const T& value)
        : name_(name), type_(full_type_name<T>()), value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, type_, name_);
    }

    Tracked(std::string_view name, const Tracked& other)
        : name_(name), type_(full_type_name<T>()), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, type_, name_);
        GraphBuilder::instance().add_copy_edge(Edge::CONSTRUCT, other.graph_id_, graph_id_);
    }

    Tracked(const Tracked& other)
        : name_(other.name_), type_(other.type_), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, type_, name_);
        GraphBuilder::instance().add_copy_edge(Edge::CONSTRUCT, other.graph_id_, graph_id_);
    }

    Tracked(Tracked&& other) noexcept
        : name_(other.name_), type_(other.type_), value_(std::move(other.value_)) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, type_, name_);
        GraphBuilder::instance().add_move_edge(Edge::CONSTRUCT, other.graph_id_, graph_id_);
    }

    template<typename U>
    Tracked(const Tracked<U>& other)
        : name_(other.name_), type_(typeid(T).name()), value_(static_cast<T>(other.value_)) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, type_, name_);
        GraphBuilder::instance().add_copy_edge(Edge::CONSTRUCT, other.graph_id_, graph_id_);
    }

    Tracked(const T& value) : value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, full_type_name<T>());
    }

    Tracked(T&& value) : value_(std::move(value)) {
        graph_id_ = GraphBuilder::instance().make_node(&value_, value_, full_type_name<T>());
    }

    Tracked& operator=(const Tracked& other) {
        value_ = other.value_;
        GraphBuilder::instance().update_node_value(graph_id_, value_);
        GraphBuilder::instance().add_copy_edge(Edge::ASSIGN, other.graph_id_, graph_id_);
        return *this;
    }

    Tracked& operator=(Tracked&& other) noexcept {
        value_ = std::move(other.value_);
        GraphBuilder::instance().update_node_value(graph_id_, value_);
        GraphBuilder::instance().add_move_edge(Edge::ASSIGN, other.graph_id_, graph_id_);
        return *this;
    }

    // operator T() const { return value_; }

#define BUILD_ARITHMETIC(op, kind)                                                              \
    friend Tracked operator op(const Tracked& a, const Tracked& b) {                            \
        Tracked r("", a.value_ op b.value_);                                                    \
        GraphBuilder::instance().add_operator_edge(Edge::kind, a.graph_id_, r.graph_id_);       \
        GraphBuilder::instance().add_operator_edge(Edge::kind, b.graph_id_, r.graph_id_);       \
        return r;                                                                               \
    }                                                                                           \
    friend Tracked operator op(const Tracked& a, const T& b) {                                  \
        Tracked r("", a.value_ op b);                                                           \
        uint64_t tmp_id = GraphBuilder::instance().make_node(&b, b, full_type_name<T>());       \
        GraphBuilder::instance().add_operator_edge(Edge::kind, a.graph_id_, r.graph_id_);       \
        GraphBuilder::instance().add_operator_edge(Edge::kind, tmp_id, r.graph_id_);            \
        return r;                                                                               \
    }                                                                                           \
    friend Tracked operator op(const T& a, const Tracked& b) {                                  \
        Tracked r("", a op b.value_);                                                           \
        GraphBuilder::instance().add_operator_edge(Edge::kind, b.graph_id_, r.graph_id_);       \
        return r;                                                                               \
    }

    BUILD_ARITHMETIC(+, ADD)
    BUILD_ARITHMETIC(-, SUB)
    BUILD_ARITHMETIC(*, MUL)
    BUILD_ARITHMETIC(/, DIV)
#undef BUILD_ARITHMETIC

#define BUILD_COMPARISON(op, kind)                                                              \
    friend Tracked<bool> operator op(const Tracked& a, const Tracked& b) {                      \
        Tracked<bool> r("", a.value_ op b.value_);                                              \
        GraphBuilder::instance().add_operator_edge(Edge::kind, a.graph_id_, r.graph_id_);       \
        GraphBuilder::instance().add_operator_edge(Edge::kind, b.graph_id_, r.graph_id_);       \
        return r;                                                                               \
    }                                                                                           \
                                                                                                \
    friend Tracked<bool> operator op(const Tracked& a, const T& b) {                            \
        Tracked<bool> r("", a.value_ op b);                                                     \
        uint64_t tmp_id = GraphBuilder::instance().make_node(&b, b, "bool");                    \
        GraphBuilder::instance().add_operator_edge(Edge::kind, a.graph_id_, r.graph_id_);       \
        GraphBuilder::instance().add_operator_edge(Edge::kind, tmp_id, r.graph_id_);            \
        return r;                                                                               \
    }                                                                                           \
                                                                                                \
    friend Tracked<bool> operator op(const T& a, const Tracked& b) {                            \
        Tracked<bool> r("", a op b.value_);                                                     \
        uint64_t tmp_id = GraphBuilder::instance().make_node(&a, a, "bool");                    \
        GraphBuilder::instance().add_operator_edge(Edge::kind, tmp_id, r.graph_id_);            \
        GraphBuilder::instance().add_operator_edge(Edge::kind, b.graph_id_, r.graph_id_);       \
        return r;                                                                               \
    }
BUILD_COMPARISON(>, GT)
BUILD_COMPARISON(<, LT)
BUILD_COMPARISON(>=, GE)
BUILD_COMPARISON(<=, LE)
BUILD_COMPARISON(==, EQ)
BUILD_COMPARISON(!=, NE)

#undef BUILD_COMPARISON

#define BUILD_COMPOUND_ASSIGN(op, kind)                                                         \
    Tracked& operator op(const Tracked& rhs) {                                                  \
        value_ op rhs.value_;                                                                   \
        GraphBuilder::instance().update_node_value(graph_id_, value_);                          \
        GraphBuilder::instance().add_operator_edge(Edge::kind, rhs.graph_id_, graph_id_);       \
        return *this;                                                                           \
    }                                                                                           \
    Tracked& operator op(const T& rhs) {                                                        \
        value_ op rhs;                                                                          \
        GraphBuilder::instance().update_node_value(graph_id_, value_);                          \
        uint64_t rhs_id = GraphBuilder::instance().make_node(&rhs, rhs, full_type_name<T>());   \
        GraphBuilder::instance().add_operator_edge(Edge::kind, rhs_id, graph_id_);              \
        return *this;                                                                           \
    }

    BUILD_COMPOUND_ASSIGN(+=, ADD)
    BUILD_COMPOUND_ASSIGN(-=, SUB)
    BUILD_COMPOUND_ASSIGN(*=, MUL)
    BUILD_COMPOUND_ASSIGN(/=, DIV)
#undef BUILD_COMPOUND_ASSIGN

    template<typename U>
    friend std::istream& operator>>(std::istream&, Tracked<U>&);

    template<typename U>
    friend std::ostream& operator<<(std::ostream&, const Tracked<U>&);
};

template<typename T>
std::istream& operator>>(std::istream& is, Tracked<T>& t) {
    is >> t.value_;
    uint64_t input_node = GraphBuilder::instance().make_node(&t.value_, t.value_, full_type_name<T>());
    GraphBuilder::instance().add_operator_edge(Edge::ASSIGN, input_node, t.graph_id_);
    return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Tracked<T>& t) {
    uint64_t output_node = GraphBuilder::instance().make_node(&t.value_, t.value_, full_type_name<T>());
    GraphBuilder::instance().add_operator_edge(Edge::ASSIGN, t.graph_id_, output_node);
    return os << t.value_;
}
