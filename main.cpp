#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include "vector.h"




struct S
{
    S() { std::cout << "Construct S\n"; };
    ~S() { std::cout << "Destruct S\n"; };

};



int main() {
    vector<int> test_vec;
    for (int i = 0; i < 10; ++i) {
        // test_vec.push_back(rand() % 100);
        test_vec.push_back(i);
    }

    int x = 10;
    test_vec.insert(test_vec.end(), 10);
    test_vec.insert(test_vec.cbegin(), -7);
    test_vec.insert(test_vec.cbegin() + 5, 1010);


    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));

    test_vec.insert(test_vec.cbegin() + 3, 5, -100);

    std::cout << '\n';

    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << '\n';

    std::vector<int> aa{1, 2, 3};

    test_vec.insert(test_vec.begin() + 1, aa.begin(), aa.end());

    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << '\n';

    // test_vec.erase(test_vec.begin(), test_vec.begin() + 5);

    test_vec.erase(test_vec.cbegin() + 4, test_vec.cend() - 4);

    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << '\n';



/*
    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));

    std::sort(test_vec.begin(), test_vec.end());
    std::cout << '\n';

    std::ranges::copy_n(test_vec.cbegin(), test_vec.size(), std::ostream_iterator<int>(std::cout, " "));


    vector<int>::iterator it = test_vec.begin();
    it = test_vec.end();
    vector<int>::const_iterator cit = test_vec.begin() + 1;
*/




    return 0;
}