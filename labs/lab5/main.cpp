#include <iostream>
#include "circular_buffer.h"

int main()
{
    CircularBuffer<int> buffer_test(3);

    buffer_test.push_back(5); // buff: 5
    buffer_test.push_back(10); // buff: 5 10
    buffer_test.push_back(15); // buff: 5 10 15
    buffer_test.print();

    buffer_test.push_back(20); // должно вызвать ошибку

    buffer_test.pop_front(); // buff: 10 15
    buffer_test.pop_back(); // buff: 10
    buffer_test.push_back(666); // buff: 10 666
    buffer_test.print();


    buffer_test.push_front(555); // buff: 555 10 666
    buffer_test.print();

    std::cout << "Access to the beginning by R-A iterator: " << *(buffer_test.begin()) << "\n"; // 555
    std::cout << "Access by index: " << buffer_test[1] << " <-- second element!\n";
    std::cout << "Access to the end by R-A iterator: " << *(--buffer_test.end()) << "\n\n"; // 666

    buffer_test.new_capacity(4);
    buffer_test.push_back(111); // 555 10 666 111
    buffer_test.print();

    return 0;
}