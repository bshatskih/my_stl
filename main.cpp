#include <iostream>
#include <vector>
#include "vector.h"




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

    vector<int> test;

    test.push_back(1);
    test.push_back(3);
    test.push_back(-2);
    test.push_back(8);
    test.push_back(2);
    test.push_back(-12);
    test.push_back(17);
    test.push_back(5);



    vector<int>::iterator it = test.begin();
    std::cout << *it;


    int* ptr;
    std::vector<int>::iterator iter_1;
    std::vector<int>::iterator iter_2; 
    ++iter_1;
    iter_1++;
    --iter_1;
    iter_1--;

    if (iter_1 > iter_2) {
        std::cout << "   ";
    }




    return 0;
}