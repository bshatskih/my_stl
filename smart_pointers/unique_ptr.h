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

    explicit constexpr unique_ptr(pointer p, const Deleter& d) noexcept : ptr(p), deleter(d) {}

    constexpr unique_ptr(pointer p, Deleter&& d) noexcept(std::is_nothrow_move_constructible_v<Deleter>) : ptr(p), deleter(std::move(d)) {}

    constexpr unique_ptr(std::nullptr_t, const Deleter& d) noexcept(std::is_nothrow_copy_constructible_v<Deleter>) : ptr(nullptr), deleter(d) {}
    
    constexpr unique_ptr(std::nullptr_t, Deleter&& d) noexcept(std::is_nothrow_move_constructible_v<Deleter>) : ptr(nullptr), deleter(std::move(d)) {}

    constexpr unique_ptr(unique_ptr&& other) noexcept(std::is_nothrow_move_constructible_v<Deleter>)
    : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    template <typename U, typename E>
    requires std::convertible_to<U*, T*> && std::constructible_from<Deleter, E&&>
    constexpr unique_ptr(unique_ptr<U, E>&& other) noexcept(std::is_nothrow_constructible_v<Deleter, E&&> && std::is_nothrow_convertible_v<U*, T*>) 
    : ptr(other.release()), deleter(std::forward<E>(other.get_deleter())) {}

    unique_ptr(const unique_ptr&) = delete;



    // Destructor

    ~unique_ptr() {
        if (ptr) {
            deleter(ptr);
        }
    }



    // Assignment

    constexpr unique_ptr& operator=(const unique_ptr&) = delete;    


    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept(std::is_nothrow_move_assignable_v<Deleter>) {
        if (this != std::addressof(other)) {
            reset(other.release());
            deleter = std::move(other.deleter); 
        }
        return *this;
    }

    constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template <typename U, typename E>
    requires std::convertible_to<U*, T*> && std::constructible_from<Deleter, E&&>
    constexpr unique_ptr& operator=(unique_ptr<U, E>&& other) 
    noexcept(std::is_nothrow_convertible_v<U*, T*> && 
            (std::is_assignable_v<Deleter&, E&&> ? std::is_nothrow_assignable_v<Deleter&, E&&> : std::is_nothrow_constructible_v<Deleter, E&&>)) {
        if (this != std::addressof(other)) {
            reset(other.release()); 

            if constexpr (std::is_assignable_v<Deleter&, E&&>) {
                deleter = std::forward<E>(other.get_deleter());
            } else {
                Deleter tmp(std::forward<E>(other.get_deleter()));
                deleter = std::move(tmp);
            }
        }
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



    // Observers

    constexpr pointer get() const noexcept {
        return ptr;
    }

    constexpr Deleter& get_deleter() noexcept {
        return deleter;
    }

    constexpr const Deleter& get_deleter() const noexcept {
        return deleter;
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    constexpr T& operator*() const noexcept{
        return *ptr;
    }

    constexpr T* operator->() const noexcept {
        return ptr;
    }
};


template <typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
    T* ptr;
    [[no_unique_address]] Deleter deleter;

public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;



    // Constructors

    constexpr unique_ptr() noexcept : ptr(nullptr) {}

    constexpr unique_ptr(std::nullptr_t) noexcept : ptr(nullptr) {}

    template <typename U>
    requires std::is_array_v<U> && std::convertible_to<typename unique_ptr<U, Deleter>::pointer, pointer>
    explicit constexpr unique_ptr(U* p) noexcept(std::is_nothrow_default_constructible_v<deleter_type>) : ptr(p) {}

    template <typename U>
    requires std::is_array_v<U> && std::convertible_to<typename unique_ptr<U, Deleter>::pointer, pointer>
    explicit constexpr unique_ptr(U* p, const deleter_type& del) noexcept(std::is_nothrow_copy_constructible_v<deleter_type>) : ptr(p), deleter(del) {}

    template <typename U>
    requires std::is_array_v<U> && std::convertible_to<typename unique_ptr<U, Deleter>::pointer, pointer>
    constexpr unique_ptr(U* p, deleter_type&& d) noexcept(std::is_nothrow_move_constructible_v<deleter_type>) : ptr(p), deleter(std::move(d)) {}

    constexpr unique_ptr(std::nullptr_t, const deleter_type& del) noexcept(std::is_nothrow_copy_constructible_v<deleter_type>) : ptr(nullptr), deleter(del) {}

    constexpr unique_ptr(std::nullptr_t, deleter_type&& d) noexcept(std::is_nothrow_move_constructible_v<deleter_type>) : ptr(nullptr), deleter(std::move(d)) {};

    constexpr unique_ptr(unique_ptr&& other) noexcept(std::is_nothrow_move_constructible_v<deleter_type>): ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    template <typename U, typename E>
    requires std::is_array_v<U> && std::convertible_to<typename unique_ptr<U, E>::pointer, pointer> && std::constructible_from<deleter_type, E&&>
    constexpr unique_ptr(unique_ptr<U, E>&& other) noexcept(std::is_nothrow_constructible_v<deleter_type, E&&>)
    : ptr(other.ptr), deleter(std::move(other.get_deleter())) {
        other.ptr = nullptr;
    }

    unique_ptr(const unique_ptr&) = delete;



    // Destructor

    constexpr ~unique_ptr() {
        deleter(ptr);
    }   



    // Assignment

    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept(std::is_nothrow_move_assignable_v<deleter_type>) {
        if (this != std::addressof(other)) {
            reset(other.release());
            deleter = std::move(other.deleter);
        }
        return *this;
    }

    template <class U, class E>
    requires std::is_array_v<U> && std::convertible_to<typename unique_ptr<U, E>::pointer, pointer> &&
            std::assignable_from<deleter_type&, E&&> && (std::is_same_v<E, deleter_type> || !std::is_reference_v<deleter_type>)
    constexpr unique_ptr& operator=(unique_ptr<U, E>&& other) noexcept(std::is_nothrow_assignable_v<deleter_type&, E&&>) {
        if (this != std::addressof(other)) {
            reset(other.release());                          
            deleter = std::forward<E>(other.get_deleter());
        }
        return *this;
    }

    constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    unique_ptr& operator=(const unique_ptr&) = delete;



    // Modifiers

    constexpr pointer release() noexcept {
        pointer old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }

    template <typename U>
    requires std::is_array_v<U> && std::convertible_to<U*, pointer>
    void reset(U* p = nullptr) noexcept(std::is_nothrow_invocable_v<deleter_type&, pointer>) {
        pointer old_ptr = ptr;
        ptr = p;
        if (old_ptr) {
            get_deleter()(old_ptr);
        }
    }

    void swap(unique_ptr& other) 
    noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Deleter>, std::is_nothrow_move_assignable<Deleter>>) {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }


    
    // Observers

    constexpr pointer get() const noexcept {
        return ptr;
    }

    constexpr Deleter& get_deleter() noexcept {
        return deleter;
    }

    constexpr const Deleter& get_deleter() const noexcept {
        return deleter;
    }

    constexpr explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    constexpr T& operator[](std::size_t i) const {
        return *(ptr + i);
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