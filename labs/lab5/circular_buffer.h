#pragma once
#include <iostream>
#include <memory>
#include <algorithm>

#define ZERO_CAPACITY std::cerr << "You can't add an element to buffer with zero capacity! Expand the capacity!"
#define EMPTY std::cerr << "Buffer is empty!"
#define INDEX_ERROR std::cerr << "You're out of the buffer!"
#define RESIZE_ERROR std::cerr << "Enter the correct capacity! Capacity may be >= current count of elements."

template <class T>
class CircularBuffer
{
private:
    size_t capacity_, count_;
    std::allocator<T> alloc_;
    T* data_;
public:
    class CIterator: public std::iterator<std::random_access_iterator_tag, T>
    {
    private:
        T* iterator;
    public:
        explicit CIterator(T* p): iterator(p) {}
        CIterator(const CIterator& other): iterator(other.iterator) {}

        bool operator==(CIterator const& other) { return iterator == other.iterator; }
        bool operator!=(CIterator const& other) { return iterator != other.iterator; }

        using type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        typename CIterator::reference operator*() const { return *iterator; }
        inline type operator-(const CIterator &it) const { return iterator - it.iterator; }
        inline CIterator operator+(type it) const { return CIterator(iterator + it); }
        inline CIterator operator-(type it) const { return CIterator(iterator - it); }
        friend inline CIterator operator+(type left, const CIterator &right) { return CIterator(left + right.iterator); }
        friend inline CIterator operator-(type left, const CIterator &right) { return CIterator(left - right.iterator); }

        inline CIterator &operator+=(type it)
        {
            iterator += it;
            return *this;
        }

        inline CIterator &operator-=(type it)
        {
            iterator -= it;
            return *this;
        }

        inline CIterator &operator++()
        {
            ++iterator;
            return *this;
        }

        inline CIterator &operator--()
        {
            --iterator;
            return *this;
        }

        inline CIterator operator++(int)
        {
            CIterator temp(*this);
            ++iterator;
            return temp;
        }

        inline CIterator operator--(int)
        {
            CIterator temp(*this);
            --iterator;
            return temp;
        }

        inline T *operator->() const { return iterator; }
        inline T &operator[](type i) const { return iterator[i]; }
        inline bool operator>(const CIterator &other) const { return iterator > other.iterator; }
        inline bool operator<(const CIterator &other) const { return iterator < other.iterator; }
        inline bool operator>=(const CIterator &other) const { return iterator >= other.iterator; }
        inline bool operator<=(const CIterator &other) const { return iterator <= other.iterator; }
    };

    using traits_t = std::allocator_traits<decltype(alloc_)>;

    CircularBuffer(): capacity_(0), count_(0) {}                                    // constructor

    CircularBuffer(size_t cap):          capacity_(cap),                            // constructor with capacity
                                         count_(0),
                                         data_(traits_t::allocate(alloc_, cap)) {}

    CircularBuffer(const CircularBuffer<T>& another)                                // copy constructor
    {
        data_ = traits_t::allocate(alloc_, another.capacity_);
        count_ = another.count_;
        capacity_ = another.capacity_;

        std::copy(another.begin, another.end, begin());

        return *this;
    }

    ~CircularBuffer() { traits_t::deallocate(alloc_, data_, capacity_); data_ = nullptr; }     // destructor

    void push_back(T);
    void push_front(T);
    void pop_back();
    void pop_front();
    void new_capacity(size_t);
    void print() const;

    T& operator[] (size_t);

    typedef CIterator iterator;
    typedef CIterator const const_iterator;

    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + capacity_); }
    iterator current = begin();

    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + capacity_); }
};

template <typename T>
void CircularBuffer<T>::print() const
{
    if (!count_)
        EMPTY;
    else
    {
        std::cout << "Elements count in your circular buffer: " << count_ << "\n";
        std::cout << "Your circular buffer capacity: " << capacity_ << "." << "\n";
        std::cout << "Your circular buffer contains these elements: " << "\n";

        auto iter = begin();
        size_t i = 0;

        while (iter != end())
        {
            std::cout << "[" << i + 1 << "] element" << ": " << *iter << "\n";
            ++i;
            ++iter;
        }
        std::cout << "\n";
    }
}

/* Механика вставки в конец: пока не полно - вставляем в конец, после - перезапись
 * Вставка в начало - вставляем, пока есть капасити, иначе - сдвиг последнего элемента в безызвестность
 * Удаление с конца - удаляем последний элемент из имеющихся
 * Удаление с начала - удаляем первый элемент из имеющихся*/

template <typename T>
void CircularBuffer<T>::push_back(T element)
{
    if (!capacity_)
        ZERO_CAPACITY;

    if (current == end()) current = begin();
    if (count_ < capacity_) ++count_;

    *current = element;
    ++current;
}

template <typename T>
void CircularBuffer<T>::pop_back()
{
    if (count_ > 1)
    {
        current = end() - 1;
        *current = reinterpret_cast<T>(0);
        --current;
    }
    else if (count_)
        traits_t::deallocate(alloc_, data_, capacity_);
    else if (!count_)
        EMPTY;

    --count_;
}

template <typename T>
void CircularBuffer<T>::push_front(T element)
{
    if (!capacity_)
        ZERO_CAPACITY;

    auto iter = end() - 1;

    for (size_t i = capacity_ - 1; i > 0; --i)
        data_[i] = data_[i - 1];

    data_[0] = element;

    if (count_ < capacity_)
        ++count_;
}

template <typename T>
void CircularBuffer<T>::pop_front()
{
    if (count_ > 1)
    {
        auto iter = begin();
        while (reinterpret_cast<int>(*iter) == 0) ++iter;
        *iter = 0;
    }
    else if (count_)
        traits_t::deallocate(alloc_, data_, capacity_);
    else if (!count_)
        EMPTY;

    --count_;
}

template <typename T>
void CircularBuffer<T>::new_capacity(size_t new_cap) // линия :(
{
    if (new_cap < count_)
        RESIZE_ERROR;

    T *new_mem = traits_t::allocate(alloc_, new_cap);

    auto iter_new = iterator(new_mem);
    auto iter_old = begin();

    std::copy(begin(), end(), iterator(new_mem));

    traits_t::deallocate(alloc_, data_, capacity_);
    capacity_ = new_cap;
    data_ = std::move(new_mem);
}

template <typename T>
T& CircularBuffer<T>::operator[] (size_t index)
{
    if (index < 0 || index > count_ - 1) INDEX_ERROR;

    return data_[index];
}