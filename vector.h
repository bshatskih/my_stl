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
    using allocator_type = Alloc;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<Alloc>::pointer;
    using const_pointer = std::allocator_traits<Alloc>::const_pointer;
    using iterator = ::base_iterator<false, T>;
    using const_iterator = ::base_iterator<true, T>;
    using reverse_iterator = ::reverse_iterator<iterator>;
    using const_reverse_iterator = ::reverse_iterator<const_iterator>;



    // Member functions

    vector() : sz_(0), cap_(0), arr_(nullptr) {}

    ~vector() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_t i = 0; i < sz_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

        arr_ = nullptr;
        sz_ = 0;
        cap_ = 0;
    }


    void assign(size_t count, const T& value) {
        if (cap_ >= count) {
            if (sz_ >= count) {
                for (size_t i = 0; i < sz_; ++i) {
                    arr_[i] = value;
                }
                for (size_t i = count; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                }
            } else {
                for (size_t i = 0; i < sz_; ++i) {
                    arr_[i] = value;
                }
                for (size_t i = sz_; i < count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, value);
                }
            }
            sz_ = count;
        } else {
            vector tmp(count, value, alloc_);
            swap(*this, tmp);
        }
    }

    template <typename InputIt>
    requires (!std::is_integral_v<InputIt>)
    void assign(InputIt first, InputIt last) {
        if (first == last) {
            clear();
            return;
        }

        if constexpr (std::derived_from<typename std::iterator_traits<InputIt>::iterator_category, std::forward_iterator_tag>) {
            size_t count = std::distance(first, last);

            std::optional<vector<value_type>> copy_range;
            const_pointer val_ptr = std::addressof(*first);
            bool value_inside_vector = arr_ && (std::less<const T*>()(val_ptr, arr_ + sz_) && std::greater_equal<const T*>()(val_ptr, arr_));
            if (value_inside_vector) {
                copy_range.emplace();
                for (InputIt it = first; it != last; ++it) {
                    copy_range->push_back(*it);
                }
                val_ptr = copy_range->data();
            }

            if (cap_ >= count) {
                if (sz_ >= count) {
                    for (size_t i = 0; i < sz_; ++i) {
                        arr_[i] = *val_ptr;
                        ++val_ptr;
                    }
                    for (size_t i = count; i < sz_; ++i) {
                        std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                    }
                } else {
                    for (size_t i = 0; i < sz_; ++i) {
                        arr_[i] = *val_ptr;
                        ++val_ptr;
                    }
                    for (size_t i = sz_; i < count; ++i) {
                        std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *val_ptr);
                        ++val_ptr;
                    }
                }
                sz_ = count;
            } else {
                vector tmp(first, last, alloc_);
                swap(*this, tmp);
            }
        } else {
            clear();
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        }
    }

    void assign(std::initializer_list<T> ilist) {
        typename std::initializer_list<T>::iterator it = ilist.begin();
        size_t count = ilist.size();
        if (cap_ >= count) {
            if (sz_ >= count) {
                for (size_t i = 0; i < sz_; ++i) {
                    arr_[i] = *it;
                    ++it;
                }
                for (size_t i = count; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                }
            } else {
                for (size_t i = 0; i < sz_; ++i) {
                    arr_[i] = *it;
                    ++it;
                }
                for (size_t i = sz_; i < count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *it);
                    ++it;
                }
            }
            sz_ = count;
        } else {
            vector tmp(ilist, alloc_);
            swap(*this, tmp);
        }
    }

    allocator_type get_allocator() const {
        return alloc_;
    }

    constexpr vector& operator=(const vector& other) {
        if (this == &other) {
            return *this;
        }

        Alloc new_alloc = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value ? other.alloc_ : alloc_;

        if (alloc_ == other.alloc_ && (std::is_nothrow_copy_assignable<T>::value || noexcept(T(std::declval<const T&>())))) {
            if (cap_ >= other.cap_) {
                if (sz_ >= other.sz_) {
                    for (size_t i = 0; i < other.sz_; ++i) {
                        arr_[i] = other.arr_[i];
                    }
                    for (size_t i = other.sz_; i < sz_; ++i) {
                        std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                    }
                } else {
                    for (size_t i = 0; i < sz_; ++i) {
                        arr_[i] = other.arr_[i];
                    }
                    for (size_t i = sz_; i < other.sz_; ++i) {
                        std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, other.arr_[i]);
                    }
                }
            } else {
                for (size_t i = 0; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                }
                reserve(other.cap_);
                for (size_t i = 0; i < other.sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, other.arr_[i]);
                }
            }
            sz_ = other.sz_;

        } else {
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
        }
        return *this;
    }

    constexpr  vector& operator=(vector&& other) noexcept(std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value || std::allocator_traits<Alloc>::is_always_equal::value) {
        if (this == &other) {
            return *this;
        }

        constexpr bool propagate = std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value;
        
        if (!propagate && alloc_ != other.alloc_) {
            clear();
            
            if (cap_ < other.sz_) {
                reserve(other.sz_);
            }
            
            for (size_t i = 0; i < other.sz_; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, std::move(other.arr_[i]));
            }
            sz_ = other.sz_;
            
            other.clear();
        }
        else {
            for (size_t i = 0; i < sz_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);
            
            arr_ = other.arr_;
            sz_ = other.sz_;
            cap_ = other.cap_;
            
            if constexpr (propagate) {
                alloc_ = std::move(other.alloc_);
            }

            other.arr_ = nullptr;
            other.sz_ = 0;
            other.cap_ = 0;
        }
        
        return *this;
    }

    vector& operator=(std::initializer_list<value_type> ilist) {
        typename std::initializer_list<value_type>::iterator it = ilist.begin();
        if (cap_ > ilist.size()) {
            if (sz_ >= ilist.size()) {
                for (size_t i = 0; i < ilist.size(); ++i) {
                    arr_[i] = *it;
                    ++it;
                }

                for (size_t i = ilist.size(); i < sz_; ++i) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                }
            } else {
                clear();
                for (size_t i = 0; i < ilist.size(); ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *it);
                    ++it;
                }
            }
        } else {
            vector tmp;
            tmp.reserve(ilist.size());
            for (const auto& item : ilist) {
                tmp.push_back(item);
            }
            swap(*this, tmp);
        }
        sz_ = ilist.size();
        return *this;
    }

    // Element access
    constexpr reference at(size_t n) {
        if (n >= size()) {
            throw std::out_of_range();
        }
        return arr_[n];
    }

    constexpr const_reference at(size_t n) const {
        if (n >= size()) {
            throw std::out_of_range();
        }
        return arr_[n];
    }

    constexpr reference operator[](size_t n) {
        return arr_[n];
    }

    constexpr const_reference operator[](size_t n) const{
        return arr_[n];
    }

    constexpr reference front() {
        return arr_[0];
    }

    constexpr const_reference front() const {
        return arr_[0];
    }

    constexpr reference back() {
        return arr_[sz_ - 1];
    }

    constexpr const_reference back() const {
        return arr_[sz_ - 1];
    }

    constexpr pointer data() noexcept {
        return sz_ == 0 ? nullptr : arr_;;
    }

    constexpr const_pointer data() const noexcept {
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

    constexpr void reserve(size_t new_cap) {
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
    
    constexpr void shrink_to_fit() {
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
  
    constexpr size_t max_size() const noexcept {
        return std::allocator_traits<Alloc>::max_size(alloc_);
    }



    // Modifiers
    constexpr void clear() noexcept {
        for (size_t i = 0; i < sz_; ++i) {
            std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
        }
        sz_ = 0;
    }

    constexpr iterator insert(const_iterator pos, const T& value) {
        size_t index = pos - cbegin();

        std::optional<value_type> copy_val;
        const_pointer val_ptr = std::addressof(value);
        bool value_inside_vector = arr_ && (std::less<const T*>()(val_ptr, arr_ + sz_) && std::greater_equal<const T*>()(val_ptr, arr_));
        if (value_inside_vector) {
            copy_val.emplace(value);
            val_ptr = std::addressof(*copy_val);
        }

        if (sz_ == cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < index; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + index, *val_ptr);
                ++i;
                for (; i <= sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - 1]));
                }
            } catch (...)   {
                 for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t j = 0; j < sz_; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            for (size_t i = sz_; i > index; --i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, std::move_if_noexcept(arr_[i - 1]));
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i - 1);
            }
            std::allocator_traits<Alloc>::construct(alloc_, arr_ + index, *val_ptr);
        }
        ++sz_;
        return iterator(arr_ + index);
    }

    constexpr iterator insert(const_iterator pos, T&& value) {
        size_t index = pos - cbegin();
        T tmp = std::move(value);
        if (sz_ == cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < index; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + index, std::move(tmp));
                ++i;
                for (; i <= sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - 1]));
                }
            } catch (...)   {
                 for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t j = 0; j < sz_; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            for (size_t i = sz_; i > index; --i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, std::move_if_noexcept(arr_[i - 1]));
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i - 1);
            }
            std::allocator_traits<Alloc>::construct(alloc_, arr_ + index, std::move(tmp));
        }
        ++sz_;
        return iterator(arr_ + index);
    }

    constexpr iterator insert(const_iterator pos, size_type count, const T& value) {
        size_t index = pos - cbegin();

        std::optional<value_type> copy_val;
        const_pointer val_ptr = std::addressof(value);
        bool value_inside_vector = arr_ && (std::less<const T*>()(val_ptr, arr_ + sz_) && std::greater_equal<const T*>()(val_ptr, arr_));
        if (value_inside_vector) {
            copy_val.emplace(value);
            val_ptr = std::addressof(*copy_val);
        }


        if (sz_ + count > cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            if (sz_ + count > new_cap) {
                new_cap = sz_ + count;
            }
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < index; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }

                for (; i < index + count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, *val_ptr);
                }
                
                for (; i < sz_ + count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - count]));
                }
            } catch (...)   {
                 for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t j = 0; j < sz_; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            for (size_t i = sz_; i-- > index;) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i + count, std::move_if_noexcept(arr_[i]));
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }

            for (size_t i = index; i < index + count; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *val_ptr);
            }
        }
        sz_ += count;
        return iterator(arr_ + index);
    }

    template <typename InputIt>
    requires (!std::is_integral_v<InputIt>)
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_t index = pos - cbegin();

        if (first == last) {
            return iterator(arr_ + index);
        }

        if constexpr (std::derived_from<typename std::iterator_traits<InputIt>::iterator_category, std::forward_iterator_tag>) {
            size_t count = std::distance(first, last);

            std::optional<vector<value_type>> copy_range;
            const_pointer val_ptr = std::addressof(*first);
            bool value_inside_vector = arr_ && (std::less<const T*>()(val_ptr, arr_ + sz_) && std::greater_equal<const T*>()(val_ptr, arr_));
            if (value_inside_vector) {
                copy_range.emplace();
                for (InputIt it = first; it != last; ++it) {
                    copy_range->push_back(*it);
                }
                val_ptr = copy_range->data();
            }

            if (sz_ + count > cap_) {
                size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
                if (sz_ + count > new_cap) {
                    new_cap = sz_ + count;
                }
                T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

                size_t i = 0;
                try {
                    for (; i < index; ++i) {
                        std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                    }

                    for (; i < index + count; ++i) {
                        std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, *val_ptr);
                        ++val_ptr;
                    }
                    
                    for (; i < sz_ + count; ++i) {
                        std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - count]));
                    }
                } catch (...)   {
                    for (size_t j = 0; j < i; ++j) {
                        std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                    }
                    std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                    throw;
                }

                for (size_t j = 0; j < sz_; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

                arr_ = new_arr;
                cap_ = new_cap;
            } else {
                for (size_t i = sz_; i-- > index;) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i + count, std::move_if_noexcept(arr_[i]));
                    std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
                }

                for (size_t i = index; i < index + count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *val_ptr);
                    ++val_ptr;
                }
            }
            sz_ += count;
        } else {
            size_t inserted_count = 0;
            try {
                while (first != last) {
                    insert(begin() + index + inserted_count, *first);
                    ++inserted_count;
                    ++first;
                }
            } catch (...) {
                erase(begin() + index, begin() + index + inserted_count);
                throw;
            }
        }
        return iterator(arr_ + index);
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        size_t index = pos - cbegin();
        size_t count = ilist.size();
        typename std::initializer_list<T>::iterator it = ilist.begin();
        if (sz_ + count > cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            if (sz_ + count > new_cap) {
                new_cap = sz_ + count;
            }
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < index; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }

                for (; i < index + count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, *it);
                    ++it;
                }
                
                for (; i < sz_ + count; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - count]));
                }
            } catch (...)   {
                 for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t j = 0; j < sz_; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            for (size_t i = sz_; i-- > index;) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i + count, std::move_if_noexcept(arr_[i]));
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }

            for (size_t i = index; i < index + count; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, *it);
                ++it;
            }
        }
        sz_ += count;
        return iterator(arr_ + index);
    }

    constexpr void push_back(const T& value) {
        emplace_back(value);
    }

    constexpr void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename ... Args>
    reference emplace_back(Args&& ... args) {
        if (sz_ == cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;

            T*  new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);
            size_t i = 0;
            try {
                for (; i < sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + sz_, std::forward<Args>(args)...);
                ++i;
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
        return arr_[sz_ - 1];
    }

    template <typename... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args) {
        size_t index = pos - cbegin();
        if (sz_ == cap_) {
            size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
            T* new_arr = std::allocator_traits<Alloc>::allocate(alloc_, new_cap);

            size_t i = 0;
            try {
                for (; i < index; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
                }
                std::allocator_traits<Alloc>::construct(alloc_, new_arr + index, std::forward<Args>(args)...);
                ++i;
                for (; i <= sz_; ++i) {
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i - 1]));
                }
            } catch (...)   {
                 for (size_t j = 0; j < i; ++j) {
                    std::allocator_traits<Alloc>::destroy(alloc_, new_arr + j);
                }
                std::allocator_traits<Alloc>::deallocate(alloc_, new_arr, new_cap);
                throw;
            }

            for (size_t j = 0; j < sz_; ++j) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + j);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);

            arr_ = new_arr;
            cap_ = new_cap;
        } else {
            for (size_t i = sz_; i > index; --i) {
                std::allocator_traits<Alloc>::construct(alloc_, arr_ + i, std::move_if_noexcept(arr_[i - 1]));
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i - 1);
            }
            std::allocator_traits<Alloc>::construct(alloc_, arr_ + index, std::forward<Args>(args)...);
        }
        ++sz_;
        return iterator(arr_ + index);
    }

    constexpr void pop_back() {
        --sz_;
        std::allocator_traits<Alloc>::destroy(alloc_, arr_ + sz_);
        return;
    }

    constexpr void resize(size_t n, const T& value = T()) {
        if (n < sz_) {
            for (size_t i = n; i < sz_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, arr_ + i);
            }
        } else if (n <= cap_) {
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
                    std::allocator_traits<Alloc>::construct(alloc_, new_arr + i, std::move_if_noexcept(arr_[i]));
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
    }

    constexpr iterator erase(iterator pos) {
        pointer p = std::to_address(pos);
        pointer new_end = std::move(p + 1, arr_ + sz_, p);
        std::allocator_traits<Alloc>::destroy(alloc_, new_end);
        --sz_;
        return iterator(p);
    }

    constexpr iterator erase(const_iterator pos) {
        iterator mutable_pos = begin() + (pos - cbegin());
        pointer p = std::to_address(mutable_pos);
        pointer new_end = std::move(p + 1, arr_ + sz_, p);
        std::allocator_traits<Alloc>::destroy(alloc_, new_end);
        --sz_;
        return mutable_pos;
    }

    constexpr iterator erase(iterator first, iterator last) {
        if (first == last) {
            return first;
        }

        pointer p_first = std::to_address(first);
        pointer p_last = std::to_address(last);
        size_type count = p_last - p_first;

        pointer new_end = std::move(p_last, arr_ + sz_, p_first);

        for (pointer p = new_end; p != arr_ + sz_; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc_, p);
        }
        sz_ -= count;
        return first;
    }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        iterator mutable_first = begin() + (first - cbegin());
        iterator mutable_last = end() + (last - cend());

        if (first == last) {
            return mutable_first;
        }

        pointer p_first = std::to_address(mutable_first);
        pointer p_last = std::to_address(mutable_last);
        size_type count = p_last - p_first;

        pointer new_end = std::move(p_last, arr_ + sz_, p_first);

        for (pointer p = new_end; p != arr_ + sz_; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc_, p);
        }
        sz_ -= count;
        return mutable_first;
    }

    constexpr void swap(vector& other) noexcept(std::allocator_traits<Alloc>::propagate_on_container_swap::value || std::allocator_traits<Alloc>::is_always_equal::value) {
        std::swap(arr_, other.arr_);        
        std::swap(sz_, other.sz_);
        std::swap(cap_, other.cap_);
        
        if constexpr (std::allocator_traits<Alloc>::propagate_on_container_swap::value) {
            std::swap(alloc_, other.alloc_);
        } else {
            assert(alloc_ == other.alloc_ && "Swapping vectors with unequal allocators is undefined behavior");
        }
    }

};