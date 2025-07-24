#pragma once
#include <iostream>
#include <ranges>
#include <cstdint>


template<typename T>
class vector {
    size_t sz_;
    size_t cap_;
    T* arr_;

    public:

    vector();

    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] size_t capasity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    void reserve(size_t n);
    void resize(size_t n, const T& value = T());
    void push_back(const T& value);
    void pop_back();


    

    


    
};