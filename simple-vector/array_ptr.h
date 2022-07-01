#pragma once

#include <stdexcept> // содержит std::logic_error

template <typename Type>
class ArrayPtr {
public:

    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            ArrayPtr();
        }
        else {
            raw_ptr_ = new Type[size]{};
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&&) = default;

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&&) = default;

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
        return raw_ptr_ != nullptr ? true : false;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        Type* buffer = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = buffer;
    }

private:
    Type* raw_ptr_ = nullptr;
};
