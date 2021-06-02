#include <iostream>
#include "circular_buffer.h"

int main()
{
    CircularBuffer<int> cb(3);

    cb.push_back(1);    // +1
    cb.push_back(2);    // +2
    cb.push_back(3);    // +3
    cb.print();                 // 1 2 3

    cb.push_back(4);    // rewriting
    cb.push_back(5);
    cb.print();                 // 4 5 3
                              // head^ ^tail

    cb.pop_back();              // 5 removed
    cb.pop_front();             // 3 removed
    cb.print();                 // 4

    std::cout << "Access to the beginning by R-A iterator: " << *(cb.begin()) << "\n"; // 555
    std::cout << "Access by index: " << cb[1] << " <-- second element!\n"; // 666
    std::cout << "Access to the end by R-A iterator: " << *(--cb.end()) << "\n\n"; // 55

    return 0;
}