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

    explicit CircularBuffer(size_t cap): capacity_(cap),
                                         count_(0),
                                         data_(traits_t::allocate(alloc_, cap)) {}

    CircularBuffer(const CircularBuffer<T>& another)
    {
        data_ = traits_t::allocate(alloc_, another.capacity_);
        count_ = another.count_;
        capacity_ = another.capacity_;

        std::copy(another.begin(), another.end(), begin());

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

    size_t get_capacity() { return capacity_; }
    size_t get_count() { return count_; }

    T& operator[] (size_t);

    typedef CIterator<T> iterator;
    typedef CIteratorConst<T> const_iterator;

    iterator begin() { return iterator(data_); }            // для взаимодействия с STL и более удобной реализации методов
    iterator end() { return iterator(data_ + capacity_); }  // не думаю, что для современного ПК 8 или 16 байт будут проблемой
    iterator head = begin();
    iterator tail = begin();

    const_iterator begin() const { return const_iterator(data_); }              // ну ладно, 16 или 32
    const_iterator end() const { return const_iterator(data_ + capacity_); }    // мало же бтв
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

        auto iter = tail;
        size_t i = 0;

        while (iter != head)
        {
            std::cout << "[" << i + 1 << "] element" << ": " << *iter << "\n";
            ++i;
            if (iter != end())
                ++iter;
            else
                iter == begin();
        }
        std::cout << "\n";
    }
}

/*
 * Механика вставки в конец: пока не полно - вставляем в конец, после - перезапись со сдвигом начала под ручку
 * в начало: двигаемся назад, после - перезапись со сдвигом конца под ручку
 * удаление с конца - удаляем, пока не достигли начала
 * удаление с начала - удаляем, пока не достигли конца
                                                                                                            */

template <typename T>
void CircularBuffer<T>::push_back(T element)
{
    if (!capacity_)
        ZERO_CAPACITY;

    if (head == end()) head = begin();
    if (count_ < capacity_) ++count_;
    if (count_ == capacity_)
    {
        tail = head + 1;
        traits_t::destroy(alloc_, data_ + std::distance(begin(), head));
    }

    traits_t::construct(alloc_, data_ + std::distance(begin(), head), element);

    ++head;
}

template <typename T>
void CircularBuffer<T>::pop_back()
{
    if (count_ > 1)
    {
        if (head == begin())
        {
            traits_t::destroy(alloc_, data_);
            head = end();
        }
        else
            traits_t::destroy(alloc_, data_ + std::distance(begin(), head) - 1);
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

    if (count_ == capacity_) traits_t::destroy(alloc_, data_ + std::distance(begin(), tail));

    if (tail == begin() && head == begin() && count_)
        traits_t::construct(alloc_, data_ + std::distance(begin(), end() - 1), element);
    else
        traits_t::construct(alloc_, data_ + std::distance(begin(), tail), element);

    if (tail == begin()) tail = end();
    if (count_ < capacity_) ++count_;
    if (count_ == capacity_) head = tail - 1;

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