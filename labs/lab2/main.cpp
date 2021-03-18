#include <iostream>
#include "polynominal.h"

int main()
{
    Polynomial poly1;

    std::cin >> poly1;

    Polynomial poly2(poly1);

    poly1 /= 0;

    std::cout << poly1;

    return 0;
}