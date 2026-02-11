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
    ~Tracked() { GraphBuilder::instance().mark_dead(graph_id_); }
    Tracked() = delete;

    // Copy constructors
    Tracked(const Tracked& other): name_(other.name_), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, other.graph_id_, graph_id_);
    }
    Tracked(std::string_view name, const Tracked& other) : name_(name), value_(other.value_) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::COPY_CONSTRUCT, other.graph_id_, graph_id_);
    }
    Tracked(std::string_view name, const T& value) : name_(name), value_(value) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
    }
    Tracked(const T& value) : value_(value) {
        graph_id_ = GraphBuilder::instance().make_node();
    }

    // Move constructors
    Tracked(Tracked&& other) : name_(std::move(other.name_)), value_(std::move(other.value_)) {
        graph_id_ = GraphBuilder::instance().make_node(name_);
        GraphBuilder::instance().add_event(Event::MOVE_CONSTRUCT, other.graph_id_, graph_id_);
        GraphBuilder::instance().mark_dead(other.graph_id_);
    }
    Tracked(T&& value) : value_(std::move(value)) {
        graph_id_ = GraphBuilder::instance().make_node();
    }
        
    // Copy assignment
    Tracked& operator=(const Tracked& other) {
        value_ = other.value_;
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, other.graph_id_, graph_id_);
        return *this;
    }

    // Move assignment
    Tracked& operator=(Tracked&& other) noexcept {
        value_ = std::move(other.value_);
        GraphBuilder::instance().add_event(Event::MOVE_ASSIGN, other.graph_id_, graph_id_);
        GraphBuilder::instance().mark_dead(other.graph_id_);
        return *this;
    }

    // Cast
    operator T() const { return value_;}

    // Operators

    #define BUILD_COMPARE(op)                                                       \
        friend bool op(const Tracked& lhs, const Tracked& rhs) {                    \
            uint64_t cmp_id = GraphBuilder::instance().make_node();                 \
            GraphBuilder::instance().add_event(Event::READ, lhs.graph_id_, cmp_id); \
            GraphBuilder::instance().add_event(Event::READ, rhs.graph_id_, cmp_id); \
            GraphBuilder::instance().mark_dead(cmp_id);                             \
            return lhs.value_ == rhs.value_;                                        \
        }                                                                           \
        
        BUILD_COMPARE(operator==)
        BUILD_COMPARE(operator!=)
        BUILD_COMPARE(operator<)
        BUILD_COMPARE(operator>)
        BUILD_COMPARE(operator>=)
        BUILD_COMPARE(operator<=)
    #undef BUILD_COMPARE
    
    
    #define BUILD_ARITHMETIC(op) \
        friend Tracked op(const Tracked& lhs, const Tracked& rhs) {
        Tracked res(lhs.value_ + rhs.value_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, lhs.graph_id_, res.graph_id_);
        GraphBuilder::instance().add_event(Event::COPY_ASSIGN, rhs.graph_id_, res.graph_id_);
        return res;
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

    template<typename U>
    friend std::istream& operator>>(std::istream& is, Tracked<U>& t);
    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Tracked<U>& t);


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
