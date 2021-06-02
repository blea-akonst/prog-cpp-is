#pragma once
#include <iostream>
#include <memory>
#include <algorithm>
#include "cbuffer_iterators.h"

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
    using traits_t = std::allocator_traits<decltype(alloc_)>;

    CircularBuffer(): capacity_(0), count_(0) {}

    explicit CircularBuffer(size_t cap):          capacity_(cap),
                                         count_(0),
                                         data_(traits_t::allocate(alloc_, cap)) {}

    CircularBuffer(const CircularBuffer<T>& another)
    {
        data_ = traits_t::allocate(alloc_, another.capacity_);
        count_ = another.count_;
        capacity_ = another.capacity_;

        std::copy(another.begin, another.end, begin());

        return *this;
    }

    ~CircularBuffer() { traits_t::deallocate(alloc_, data_, capacity_);
                        traits_t::destroy(alloc_, data_);               }

    void push_back(T);
    void push_front(T);
    void pop_back();
    void pop_front();
    void new_capacity(size_t);
    void print();

    T& operator[] (size_t);

    typedef CIterator<T> iterator;
    typedef CIterator<const T> const_iterator;

    iterator begin() { return iterator(data_); }            // это надо для взаимодействия с стл-алгосами (вроде как)
    iterator end() { return iterator(data_ + capacity_); }  // плюс я хочу выводить весь буфер через итераторы простым действием
    iterator head = begin();                                // поэтому я хочу 4 итератора
    iterator tail = begin();                                // современные компьютеры не пострадают

    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + capacity_); }
};

template <typename T>
void CircularBuffer<T>::print()
{
    if (!count_)
        EMPTY;
    else
    {
        std::cout << "Elements count in your circular buffer: " << count_ << "\n";
        std::cout << "Your circular buffer capacity: " << capacity_ << "." << "\n";
        std::cout << "Your circular buffer contains these elements: " << "\n";

        auto from = begin();
        size_t i = 0;

        while (from != end() && i != count_)
        {
            std::cout << "[" << i + 1 << "] element" << ": " << *from << "\n";
            ++i;
            ++from;
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

    if (head == end()) head = begin();
    if (count_ < capacity_) ++count_;
    if (count_ == capacity_) tail = head + 1; // смещаем начало в случае, если мы пошли по кольцу

    traits_t::construct(alloc_, data_ + std::distance(begin(), head), element);

    ++head;
}

template <typename T>
void CircularBuffer<T>::pop_back()
{
    if (count_ > 1)
    {
        traits_t::destroy(alloc_, data_ + std::distance(begin(), head) - 1);
        if (head == begin()) head = end(); // переброс в конец кольца
        --head;
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

    traits_t::construct(alloc_, data_ + std::distance(begin(), tail), element);

    if (tail == begin()) tail = end();
    if (count_ < capacity_) ++count_;
    if (count_ == capacity_) head = tail - 1; // смещение конца относительно начала

    --tail;
}

template <typename T>
void CircularBuffer<T>::pop_front()
{
    if (count_ > 1)
    {
        traits_t::destroy(alloc_, data_ + std::distance(begin(), tail) - 1);
        if (tail == end()) head = begin(); // переброс в конец кольца
        ++tail;
    }
    else if (count_)
        traits_t::deallocate(alloc_, data_, capacity_);
    else if (!count_)
        EMPTY;

    --count_;
}

template <typename T>
void CircularBuffer<T>::new_capacity(size_t new_cap)
{
    if (new_cap < count_)
        RESIZE_ERROR;

    T *new_mem = traits_t::allocate(alloc_, new_cap);

    auto iter_new = iterator(new_mem);

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