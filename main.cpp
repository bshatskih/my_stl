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
    std::vector<int> b(10);
    a.push_back(111);

    std::vector<int>::const_iterator itata = a.begin();
    std::vector<int>::iterator itatq = b.begin();




    // std::cout << "a_size  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // std::cout << "a_capasity  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + 2 * sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // Мы убедились, что вектор хранит под собой три указателя - на начало выделенной памяти, 
    // на конец используемой памяти и на конец выделенной памяти

;
    vector<int> test;

    // test.push_back(1);
    // test.push_back(3);
    // test.push_back(-2);
    // test.push_back(8);
    // test.push_back(2);
    // test.push_back(-12);
    // test.push_back(17);
    // test.push_back(5);

    std::cout << sizeof(test);

    return 0;
}