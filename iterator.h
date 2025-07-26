#pragma once
#include <iostream>




template <typename T>
class iterator {
    T* ptr;
    
    public:

    iterator(T* ptr_) : ptr(ptr_) {}
    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = default;

    T& operator*() const noexcept { 
        return *ptr; 
    }

    // Компилятор сам допишет ещё одну стрелочку к возвращаемому объекту
    T* operator->() const noexcept {
        return ptr;
    }


    iterator& operator++() noexcept { 
        ++ptr;
        return *this;
    }

    iterator operator++(int) noexcept {
        iterator copy = *this;
        ++ptr;
        return copy;
    }

    iterator& operator--() noexcept { 
        --ptr;
        return *this;
    }

    iterator operator--(int) noexcept {
        iterator copy = *this;
        --ptr;
        return copy;
    }


    bool operator==(const iterator& other) const noexcept {
        return ptr == other.ptr;
    }

    bool operator!=(const iterator& other) const noexcept {
        return ptr != other.ptr;
    }

    bool operator>(const iterator& other) const noexcept {
        return ptr > other.ptr;
    }

    bool operator>=(const iterator& other) const noexcept {
        return ptr >= other.ptr;
    }

    bool operator<(const iterator& other) const noexcept {
        return ptr < other.ptr;
    }

    bool operator<=(const iterator& other) const noexcept {
        return ptr <= other.ptr;
    }

    ~iterator() = default;
};