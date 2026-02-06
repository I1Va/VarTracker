#pragma once
#include <string_view>
#include <iostream>
#include <utility>
#include "graph_builder.hpp"

#define TRACK_VAR(T, name, ...) \
    Tracked<T> name(#name, __VA_ARGS__);

static inline void log(const std::string_view name, const char msg[]) {
    if (!name.empty()) {
        std::cerr << name << ": " << msg << '\n';
    }
}

template <typename T>
class Tracked {
    uint64_t graph_id_;
    std::string_view name_{};
    T value_;
public:
    Tracked() = delete;

    ~Tracked() {
        GraphBuilder::instance().mark_dead(graph_id_);
        GraphBuilder::instance().add_event(Event::DESTRUCT, graph_id_, 1);
    }

    Tracked(const Tracked& other)
        : name_(other.name_), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, 0, graph_id_);
    }   

    Tracked(std::string_view name, const T& value)
        : name_(name), value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::INIT_CONSTRUCT, 0, graph_id_);
    }

    Tracked(const T& value)
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

    friend Tracked operator+(const Tracked& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ + rhs.value_);
        return tmp;
    }
    friend Tracked operator-(const Tracked& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ - rhs.value_);
        return tmp;
    }
    friend Tracked operator*(const Tracked& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ * rhs.value_);
        return tmp;
    }
    friend Tracked operator/(const Tracked& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ / rhs.value_);
        return tmp;
    }

    // Tracked op T
    friend Tracked operator+(const Tracked& lhs, const T& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ + rhs);
        return tmp;
    }
    friend Tracked operator-(const Tracked& lhs, const T& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ - rhs);
        return tmp;
    }
    friend Tracked operator*(const Tracked& lhs, const T& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ * rhs);
        return tmp;
    }
    friend Tracked operator/(const Tracked& lhs, const T& rhs) {
        TRACK_VAR(T, tmp, lhs.value_ / rhs);
        return tmp;
    }

    // T op Tracked
    friend Tracked operator+(const T& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs + rhs.value_);
        return tmp;
    }
    friend Tracked operator-(const T& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs - rhs.value_);
        return tmp;
    }
    friend Tracked operator*(const T& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs * rhs.value_);
        return tmp;
    }
    friend Tracked operator/(const T& lhs, const Tracked& rhs) {
        TRACK_VAR(T, tmp, lhs / rhs.value_);
        return tmp;
    }

    // Compound assignments
    Tracked& operator+=(const Tracked& rhs) { value_ += rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator-=(const Tracked& rhs) { value_ -= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator*=(const Tracked& rhs) { value_ *= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator/=(const Tracked& rhs) { value_ /= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }

    Tracked& operator+=(const T& rhs) { value_ += rhs; return *this; }
    Tracked& operator-=(const T& rhs) { value_ -= rhs; return *this; }
    Tracked& operator*=(const T& rhs) { value_ *= rhs; return *this; }
    Tracked& operator/=(const T& rhs) { value_ /= rhs; return *this; }

    // Increment/Decrement
    Tracked& operator++() { ++value_; return *this; }
    Tracked operator++(int) { Tracked tmp(*this); ++value_; return tmp; }
    Tracked& operator--() { --value_; return *this; }
    Tracked operator--(int) { Tracked tmp(*this); --value_; return tmp; }

    std::istream& operator>>(std::istream& is) {
        return is >> value_;
    }

    std::ostream& operator<<(std::ostream& os) {
        return os << value_;
    }
};


