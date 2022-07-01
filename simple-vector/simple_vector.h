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
        size_(size), capacity_(size), items_(new Type[size]{ 0 }) {
    }

    explicit SimpleVector(ReserveProxyObj obj) :
        size_(0u), capacity_(obj.GetVoid()), items_(new Type[obj.GetVoid()]{}) {
        obj.~ReserveProxyObj();
    }

    SimpleVector(size_t size, const Type& value) :
        size_(size), capacity_(size), items_(new Type[size]{ 0 }) {
        for (size_t i = 0; i != size; i++) {
            items_[i] = value;
        }
    }

    SimpleVector(std::initializer_list<Type> init) :
        size_(init.size()), capacity_(init.size()), items_(new Type[init.size()]{ 0 }) {
        int i = 0;
        for (auto init_unit = init.begin(); init_unit != init.end(); init_unit++) {
            items_[i] = *init_unit;
            i++;
        }
    }

    SimpleVector(const SimpleVector& other) {
        size_t other_size = other.GetSize();
        SimpleVector tmp(other_size);
        int i = 0;
        for (auto it = other.begin(); it != other.end(); it++) {
            tmp.items_[i] = *it;
            i++;
        }
        swap(tmp);
    }

    SimpleVector(SimpleVector&& other) noexcept :
        size_(other.size_), capacity_(other.capacity_), items_(std::move(other.items_)) {
        other.size_ = 0;
        other.capacity_ = 0;
        assert(other.items_.Release() == begin());
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
        return (size_ == 0 ? 1 : 0);
    }

    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return At(index);
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
        if (new_size <= capacity_ && new_size < size_) {
            size_ = new_size;
        }
        else if (new_size <= capacity_ && new_size > size_) {
            ArrayPtr<Type> new_size_(new Type[new_size]{ 0 });
            int i = 0;
            for (auto it = &new_size_[0]; it != &new_size_[size_]; it++) {
                std::swap(*it, items_[i]);
                i++;
            }
            items_.swap(new_size_);
            size_ = new_size;
        }
        else if (new_size <= capacity_ && new_size == size_) {
            return;
        }
        else {
            ArrayPtr<Type> new_size_(new Type[new_size]{ 0 });
            int i = 0;
            for (auto it = &new_size_[0]; it != &new_size_[size_]; it++) {
                std::swap(*it, items_[i]);
                i++;
            }

            items_.swap(new_size_);
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        else {
            ArrayPtr<Type> new_vec_(new Type[new_capacity]{ 0 });
            int i = 0;
            for (auto it = &new_vec_[0]; it != &new_vec_[size_]; it++) {
                std::swap(*it, items_[i]);
                i++;
            }
            size_ = static_cast<size_t>(i);
            capacity_ = new_capacity;
            items_.swap(new_vec_);
        }
    }

    Iterator begin() noexcept {
        return items_.Get();
    }

    Iterator end() noexcept {
        return &items_[size_];
    }

    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    ConstIterator end() const noexcept {
        return &items_[size_];
    }

    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    ConstIterator cend() const noexcept {
        return &items_[size_];
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
            if (size_ < capacity_) {
                items_[size_] = item;
                size_++;
            }
            else {
                Reserve(capacity_ * 2);
                items_[size_] = item;
                size_++;
            }
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
            if (size_ < capacity_) {
                std::swap(item, items_[size_]);
                size_++;
            }
            else {
                Reserve(capacity_ * 2);
                std::swap(item, items_[size_]);
                size_++;
            }
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        if (IsEmpty()) {
            PushBack(value);
            return begin();
        }
        else if (pos == end()) {
            PushBack(value);
            return (end() - 1);
        }
        else if (size_ < capacity_) {
            std::copy_backward(pos, end(), end() + 1);
            *pos = value;
            size_++;

            return pos;
        }
        else {
            auto distance = std::distance(begin(), pos);
            Reserve(capacity_ * 2);

            std::copy_backward(begin() + distance, end(), end() + 1);
            *(begin() + distance) = value;
            size_++;

            return begin() + distance;
        }
    }

    // Перемещающий Insert
    Iterator Insert(Iterator pos, Type&& value) {
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

            std::copy_backward(move_pos, move_end, end() + 1);
            std::swap(value, *pos);
            size_++;

            return pos;
        }
        else {
            auto distance = std::distance(begin(), pos);
            Reserve(capacity_ * 2);
            
            auto move_pos = std::make_move_iterator(begin() + distance);
            auto move_end = std::make_move_iterator(end());

            std::copy_backward(move_pos, move_end, end() + 1);
            std::swap(value, *(begin() + distance));
            size_++;

            return (begin() + distance);
        }
    }

    void PopBack() noexcept {
        if (IsEmpty()) {
            // считаю, что сюда лучше вставить ассерт
            // вылет ошибки может подсказать программисту
            // что у него что-то идет не так
            assert(!IsEmpty());
            return;
        }
        else {
            size_--;
        }
    }

    Iterator Erase(Iterator pos) {
        if (IsEmpty()) {
            // считаю, что сюда лучше вставить ассерт
            // вылет ошибки может подсказать программисту
            // что у него что-то идет не так
            assert(!IsEmpty());
            return begin();
        }
        else if (pos == (end() - 1)) {
            PopBack();
            return end();
        }
        else {

            auto move_pos_after = std::make_move_iterator(pos + 1);
            auto move_end = std::make_move_iterator(end());

            std::copy(move_pos_after, move_end, pos);
            size_--;

            return pos;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (IsEmpty()) {
            // считаю, что сюда лучше вставить ассерт
            // вылет ошибки может подсказать программисту
            // что у него что-то идет не так
            assert(!IsEmpty());
            return begin();
        }
        else if (pos == (cend() - 1)) {
            PopBack();
            return end();
        }
        else {

            auto distance = std::distance(cbegin(), pos);
            Iterator it_pos = begin() + distance;

            std::copy((it_pos + 1), end(), it_pos);
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
    return std::equal(lhs.begin(), lhs.end(), rhs.begin()) && lhs.GetSize() == rhs.GetSize();
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
    return (lhs == rhs || lhs < rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs == rhs || lhs > rhs);
}