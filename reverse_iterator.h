#pragma onсe
#include <iostream>
#include <type_traits>




template <typename Iter>
requires(std::derived_from<
    typename std::iterator_traits<Iter>::iterator_category,
    std::bidirectional_iterator_tag
>)
class reverse_iterator {
    public:

    using value_type = std::iterator_traits<Iter>::value_type;
    using pointer = std::iterator_traits<Iter>::pointer;
    using reference = std::iterator_traits<Iter>::reference;
    using difference_type = std::iterator_traits<Iter>::difference_type;
    using iterator_category = std::iterator_traits<Iter>::iterator_category;

    private:
    Iter current;

    public:

    constexpr Iter base() const { 
        return current; 
    }

    constexpr reverse_iterator() noexcept = default;
    explicit constexpr reverse_iterator(Iter other) = default;
    explicit reverse_iterator(const reverse_iterator& other) = default;

    template <typename U>
    requires(std::convertible_to<const U&, Iter>)
    constexpr reverse_iterator(const reverse_iterator<U>& other): current(other.base()) {}

    reverse_iterator& operator=(const reverse_iterator&) = default;

    template <typename U>
    requires(std::convertible_to<const U&, Iter>)
    reverse_iterator& operator=(const reverse_iterator<U>& other) {
        current = other.base();
        return *this;
    }

    //
    constexpr reference operator*() const noexcept(noexcept(std::prev(current))){
        Iter tmp = current;  
        --tmp;                  
        return *tmp; 
    }

    constexpr pointer operator->() const noexcept(noexcept(std::prev(current))) 
    requires (std::is_pointer_v<Iter> || requires { current.operator->(); }) {
        Iter tmp = current;              
        --tmp;
        if constexpr (std::is_pointer_v<Iter>) {
            return tmp;                  
        } else {
            return tmp.operator->();     
        }
    }


    constexpr reference operator[](difference_type n) const noexcept(noexcept(*(current - (n + 1)))) {
        return *(current + (n - 1));
    }

    constexpr reverse_iterator& operator++() noexcept(noexcept(--current)) {
        --current;
        return *this;
    }

    constexpr reverse_iterator operator++(int) noexcept(noexcept(--current) && noexcept(reverse_iterator(current))) {
        reverse_iterator tmp = *this;
        --current;
        return tmp;
    }

    constexpr reverse_iterator& operator--() noexcept(noexcept(++current)) {
        ++current;
        return *this;
    }

    constexpr reverse_iterator operator--(int) noexcept(noexcept(++current) && noexcept(reverse_iterator(current))) {
        reverse_iterator tmp = *this;
        ++current;
        return tmp;
    }

    constexpr reverse_iterator& operator+=(difference_type n) noexcept(noexcept(current -= n) && noexcept(current - n)) {
        static_assert(std::derived_from<
            typename std::iterator_traits<Iter>::iterator_category,
            std::random_access_iterator_tag>);
        current -= n;
        return *this;
    }

    constexpr reverse_iterator operator+(difference_type n) noexcept(noexcept(current - n) && noexcept(reverse_iterator(current - n))) {
        static_assert(std::derived_from<
            typename std::iterator_traits<Iter>::iterator_category,
            std::random_access_iterator_tag>);
        return reverse_iterator(current - n);
    }

    constexpr reverse_iterator& operator-=(difference_type n) noexcept(noexcept(current += n) && noexcept(current + n)) {
        static_assert(std::derived_from<
            typename std::iterator_traits<Iter>::iterator_category,
            std::random_access_iterator_tag>);
        current += n;
        return *this;
    }

    constexpr reverse_iterator operator-(difference_type n) noexcept(noexcept(current + n) && noexcept(reverse_iterator(current + n))) {
        static_assert(std::derived_from<
            typename std::iterator_traits<Iter>::iterator_category,
            std::random_access_iterator_tag>);
        return reverse_iterator(current + n);
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator==(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current == other.base();
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator!=(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current != other.base();
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator>=(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current >= other.base();
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator<=(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current <= other.base();
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator>(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current > other.base();
    }

    template <typename OtherIter>
    requires(std::is_same_v<value_type, typename std::iterator_traits<OtherIter>::value_type>)
    bool operator<(const reverse_iterator<OtherIter>& other) const noexcept(noexcept(current == other.base())){
        return current < other.base();
    }
};