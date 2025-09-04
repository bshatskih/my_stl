#pragma once
#include <iostream>
#include <type_traits>
#include <memory>


template <typename T>
struct Deleter {
    using pointer = T*;
    using element_type = T;

    constexpr Deleter() noexcept = default;

    template <typename U>
    requires std::convertible_to<U*, T*>
    Deleter(const Deleter<U>&) noexcept {}

    void operator()(pointer ptr) const noexcept {
        static_assert(sizeof(T) > 0);
        delete ptr;
    }
};  

template <typename T>
struct Deleter<T[]> {
    using pointer = T*;
    using element_type = T;

    constexpr Deleter() noexcept = default;

    template <typename U>
    requires std::convertible_to<U(*)[], T(*)[]>
    Deleter(const Deleter<U[]>&) noexcept {}

    void operator()(pointer ptr) const noexcept {
        static_assert(sizeof(T) > 0);
        delete[] ptr;
    }
};




template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
    T* ptr;
    [[no_unique_address]] Deleter deleter;

public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    // Constructors

    constexpr unique_ptr() noexcept : ptr(nullptr) {}

    constexpr unique_ptr(std::nullptr_t) noexcept : ptr(nullptr) {}

    explicit constexpr unique_ptr(pointer p) noexcept : ptr(p) {}

    explicit constexpr unique_ptr(pointer p, const Deleter& d) noexcept : ptr(p), deleter(d) {};

    constexpr unique_ptr(pointer p, Deleter&& d) noexcept(std::is_nothrow_move_constructible_v<Deleter>) : ptr(p), deleter(std::move(d)) {};

    constexpr unique_ptr(std::nullptr_t, const Deleter& d) noexcept(std::is_nothrow_copy_constructible_v<Deleter>) : ptr(nullptr), deleter(d) {};
    
    constexpr unique_ptr(std::nullptr_t, Deleter&& d) noexcept(std::is_nothrow_move_constructible_v<Deleter>) : ptr(nullptr), deleter(std::move(d)) {};

    // Используем std::forward вместо std::move, чтобы сохранить значение категории (lvalue/rvalue) переданного объекта deleter
    constexpr unique_ptr(unique_ptr&& other) noexcept(std::is_nothrow_move_constructible_v<Deleter>)
    requires std::is_move_constructible_v<Deleter> 
    : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    } 

    template <typename U, typename E>
    requires std::convertible_to<U*, T*> && std::constructible_from<Deleter, E&&>
    constexpr unique_ptr(unique_ptr<U, E>&& other) noexcept(std::is_nothrow_constructible_v<Deleter, E&&>) {
        unique_ptr tmp(other.release(), std::forward<E>(other.get_deleter()));
        tmp.swap(*this);
    }

    unique_ptr(const unique_ptr&) = delete; 




    // Destructor

    ~unique_ptr() noexcept {
        if (ptr) {
            deleter(ptr);
        }
    }



    // Assignment

    unique_ptr& operator=(const unique_ptr&) = delete;    

    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept(std::is_nothrow_move_assignable_v<Deleter>) {
        if (this == &other) {
            return *this;
        }
        unique_ptr tmp(std::move(other));
        this->swap(tmp);               
        return *this;
    }

    constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template <typename U, typename E>
    requires std::is_constructible_v<U*, T*> && std::is_constructible_v<Deleter, E&&>
    constexpr unique_ptr& operator=(unique_ptr<U, E>&& other) noexcept(std::is_nothrow_constructible_v<Deleter, E&&>) {
        unique_ptr tmp(other.release(), std::forward<E>(other.get_deleter()));
        tmp.swap(*this);
        return *this;
    }



    // Modifiers

    constexpr void reset(pointer new_ptr = pointer()) noexcept {
        ptr = new_ptr;
    }

    void swap(unique_ptr& other) 
    noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Deleter>, std::is_nothrow_move_assignable<Deleter>>) {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }

    constexpr pointer release() noexcept {
        pointer old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }

    constexpr pointer get() const noexcept {
        return ptr;
    }




    // Observers

    constexpr Deleter& get_deleter() noexcept {
        return deleter;
    }

    constexpr const Deleter& get_deleter() const noexcept {
        return deleter;
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    T& operator*() const noexcept{
        return *ptr;
    }

    T* operator->() const noexcept {
        return ptr;
    }
};


// Non-member functions

template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() == y.get();
}

template <typename T, typename D>
constexpr bool operator==(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return x.get() == nullptr;
}

template <typename T, typename D>
constexpr bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return x.get() == nullptr;
}


template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() != y.get();
}

template <typename T, typename D>
constexpr bool operator!=(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return x.get() != nullptr;
}

template <typename T, typename D>
constexpr bool operator!=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return x.get() != nullptr;
}


template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return std::less<>()(x.get(), y.get());
}

template <typename T, typename D>
constexpr bool operator<(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return std::less<>()(nullptr, x.get());
}

template <typename T, typename D>
constexpr bool operator<(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return std::less<>()(x.get(), nullptr);
}


template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return !std::less<>()(y.get(), x.get());
}

template <typename T, typename D>
constexpr bool operator<=(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return !std::less<>()(x.get(), nullptr);
}

template <typename T, typename D>
constexpr bool operator<=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return !std::less<>()(nullptr, x.get());
}


template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return std::less<>()(y.get(), x.get());
}

template <typename T, typename D>
constexpr bool operator>(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return std::less<>()(x.get(), nullptr);
}

template <typename T, typename D>
constexpr bool operator>(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return std::less<>()(nullptr, x.get());
}


template <typename T1, typename D1, typename T2, typename D2>
constexpr bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return !std::less<>()(x.get(), y.get());
}

template <typename T, typename D>
constexpr bool operator>=(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return !std::less<>()(nullptr, x.get());
}

template <typename T, typename D>
constexpr bool operator>=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return !std::less<>()(x.get(), nullptr);
}


// since C++20
// template <typename T1, typename D1, typename T2, typename D2>
// requires std::three_way_comparable_with<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
// std::compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer> 
// operator<=>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
//     return x.get() <=> y.get();
// }


template <typename T, typename D>
constexpr void swap(std::unique_ptr<T, D>& lhs, std::unique_ptr<T, D>& rhs) 
noexcept(std::conjunction_v<std::is_nothrow_move_constructible<D>, std::is_nothrow_move_assignable<D>>) {
    lhs.swap(rhs);
}

template <typename CharT, typename Traits, typename T, typename D>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const std::unique_ptr<T, D>& p) {
    return os << p.get();
}


template <typename T, typename... Args>
constexpr unique_ptr<T> make_unique( Args&&... args) {
    return unique_ptr(new T(std::forward<Args>(args)...));
}