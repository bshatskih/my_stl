#pragma once
#include <iostream>
#include <ranges>
#include <cstdint>
#include "iterator.h"
#include "allocator.h"
#include "reverse_iterator.h"


template<typename T, typename Alloc = std::allocator<T>>
class vector {
    size_t sz_;
    size_t cap_;
    T* arr_;
    [[no_unique_address]] Alloc alloc_;

    public:

    using value_type = T;
    using allocator_type = std::allocator<T>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<Alloc>::pointer;
    using const_pointer = std::allocator_traits<Alloc>::const_pointer;
    using iterator = ::base_iterator<false, T>;
    using const_iterator = ::base_iterator<true, T>;
    using reverse_iterator = ::reverse_iterator<iterator>;
    using const_reverse_iterator = ::reverse_iterator<const_iterator>;


    // Element access
    reference at(size_t n) {
        if (n >= size()) {
            throw std::out_of_range();
        }
        return arr_[n];
    }

    const_reference at(size_t n) const {
        if (n >= size()) {
            throw std::out_of_range();
        }
        return arr_[n];
    }

    reference operator[](size_t n) {
        return arr_[n];
    }

    const_reference operator[](size_t n) const{
        return arr_[n];
    }

    reference front() {
        return arr_[0];
    }

    const_reference front() const {
        return arr_[0];
    }

    reference back() {
        return arr_[sz_ - 1];
    }

    const_reference back() const {
        return arr_[sz_ - 1];
    }

    pointer data() noexcept {
        return sz_ == 0 ? nullptr : arr_;;
    }

    const_pointer data() const noexcept {
        return sz_ == 0 ? nullptr : arr_;;
    }


    // Iterators
    constexpr iterator begin() noexcept {
        return iterator(arr_);
    }

    constexpr iterator end() noexcept {
        return iterator(arr_ + sz_);
    }

    constexpr const_iterator begin() const noexcept {
        return const_iterator(arr_);
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator(arr_ + sz_);
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(arr_);
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(arr_ + sz_);
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept {    
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }


    // Capacity
    [[nodiscard]] constexpr size_t size() const noexcept {
        return sz_;
    }
    
    [[nodiscard]] constexpr size_t capacity() const noexcept {
        return cap_;
    }
    
    [[nodiscard]] constexpr bool empty() const noexcept {
        return sz_ == 0;
    }

    void reserve(size_t new_cap) {
        if (new_cap <= cap_) {
            return;
        }

        if (new_cap > max_size()) {
            throw std::length_error();
        }

        T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

        size_t i = 0;
        try {
            for (; i < sz_; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j  );
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
            throw;
        }

        for (size_t k = 0; k < sz_; ++k) {
            std::allocator_traits<Alloc>::destroy(alloc_, arr_ + k);
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

        arr_ = new_arr;
        cap_ = new_cap;
    }
    
    void shrink_to_fit() {
        if (sz_ == 0) {
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);
            arr_ = nullptr;
            cap_ = 0;
            return;
        }
        
        if (sz_ == cap_) {
            return;
        }

        T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, sz_);

        size_t i = 0;
        try {
            for (; i < sz_; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, sz_);
            throw;
        }

        for (size_t j = 0; j < sz_; ++j) {
            std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

        arr_ = new_arr;
        cap_ = sz_;
    }
  
    size_t max_size() const noexcept {
        return std::allocator_traits<Alloc>::max_size(alloc_);
    }

    vector() : sz_(0), cap_(0), arr_(nullptr) {}


    
    
    void resize(size_t n, const T& value = T()) {
        if (n < sz_) {
            for (size_t i = n; i < sz_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }
        } else if (n > sz_ && n <= cap_) {
            size_t i = sz_;
            try {
                for (; i < n; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, value);
                }
            } catch (...) {
                for (size_t j = sz_; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
                }
                throw;
            }
        } else {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            if (new_cap < n) {
                new_cap = n;
            }
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, arr_[i]);
                }

                for (; i < n; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, value);
                }
            } catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }

                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t k = 0; k < sz_; ++k) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + k);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);
            arr_ = new_arr;
            cap_ = new_cap;
        }

        
        sz_ = n;
        return;
    }
    
    vector& operator=(const vector& other) {
        Alloc new_alloc = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value ?
                          other.alloc_ : alloc_;

        T* new_arr = std::allocator_traits<Alloc>::allocate(new_alloc, other.cap_);
        
        size_t i = 0;
        try {
            for (; i < other.sz_; ++i) {
                std::allocator_traits<Alloc>::construct(new_alloc, new_arr + i, other[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                std::allocator_traits<Alloc>::destroy(new_alloc, new_arr + j);
            }
            std::allocator_traits<Alloc>::deallocate(new_alloc, new_arr, other.cap_);
            throw;
        }

        for (size_t k = 0; k < sz_; ++k) {
            std::allocator_traits<Alloc>::destroy(alloc_, arr_ + k);
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

        alloc_ = new_alloc;
        arr_ = new_arr;
        sz_ = other.sz_;
        cap_ = other.cap_; 
        return *this;
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename ... Args>
    void emplace_back(Args&& ... args) {
        if (sz_ == cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;

            T*  new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);
            size_t i = 0;
            try {
                for (; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + sz_, std::forward<Args>(args)...);
            } catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t k = 0; k < sz_; ++k) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + k);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);
            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            std::allocator_traits<Alloc>::construct(alloc_, arr_ + sz_, std::forward<Args>(args)...);
        }
        ++sz_;
    }

    void pop_back() {
        /*
        Было бы славно добавить проверку вектора на наличия в нём элементов, но поскольку в stl это не реализовано я также не буду этого делать,
        то есть pop_back, вызванная для пустого ветора приведёт к UB
        */
        --sz_;
        std::allocator_traits<Alloc>::destroy(alloc_, arr_ + sz_);
        return;
    }

};