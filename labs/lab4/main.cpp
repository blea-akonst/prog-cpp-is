#include <iostream>
#include <vector>
#include <algorithm>

namespace lab
{
    template<typename T, typename P>
    bool all_of(const T &begin, const T &end, P predicate)
    {
        T iterator = begin;

        while (++iterator != end)
            if (!predicate(*iterator))
                return false;

        return true;
    }

    template<typename T, typename P>
    bool any_of(const T& begin, const T& end, P predicate)
    {
        T iterator = begin;

        if (predicate(*iterator))
            return true;

        return false;
    }

    template<typename T, typename P>
    bool none_of(const T& begin, const T& end, P predicate)
    {
        T iterator = begin;

        if (predicate(*iterator))
            return false;

        return true;
    }

    template<typename T, typename P>
    bool one_of(const T& begin, const T& end, P predicate)
    {
        unsigned count = 0;
        T iterator = begin;

        while (++iterator != end)
            if (predicate(*iterator))
                ++count;

        if (count == 1)
            return true;
        else
            return false;
    }

    template <typename T, typename P>
    bool is_sorted(const T& begin, const T& end, P comp)
    {
        T iterator = begin + 1;
        while (++iterator != end)
        {
            if (!comp(*(iterator - 1), *iterator))
                return false;
        }

        return true;
    }

    template <typename T, typename P>
    bool is_partitioned(const T& begin, const T& end, P predicate)
    {
        T iterator = begin;

        while (++iterator != end)
            if (!predicate(*iterator))
                break;

        while (++iterator != end)
            if(predicate(*iterator))
                return false;

        return true;
    }

    template <typename T> // returns 0 if all of the elements satisfies the predicate
    size_t find_not (const T& begin, const T& end, T element)
    {
        T iterator = begin;
        while (++iterator != end)
        {
            if (*iterator != element)
                return std::distance(begin, iterator);
        }

        return 0;
    }

    template <typename T>
    size_t find_backward (const T& begin, const T& end, T element)
    {
        while (--end != begin)
            if (*end == element)
                return std::distance(begin, end);
    }

    template <typename T>
    bool is_palindrome(const T& begin, const T& end)
    {
        if (std::distance(begin, end) % 2 == 0)
            return false;

        T start = begin, finish = end - 1;
        bool flag = false;

        while (start != finish)
        {
            if (*start == *finish)
                flag = true;
            else
                return false;
            ++start;
            --finish;
        }

        return flag;
    }
}

int main()
{
    std::vector<int> test {1, 2, 3, 4 ,3, 2, 1};

    // lab::all_of(test.begin(), test.end(), [](int i) { return i >= 0; }) ? std::cout << "true" : std::cout << "false";
    // lab::any_of(test.begin(), test.end(), [](int i) { return i < 0; }) ? std::cout << "true" : std::cout << "false";
    // lab::one_of(test.begin(), test.end(), [](int i) { return i < 0; }) ? std::cout << "true" : std::cout << "false";
    // lab::is_sorted(test.begin(), test.end(), [](int i, int j) { return i < j; }) ? std::cout << "true" : std::cout << "false";
    // lab::is_palindrome(test.begin(), test.end()) ? std::cout << "true" : std::cout << "false";

    return 0;
}
