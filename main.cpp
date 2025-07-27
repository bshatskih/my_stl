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

    if (itata > itatq) {
        std::cout << 1 << "dwjcn";
    }



    // std::cout << "a_size  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // std::cout << "a_capasity  " << *reinterpret_cast<int**>(static_cast<std::byte*>(static_cast<void*>(&a)) + 2 * sizeof(ptrdiff_t)) 
    //                             - *reinterpret_cast<int**>(&a) << '\n';

    // Мы убедились, что вектор хранит под собой три указателя - на начало выделенной памяти, 
    // на конец используемой памяти и на конец выделенной памяти

;
    vector<int> test;

    test.push_back(1);
    test.push_back(3);
    test.push_back(-2);
    test.push_back(8);
    test.push_back(2);
    test.push_back(-12);
    test.push_back(17);
    test.push_back(5);


    

    vector<double> d;

    vector<int>::iterator it = test.begin();
    vector<double>::const_iterator it_1 = d.cbegin();
    
    // if (it == it_1) {
    //     std::cout << 1;
    // }


    int* ptr;
    std::vector<int>::iterator iter_1; 

    std::vector<int>::const_iterator iter_2; 

    // iter_1 = iter_2;
    iter_2 = iter_1;







    vector<int>::iterator iter_3; 

    vector<int>::const_iterator iter_4(iter_3); 

    // iter_3 = iter_4;
    iter_4 = iter_3;

    *(iter_3 + 2);


    // if (iter_1 > iter_2) {
    //     std::cout << "   ";
    // }




    return 0;
}