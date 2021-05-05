#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

class CPoint
{
public:
    int x, y;
    double vector_module = sqrt(x*x + y*y);
};

bool point_comparator ( CPoint left,  CPoint right)
{
    return left.vector_module < right.vector_module;
}

namespace lab
{
    template<typename T, typename P>
    bool all_of(T begin, T end, P predicate)
    {
        T iterator = begin;

        while (iterator != end)
        {
            if (!predicate(*iterator))
                return false;
            ++iterator;
        }

        return true;
    }

    template<typename T, typename P>
    bool any_of(T begin, T end, P predicate)
    {
        T iterator = begin;

        while (iterator != end)
        {
            if (predicate(*iterator))
                return true;
            ++iterator;
        }

        return false;
    }

    template<typename T, typename P>
    bool none_of(T begin, T end, P predicate)
    {
        T iterator = begin;

        while (iterator != end)
        {
            if (predicate(*iterator))
                return false;
            ++iterator;
        }

        return true;
    }

    template<typename T, typename P>
    bool one_of(T begin, T end, P predicate)
    {
        unsigned count = 0;
        T iterator = begin;

        while (iterator != end)
        {
            if (predicate(*iterator))
                ++count;
            if (count > 1)
                return false;
            ++iterator;
        }

        return count == 1;
    }

    template <typename T, typename P>
    bool is_sorted(T begin, T end, P comp)
    {
        T iterator = begin + 1;
        while (iterator != end)
        {
            if (!comp(*(iterator - 1), *iterator))
                return false;
            ++iterator;
        }

        return true;
    }

    template <typename T>
    bool is_partitioned(T begin, T end)
    {
        T iterator = begin;

        while (iterator != end)
        {
            if (!predicate(*iterator))
                break;
            ++iterator;
        }

        while (iterator != end)
        {
            if(predicate(*iterator))
                return false;
            ++iterator;
        }

        return true;
    }

    template <typename T>
    T find_not (T begin, T end, T element)
    {
        T iterator = begin;
        while (iterator != end)
        {
            if (*iterator != element)
                return iterator;
            ++iterator;
        }

        return end;
    }

    template <typename T>
    T find_backward (T begin, T end, T element)
    {
        if (begin == end)
            return end;

        T iterator = end;
        --iterator;

        while (iterator != begin)
        {
            if (*iterator == element)
                return iterator;
            --iterator;
        }

        if (*begin == element)
            return begin;

        return end;
    }

    template <typename T>
    bool is_palindrome(T begin, T end)
    {
        if (begin == end) return true;

        T start = begin, finish = end;
        --finish;

        while (start != finish)
        {
            if (*start != *finish)
                return false;
            ++start;
            if (start == finish)
                return true;
            --finish;
        }

        return true;
    }
}

int main()
{
    std::vector<int> test {1, 2, 3, 4 ,3, 2, 1};
    std::vector<CPoint> test_class {{3, 3}, {2, 2}, {1, 1}, {4, 4}};

    // lab::all_of(test.begin(), test.end(), [](int i) { return i >= 0; }) ? std::cout << "true" : std::cout << "false";
    // lab::any_of(test.begin(), test.end(), [](int i) { return i < 0; }) ? std::cout << "true" : std::cout << "false";
    // lab::one_of(test.begin(), test.end(), [](int i) { return i < 0; }) ? std::cout << "true" : std::cout << "false";
    lab::is_sorted(test_class.begin(), test_class.end(), point_comparator) ? std::cout << "true" : std::cout << "false";
    // lab::is_palindrome(test.begin(), test.end()) ? std::cout << "true" : std::cout << "false";

    return 0;
}