#pragma once
#include <iostream>
#include <memory>
#include <algorithm>

#define ZERO_CAPACITY "You can't add an element to buffer with zero capacity! Expand the capacity!"
#define EMPTY "Buffer is empty!"
#define FULL "Buffer is full!"
#define INDEX_ERROR "You're out of the buffer!";
#define RESIZE_ERROR "Enter the correct capacity! Capacity may be >= current count of elements."

#define CATCH_ERROR catch(const char* expression) {  std::cerr << "Error: " << expression << "\n"; }

template <class T>
class CircularBuffer
{
private:
    size_t capacity_, count_, current_;
    std::allocator<T> alloc_;
    T* data_;
public:
    class CIterator: public std::iterator<std::random_access_iterator_tag, T>
    {
    private:
        T* iterator;
    public:
        CIterator(T* p): iterator(p) {}
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
            iterator++;
            return *this;
        }

        inline CIterator &operator--()
        {
            iterator--;
            return *this;
        }

        inline CIterator operator++(int)
        {
            CIterator temp(*this);
            iterator++;
            return temp;
        }

        inline CIterator operator--(int)
        {
            CIterator temp(*this);
            iterator--;
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
                                         current_(cap / 2),
                                         data_(traits_t::allocate(alloc_, cap * 2)) {}

    CircularBuffer(const CircularBuffer<T>& another)                                // copy constructor
    {
        data_ = traits_t::allocate(alloc_, 2 * another.capacity_);
        capacity_ = another.capacity_;

        std::copy(another.data_begin(), another.data_end(), data_begin());
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
    iterator end() { return iterator(data_ + 2 * capacity_); }
    iterator data_begin = iterator(data_ + capacity_);
    iterator data_end = data_begin;

    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + count_); }
};

template <typename Iter>
size_t get_index(Iter first, Iter second) { return std::distance(first, second) - 1; }

template <typename T>
void CircularBuffer<T>::print() const
{
    if (!count_)
        std::cout << EMPTY << "\n";
    else
    {
        std::cout << "Elements count in your circular buffer: " << count_ << "\n";
        std::cout << "Your circular buffer capacity: " << capacity_ << "." << "\n";
        std::cout << "Your circular buffer contains these elements: " << "\n";

        auto iter = data_begin;
        size_t i = 0;

        while (iter != data_end)
        {
            std::cout << "[" << i + 1 << "] element" << ": " << *iter << "\n";
            ++iter;
            ++i;
        }
        std::cout << "\n";
    }
}

/* Механика вставки в конец: пока не полно - вставляем в конец, после - перезапись
 * Вставка в начало - вставляем, пока есть капасити, иначе - удаление последнего элемента заместо появления первого
 * Удаление с конца - удаляем последний элемент из имеющихся
 * Удаление с начала - удаляем первый элемент из имеющихся*/

template <typename T>
void CircularBuffer<T>::push_back(T element)
{
    try
    {
        if (!capacity_)
            throw ZERO_CAPACITY;

        if (capacity_ != count_)
        {
            data_[current_] = element;
            ++data_end;
            ++count_;
        }
        else
        {
            current_ = get_index(begin(), data_begin);
            throw FULL;
        }
    }
    CATCH_ERROR;
}

template <typename T>
void CircularBuffer<T>::pop_back()
{
    try
    {
        if (count_ > 1)
        {
            --data_end;
        }
        else if (count_)
        {
            traits_t::deallocate(alloc_, data_, 2 * capacity_);
        }
        else if (!count_)
        {
            throw EMPTY;
        }

        --count_;
    }

    CATCH_ERROR;
}

template <typename T>
void CircularBuffer<T>::push_front(T element)
{
    try
    {
        if (!capacity_)
            throw ZERO_CAPACITY;

        if (count_ < capacity_)
        {
            --data_begin;
            data_[get_index(begin(), data_begin)] = element;
        }
        else
        {
            --data_end;
            --data_begin;
            data_[get_index(begin(), data_begin)] = element;
        }
    }

    CATCH_ERROR;
}

template <typename T>
void CircularBuffer<T>::pop_front()
{
    try
    {
        if (count_ > 1)
        {
            ++data_begin;
        }
        else if (count_)
            traits_t::deallocate(alloc_, data_, capacity_ * 2);
        else if (!count_)
        {
            throw EMPTY;
        }

        --count_;
    }

    CATCH_ERROR;
}

template <typename T>
void CircularBuffer<T>::new_capacity(size_t new_cap) // линия :(
{
    try
    {
        if (new_cap < count_)
            throw RESIZE_ERROR;

        T *new_mem = traits_t::allocate(alloc_, new_cap * 2);

        int i = new_cap;

        while (data_begin != data_end)
        {
            new_mem[i] = *data_begin;
            ++data_begin;
        }

        traits_t::deallocate(alloc_, data_, capacity_ * 2);

        data_ = new_mem;
        data_begin = iterator(data_ + new_cap);
        data_end = iterator(data_ + new_cap + count_);

        capacity_ = new_cap;
    }

    CATCH_ERROR;
}

template <typename T>
T& CircularBuffer<T>::operator[] (size_t index)
{
    try
    {
        if (index < 0 || index > count_ - 1)
            throw INDEX_ERROR;

        return data_[get_index(begin(), data_begin) + index];
    }

    CATCH_ERROR;
}