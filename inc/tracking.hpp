#pragma once
#include <string_view>
#include <iostream>
#include <utility>


#define TRACK_VAR(T, name, ...) \
    Tracked<T> name{#name, __VA_ARGS__, }


static inline void log(const std::string_view name, const char msg[]) {
    if (!name.empty()) {
        std::cerr << name << ": " << msg << '\n';
    }
}

template <typename T>
struct Tracked {
    std::string_view name_{};
    T value_;

    Tracked() = delete;

    ~Tracked() { log(name_, "dtor"); }

    Tracked(const Tracked& other)
        : name_(other.name_), value_(other.value_) {
            log(name_, "copy ctor");
    }

    Tracked(std::string_view name, T& value)
        : name_(name), value_(value) {
        log(name_, "copy ctor");
    }

    Tracked(T& value)
        : value_(value) {
        log(name_, "copy ctor");
    }

    Tracked& operator=(const Tracked& other) {
        log(name_, "copy assign");
        value_ = other.value_;
        return *this;
    }

    Tracked(Tracked&& other) noexcept
        : name_(other.name_), value_(std::move(other.value_)) {
        log(name_, "move ctor");
    }

    Tracked(T&& value) noexcept
        : value_(std::move(value)) {
        log(name_, "move ctor");
    }

    Tracked(std::string_view name, T&& value) noexcept
        : name_(name), value_(std::move(value)) {
        log(name_, "move ctor");
    }

    Tracked& operator=(Tracked&& other) noexcept {
        log(name_, "move assign");
        value_ = std::move(other.value_);
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

