#include "add.h"
#include <iostream>

int main()
{
    const int actual = add(2, 3);
    const int expected = 5;

    if (actual != expected)
    {
        std::cerr << "FAIL: expected " << expected
                  << ", got " << actual << '\n';
        return 1;
    }

    std::cout << "PASS: add(2, 3) == 5\n";
    return 0;
}