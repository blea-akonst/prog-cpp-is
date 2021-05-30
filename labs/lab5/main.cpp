#include <iostream>
#include "circular_buffer.h"

int main()
{
    CircularBuffer<int> buffer_test(3);

    buffer_test.push_back(5);
    buffer_test.push_back(10);
    buffer_test.push_back(15);
    buffer_test.print();                // buff 5 10 15

    buffer_test.push_back(20);
    buffer_test.push_back(666);
    buffer_test.push_back(30);
    buffer_test.print();                // buff: 20 666 30

    buffer_test.push_front(55);
    buffer_test.push_front(56);
    buffer_test.push_front(57);
    buffer_test.pop_front();
    buffer_test.pop_front();
    buffer_test.print(); // buff: 55
    buffer_test.push_back(555);
    buffer_test.push_back(666);
    buffer_test.print(); // buff: 555 666 55

    std::cout << "Access to the beginning by R-A iterator: " << *(buffer_test.begin()) << "\n"; // 555
    std::cout << "Access by index: " << buffer_test[1] << " <-- second element!\n"; // 666
    std::cout << "Access to the end by R-A iterator: " << *(--buffer_test.end()) << "\n\n"; // 55

    return 0;
}