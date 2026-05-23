#pragma once

#include <stdexcept>

// Simple Optional implementation for C++14 compatibility
template<typename T>
class Optional {
public:
    Optional() : has_value_(false) {}
    explicit Optional(const T& value) : value_(value), has_value_(true) {}

    Optional& operator=(const T& value) {
        value_ = value;
        has_value_ = true;
        return *this;
    }

    bool has_value() const { return has_value_; }

    T& value() {
        if (!has_value_) throw std::runtime_error("Optional does not contain a value");
        return value_;
    }

    const T& value() const {
        if (!has_value_) throw std::runtime_error("Optional does not contain a value");
        return value_;
    }

    T value_or(const T& default_value) const {
        return has_value_ ? value_ : default_value;
    }

    T* operator->() { return has_value_ ? &value_ : nullptr; }
    const T* operator->() const { return has_value_ ? &value_ : nullptr; }

    T& operator*() { return value_; }
    const T& operator*() const { return value_; }

    explicit operator bool() const { return has_value_; }

private:
    T value_;
    bool has_value_;
};
