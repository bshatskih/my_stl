#include <iostream>
#include <vector>
#include "my_vector.h"




struct S
{
    S() { std::cout << "Construct S\n"; };
    ~S() { std::cout << "Destruct S\n"; };

};



int main() {
    std::vector<int> a(5);
    a.push_back(4);

    // std::cout << "a_size  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // std::cout << "a_capasity  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + 2 * sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // Мы убедились, что вектор хранит под собой три указателя - на начало выделенной памяти, 
    // на конец используемой памяти и на конец выделенной памяти


    std::vector<S> b(5);

    b.resize(3);

    std::cout << '\n';
    return 0;
}