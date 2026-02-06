#pragma once
#include <string_view>
#include <iostream>
#include <utility>
#include "graphvizer.hpp"

#include "graph_builder.hpp"

#define TRACK_VAR(T, name, ...) \
    Tracked<T> name{#name, __VA_ARGS__, }

static inline void log(const std::string_view name, const char msg[]) {
    if (!name.empty()) {
        std::cerr << name << ": " << msg << '\n';
    }
}

template <typename T>
struct Tracked {
    uint64_t graph_id_;
    std::string_view name_{};
    T value_;

    Tracked() = delete;

    ~Tracked() {
        GraphBuilder::instance().mark_dead(graph_id_);
        GraphBuilder::instance().add_event(Event::DESTRUCT, graph_id_, 0);
    }

    Tracked(const Tracked& other)
        : name_(other.name_), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, 0, graph_id_);
    }   

    Tracked(std::string_view name, T& value)
        : name_(name), value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::INIT_CONSTRUCT, 0, graph_id_);
    }

    Tracked(T& value)
        : value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, 0, graph_id_);
    }

    Tracked& operator=(const Tracked& other) {
        value_ = other.value_;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, other.graph_id_, graph_id_);
        
        return *this;
    }

    Tracked(Tracked&& other) noexcept
        : name_(other.name_), value_(std::move(other.value_)) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::MOVE_CONSTRUCT, other.graph_id_, graph_id_);
        GraphBuilder::instance().mark_dead(other.graph_id_);
    }

    Tracked& operator=(Tracked&& other) noexcept {
        value_ = std::move(other.value_);
        GraphBuilder::instance().add_event(Event::MOVE_ASSIGN, other.graph_id_, graph_id_);
        GraphBuilder::instance().mark_dead(other.graph_id_);
        return *this;
    }

    operator T() const { return value_; }
    T& get() { return value_; }
    const T& get() const { return value_; }
};


template <typename T>
std::istream& operator>>(std::istream& is, Tracked<T>& x) {
    return is >> x.get();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Tracked<T>& x) {
    return os << x.get();
}

