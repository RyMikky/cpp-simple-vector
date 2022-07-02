#pragma once

#include <stdexcept>
#include <utility>

template <typename Type>
class ArrayPtr {
public:

    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        raw_ptr_ = new Type[size]{};  
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) noexcept : raw_ptr_(other.raw_ptr_) {
        other.raw_ptr_ = nullptr;
    };

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (&other == this)
            return *this;

        delete raw_ptr_;

        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr; 

        return *this;
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* returned_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return returned_ptr;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return raw_ptr_;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(*this, other);
    }

private:
    Type* raw_ptr_ = nullptr;
};
