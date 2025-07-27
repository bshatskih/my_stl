#pragma once
#include <iostream>




template <typename Container>
class back_insert_iterator {
    Container* container;

    public:

    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    back_insert_iterator(Container& container_) : container(&container_) {}

    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }


    back_insert_iterator& operator=(typename Container::value_type&& value) {
        container->push_back(std::move(value));
        return *this;
    }


    back_insert_iterator& operator++() {
        return *this;
    }

    back_insert_iterator operator++(int) {
        return *this;
    }

    back_insert_iterator& operator*() {
        return *this;
    }
};

template <typename Container>
back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}