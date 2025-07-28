#pragma once
#include <iostream>




template <typename T>
struct allocator {

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_copy_assignment = std::true_type;  // Можно копировать (но без разницы)
    using propagate_on_container_move_assignment = std::true_type;  // Можно перемещать (но без разницы)
    using propagate_on_container_swap = std::true_type;             // Можно менять местами (но без разницы)
    using is_always_equal = std::true_type; 

    allocator() = default;

    T* allocate(size_t count) {
        if (count == 0) {
            return nullptr;
        }
        return static_cast<T*>(::operator new(count * sizeof(T)));
    }


    void deallocate(T* ptr, size_t) {
        ::operator delete(ptr);
        return;
    }

    template <typename U, typename ... Args>
    void construct(U* ptr, const Args& ... args) {
        new(ptr) U(args...);
        return;
    }

    template <typename U>
    void destroy(U* ptr) {
        ptr->~U();
        return;
    }

    template <typename U>
    allocator(const allocator<U>&) noexcept {};

    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    bool operator==(const allocator&) const noexcept { 
        return true; 
    }
    bool operator!=(const allocator&) const noexcept { 
        return false; 
    }

};
