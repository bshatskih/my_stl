#pragma once
#include <iostream>


/*
    Поскольку реализация const_iterator почти полностью повторяет обычный iterator, за исключением того, что мы пишем const T* ptr и в 
    методах operator*() и operator->() возвращать const T& и const T* соответственно, то неоптимально было бы заниматься копипасном, 
    решением будет использование шаблонного класса base_iterator
*/


template <bool IsConst, typename T, typename U>
struct conditional {
    using type = U;
};


template <typename T, typename U>
struct conditional<true, T, U> {
    using type = T;
};


template <bool IsConst, typename T, typename U>
using conditional_t = typename conditional<IsConst, T,  U>::type;




template <bool IsConst, typename T>
class base_iterator {
    conditional_t<IsConst, const T*, T*> ptr;

 public:

    /*
    Эта строка говорит компилятору: Я разрешаю всем инстанциациям шаблона base_iterator<B, U> доступ к моим приватным членам
    */
    template <bool B, typename U>
    friend class base_iterator;

    base_iterator() : ptr(nullptr) {}
    base_iterator(T* ptr_) : ptr(ptr_) {}

    template <bool B = IsConst, typename = std::enable_if_t<B>>
    base_iterator(const base_iterator<false, T>& other) : ptr(other.ptr) {}
    base_iterator(const base_iterator&) = default;


    template <bool B = IsConst, typename = std::enable_if_t<B>>
    base_iterator& operator=(const base_iterator<false, T>& other) {
        ptr = other.ptr;
        return *this;
    }
    base_iterator& operator=(const base_iterator&) = default;


    conditional_t<IsConst, const T&, T&> operator*() const noexcept { 
        return *ptr; 
    }

    // Компилятор сам допишет ещё одну стрелочку к возвращаемому объекту
    conditional_t<IsConst, const T*, T*> operator->() const noexcept {
        return ptr;
    }


    base_iterator& operator++() noexcept { 
        ++ptr;
        return *this;
    }

    base_iterator operator++(int) noexcept {
        base_iterator copy = *this;
        ++ptr;
        return copy;
    }

    base_iterator& operator--() noexcept { 
        --ptr;
        return *this;
    }

    base_iterator operator--(int) noexcept {
        base_iterator copy = *this;
        --ptr;
        return copy;
    }


    bool operator==(const base_iterator& other) const noexcept {
        return ptr == other.ptr;
    }

    bool operator!=(const base_iterator& other) const noexcept {
        return ptr != other.ptr;
    }

    bool operator>(const base_iterator& other) const noexcept {
        return ptr > other.ptr;
    }

    bool operator>=(const base_iterator& other) const noexcept {
        return ptr >= other.ptr;
    }

    bool operator<(const base_iterator& other) const noexcept {
        return ptr < other.ptr;
    }

    bool operator<=(const base_iterator& other) const noexcept {
        return ptr <= other.ptr;
    }

    ~base_iterator() = default;
};