#include "my_vector.h"


template <typename T>
vector<T>::vector() : sz_(0), cap_(0), arr_(nullptr) {}


template <typename T>
size_t vector<T>::size() const {
    return sz_;
}


template <typename T>
size_t vector<T>::capasity() const {
    return cap_;
}


template <typename T>
void vector<T>::reserve(size_t new_cap) {
    if (new_cap <= cap_) {
        return;
    }
    /*
    Выделяем помять под новый массив, возникает желание написать так - T* new_arr = new T[new_cap], однако это непреемлемый
    вариант по нескольким причинам:
    1) Противоречит самой цели метода - reserve должен выделить достаточное кол-во памяти, чтобы хранить в векторе
       new_cap объектов типа T, то не создавать эти объекты
    2) У типа T может не быть конструктора по умолчанию
    Поэтому мы будем выделять достаточное кол-во памяти для хнанения new_cap штук объектов типа T, при этом не создавая сами объекты
    */
    T* new_arr = reinterpret_cast<T*>(new std::byte[new_cap * sizeof(T)]);

    /*
    Поскольку методы контейнеров должны давать строгую гарантию безопасности отностительно исключений, нужно предусмотреть
    случай, когда конструктор копирования типа T бросит исключение, и обработать данную ситуацию
    */
    size_t i = 0;
    try {
        /*
        Копируем элементы в новый массив, и вновь возникает желание написать что-то привычное в духе new_arr[i] = arr_[i],
        однако этот вариант неправильный, поскольку мы вызываем оператор присваивания от двух объектов типа T, но первый из них в 
        реальности не сконструирован, т.е. под new_arr[i] не лежит никакого объекта
        Воспользуемся placement new, чтобы по данному адресу создать объект от заданнх параметров
        */
        for (; i < sz_; ++i) {
            new(new_arr + i) T(arr_[i]);
        }
    } catch (...) {
        for (size_t j = 0; j < i; ++j) {
            (new_arr + i)->~T();
        }
        delete[] reinterpret_cast<std::byte*>(new_arr);
        throw;
    }

    /*
    Тут кроется ключевой момент сильной гарантии безопасности - если мы не смогли переложить все елементы из arr_ в new_arr
    размера new_cap, то исходный arr_ останется в неизменном состоянии
    */

    for (size_t i = 0; i < sz_; ++i) {
        (arr_ + 1)->~T();
    }
    delete[] reinterpret_cast<std::byte*>(arr_);
    arr_ = new_arr;
    cap_ = new_cap;
    return;
}


template <typename T>
void vector<T>::resize(size_t n, const T& value) {
    if (n > cap_) {
        reserve(n);
    }
    size_t i = sz_;
    try {
        for (; i < n; ++i) {
            new (arr_ + i) T(value);
        }
    } catch (...) {
        for (size_t j = sz_; j < i; ++j) {
            (arr_ + i)->~T();
        }
    }
    if (n < sz_) {
        sz_  = n;
    }
    return;
}


template <typename T>
void vector<T>::push_back(const T& value) {
    if (sz_ == cap_) {
        size_t new_cap = cap_ != 0 ? cap_ * 2 : 1;
        T* new_arr = reinterpret_cast<T*>(new std::byte[new_cap * sizeof(T)]);

        size_t i = 0;
        try {
            for (; i < sz_; ++i) {
                new(new_arr + i) T(arr_[i]);
            }
            new(new_arr + sz_) T(value);
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                (new_arr + i)->~T();
            }
            delete[] reinterpret_cast<std::byte*>(new_arr);
            throw;
        }

        for (size_t i = 0; i < sz_; ++i) {
            (arr_ + 1)->~T();
        }
        delete[] reinterpret_cast<std::byte*>(arr_);
        arr_ = new_arr;
        cap_ = new_cap;
    } else {
        /*
        Если при создании объекта T(value), будет брошено исключение, то sz_ не увеличится и наш вектор останется 
        в исходном состоянии
        ! Ответственность за ресурсы, которые могли быть выделены при создании T(value) лежит на
        конструкторе этого объекта  
        */
        new(arr_ + sz_) T(value);
    }
    ++sz_;
    return;
}


template <typename T>
void vector<T>::pop_back() {
    --sz_;
    (arr_ + sz_)->~T();
    return;
}


