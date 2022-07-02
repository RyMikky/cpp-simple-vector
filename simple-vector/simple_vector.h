#pragma once

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <utility>

#include "array_ptr.h"


class ReserveProxyObj {

public:
    ReserveProxyObj() noexcept = default;

    explicit ReserveProxyObj(size_t capacity) : void_capacity_(capacity) {
    }

    ~ReserveProxyObj() noexcept = default;

    size_t GetVoid() {
        return void_capacity_;
    }

private:
    size_t void_capacity_ = 0u;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : 
        size_(size), capacity_(size), items_(ArrayPtr<Type>(size)) {
    }

    explicit SimpleVector(ReserveProxyObj obj) :
        size_(0u), capacity_(obj.GetVoid()), items_(ArrayPtr<Type>(obj.GetVoid())) {
    }

    SimpleVector(size_t size, const Type& value) :
        size_(size), capacity_(size), items_(ArrayPtr<Type>(size)) {
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init) :
        size_(init.size()), capacity_(init.size()), items_(ArrayPtr<Type>(init.size())) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) : 
        size_(other.GetSize()), capacity_(other.GetSize()), items_(ArrayPtr<Type>(other.GetSize())) {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector&& other) noexcept :
        size_(other.size_), capacity_(other.capacity_), items_(std::move(other.items_)) {
        other.size_ = 0;
        other.capacity_ = 0;
    } 

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this == &rhs) { 
            return *this;
        }
        else {
            SimpleVector tmp(rhs);
            swap(tmp);
            return *this;
        }
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return !size_;
    }

    Type& operator[](size_t index) noexcept {
        assert(index >= 0);
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index >= 0);
        assert(index < size_);
        return const_cast<Type>(items_[index]);
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index is Out of Range");
        }
        else {
            return items_[index];
        }
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index is Out of Range");
        }
        else {
            return items_[index];
        }
    }

    void Clear() noexcept {
        size_ = 0u;
    }

    void Resize(size_t new_size) {
        if (new_size == size_) {
            return;
        }
        else if (new_size < size_) {
            size_ = new_size;
        }
        else {
            if (new_size <= capacity_) {
                ArrayPtr<Type> new_size_(new Type[new_size]{ 0 });
                std::move(begin(), end(), new_size_.Get());

                items_.swap(new_size_);
                size_ = new_size;
            }
            else {
                ArrayPtr<Type> new_size_(new Type[new_size]{ 0 });
                std::move(begin(), end(), new_size_.Get());

                items_.swap(new_size_);
                size_ = new_size;
                capacity_ = new_size;
            }
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        else {
            ArrayPtr<Type> new_vec_(new Type[new_capacity]{ 0 });
            std::move(begin(), end(), new_vec_.Get());

            capacity_ = new_capacity;
            items_.swap(new_vec_);
        }
    }

    Iterator begin() noexcept {
        return items_.Get();
    }

    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }

    // Копирующий Push_Back
    void PushBack(const Type& item) {
        if (IsEmpty() && capacity_ == 0) {
            ArrayPtr<Type> new_size_vec_(new Type[1]{ item });
            items_.swap(new_size_vec_);
            size_ = 1;
            capacity_ = size_;
        }
        else {
            if (size_ >= capacity_) {
                Reserve(capacity_ * 2);
            }
            items_[size_] = item;
            size_++;
        } 
    }

    // Перемещающий Push_Back
    void PushBack(Type&& item) {
        if (IsEmpty() && capacity_ == 0) {
            ArrayPtr<Type> new_size_vec_(new Type[1]{0});
            std::swap(item, new_size_vec_[0]);
            items_.swap(new_size_vec_);
            size_ = 1;
            capacity_ = size_;
        }
        else {
            if (size_ >= capacity_) {
                Reserve(capacity_ * 2);
            }
            items_[size_] = std::move(item);
            size_++;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {

        assert(pos >= cbegin());
        assert(pos <= cend());

        if (IsEmpty()) {
            PushBack(value);
            return begin();
        }
        else if (pos == end()) {
            PushBack(value);
            return (end() - 1);
        }
        else if (size_ < capacity_) {
            std::move_backward(pos, end(), end() + 1);
            *pos = value;
            size_++;

            return pos;
        }
        else {
            auto distance = std::distance(begin(), pos);
            Reserve(capacity_ * 2);

            std::move_backward(begin() + distance, end(), end() + 1);
            *(begin() + distance) = value;
            size_++;

            return begin() + distance;
        }
    }

    // Перемещающий Insert
    Iterator Insert(Iterator pos, Type&& value) {

        assert(pos >= begin());
        assert(pos <= end());

        if (IsEmpty()) {
            PushBack(std::move(value));
            return begin();
        }
        else if (pos == end()) {
            PushBack(std::move(value));
            return end() - 1;
        }
        else if (size_ < capacity_) {

            auto move_pos = std::make_move_iterator(pos);
            auto move_end = std::make_move_iterator(end());

            std::move_backward(move_pos, move_end, end() + 1);
            std::swap(value, *pos);
            size_++;

            return pos;
        }
        else {
            auto distance = std::distance(begin(), pos);
            Reserve(capacity_ * 2);
            
            auto move_pos = std::make_move_iterator(begin() + distance);
            auto move_end = std::make_move_iterator(end());

            std::move_backward(move_pos, move_end, end() + 1);
            std::swap(value, *(begin() + distance));
            size_++;

            return (begin() + distance);
        }
    }

    void PopBack() noexcept {
        assert(!IsEmpty());
        size_--;
    }

    Iterator Erase(Iterator pos) {

        assert(pos >= begin());
        assert(pos <= end());
        assert(!IsEmpty());

        if (pos == (end() - 1)) {
            PopBack();
            return end();
        }
        else {

            auto move_pos_after = std::make_move_iterator(pos + 1);
            auto move_end = std::make_move_iterator(end());

            std::move(move_pos_after, move_end, pos);
            size_--;

            return pos;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {

        assert(pos >= cbegin());
        assert(pos <= cend());
        assert(!IsEmpty());

        if (pos == (cend() - 1)) {
            PopBack();
            return end();
        }
        else {

            auto distance = std::distance(cbegin(), pos);
            Iterator it_pos = begin() + distance;

            std::move((it_pos + 1), end(), it_pos);
            size_--;

            return it_pos;
        }
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->size_, other.size_);
        items_.swap(other.items_);
    }

    //// отладночная функция для простых типов int/double/char/string
    //void PrintInLine() {
    //    for (int i = 0; i < size_; i++) {
    //        std::cout << items_[i] << " ";
    //    }
    //    std::cout << std::endl;
    //}

private:
    size_t size_ = 0u;
    size_t capacity_ = 0u;

    ArrayPtr<Type> items_;
};

template <typename Type>
void swap(SimpleVector<Type>& lhs, SimpleVector<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}


template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}