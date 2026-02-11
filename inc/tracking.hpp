#pragma once
#include <string_view>
#include <iostream>
#include <utility>
#include <cstdint>
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
    }

    Tracked(const Tracked& other)
        : name_(other.name_), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, other.graph_id_, graph_id_);
    }

    Tracked(std::string_view name, const T& value)
        : name_(name), value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
    }

    Tracked(std::string_view name, const Tracked& other)
        : name_(name), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, other.graph_id_, graph_id_);
    }

 
    Tracked(const T& value)
        : value_(value) {
        graph_id_ = GraphBuilder::instance().make_node("#lit");
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

    Tracked(std::string_view name, Tracked&& other)
        : name_(name), value_(std::move(other.value_)) {
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

    template<typename U>
    friend std::istream& operator>>(std::istream& is, Tracked<U>& t);
    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Tracked<U>& t);

    // Tracked<T> vs Tracked<T>
    friend bool operator==(const Tracked& lhs, const Tracked& rhs) {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ == rhs.value_;
    }
    friend bool operator!=(const Tracked& lhs, const Tracked& rhs) {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ != rhs.value_;
    }
    friend bool operator<(const Tracked& lhs, const Tracked& rhs)  {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ <  rhs.value_;
    }
    friend bool operator>(const Tracked& lhs, const Tracked& rhs)  {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ >  rhs.value_;
    }
    friend bool operator<=(const Tracked& lhs, const Tracked& rhs) {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ <= rhs.value_;
    }
    friend bool operator>=(const Tracked& lhs, const Tracked& rhs) {
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ >= rhs.value_;
    }
        
    friend Tracked operator+(const Tracked& lhs, const Tracked& rhs) {
        Tracked res("+", lhs.value_ + rhs.value_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, lhs.graph_id_, res.graph_id_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, res.graph_id_);
        return res;
    }

    friend Tracked operator-(const Tracked& lhs, const Tracked& rhs) {
        Tracked res("-", lhs.value_ - rhs.value_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, lhs.graph_id_, res.graph_id_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, res.graph_id_);
        return res;
    }

    friend Tracked operator*(const Tracked& lhs, const Tracked& rhs) {
        Tracked res("*", lhs.value_ * rhs.value_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, lhs.graph_id_, res.graph_id_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, res.graph_id_);
        return res;
    }

    friend Tracked operator/(const Tracked& lhs, const Tracked& rhs) {
        Tracked res("/", lhs.value_ / rhs.value_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, lhs.graph_id_, res.graph_id_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, res.graph_id_);
        return res;
    }

    // Tracked op T
    friend bool operator==(const Tracked& lhs, const T& rhs) {
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ == rhs;
    }
    friend bool operator!=(const Tracked& lhs, const T& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ != rhs; }
    friend bool operator<(const Tracked& lhs, const T& rhs)  { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ <  rhs; }
    friend bool operator>(const Tracked& lhs, const T& rhs)  { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ >  rhs; }
    friend bool operator<=(const Tracked& lhs, const T& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ <= rhs; }
    friend bool operator>=(const Tracked& lhs, const T& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs.value_ >= rhs; }

    // T op Tracked
    friend bool operator==(const T& lhs, const Tracked& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs == rhs.value_; }
    friend bool operator!=(const T& lhs, const Tracked& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs != rhs.value_; }
    friend bool operator<(const T& lhs, const Tracked& rhs)  { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs <  rhs.value_; }
    friend bool operator>(const T& lhs, const Tracked& rhs)  { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs >  rhs.value_; }
    friend bool operator<=(const T& lhs, const Tracked& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs <= rhs.value_; }
    friend bool operator>=(const T& lhs, const Tracked& rhs) { 
        uint64_t lit = GraphBuilder::instance().make_node("#lit");
        uint64_t cmp_id = GraphBuilder::instance().make_node("#cmp");
        GraphBuilder::instance().add_event(Event::READ, lit, cmp_id);
        GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id);
        GraphBuilder::instance().mark_dead(lit);
        GraphBuilder::instance().mark_dead(cmp_id);
        return lhs >= rhs.value_; }

    Tracked& operator+=(const Tracked& rhs) { value_ += rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator-=(const Tracked& rhs) { value_ -= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator*=(const Tracked& rhs) { value_ *= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }
    Tracked& operator/=(const Tracked& rhs) { value_ /= rhs.value_; GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, graph_id_); return *this; }

    Tracked& operator+=(const T& rhs) { 
        Tracked unnamed_operand(rhs);
        value_ += rhs;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, unnamed_operand.graph_id_, graph_id_);
        return *this; 
    }

    Tracked& operator-=(const T& rhs) {
        Tracked unnamed_operand(rhs);
        value_ -= rhs;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, unnamed_operand.graph_id_, graph_id_);
        return *this; 
    }

    Tracked& operator*=(const T& rhs) { 
        Tracked unnamed_operand(rhs);
        value_ *= rhs;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, unnamed_operand.graph_id_, graph_id_);
        return *this; 
    }

    Tracked& operator/=(const T& rhs) {
        Tracked unnamed_operand(rhs);
        value_ /= rhs;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, unnamed_operand.graph_id_, graph_id_);
        return *this; 
    }

    Tracked& operator++() {
        uint64_t old_id = graph_id_;
        ++value_;
        uint64_t new_id = GraphBuilder::instance().make_node(name_.empty() ? "#tmp" : name_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, old_id, new_id);
        GraphBuilder::instance().mark_dead(old_id);
        graph_id_ = new_id;
        return *this;
    }
    Tracked operator++(int) {
        Tracked tmp(*this); 
        ++(*this);         
        return tmp;
    }
    Tracked& operator--() {
        uint64_t old_id = graph_id_;
        --value_;
        uint64_t new_id = GraphBuilder::instance().make_node(name_.empty() ? "#tmp" : name_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, old_id, new_id);
        GraphBuilder::instance().mark_dead(old_id);
        graph_id_ = new_id;
        return *this;
    }
    Tracked operator--(int) {
        Tracked tmp(*this);
        --(*this);
        return tmp;
    }


};

template<typename T>
std::istream& operator>>(std::istream& is, Tracked<T>& t) {
    is >> t.value_;
    uint64_t in_id = GraphBuilder::instance().make_node("#input");
    GraphBuilder::instance().add_event(Event::COPY_ASSIGN, in_id, t.graph_id_);
    GraphBuilder::instance().mark_dead(in_id);
    return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Tracked<T>& t) {
    uint64_t out_id = GraphBuilder::instance().make_node("#output");
    GraphBuilder::instance().add_event(Event::READ, t.graph_id_, out_id);
    GraphBuilder::instance().mark_dead(out_id);
    return os << t.value_;
}
