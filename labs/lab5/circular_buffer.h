#pragma once
#include <iostream>
#include <memory>

#define ZERO_CAPACITY "You can't add an element to buffer with zero capacity! Expand the capacity!"
#define EMPTY "Buffer is empty!"
#define FULL "Buffer is full!"
#define INDEX_ERROR "You're out of the buffer!";
#define EXPAND_ERROR "Enter the correct capacity! Capacity may be an unsigned integer > 0!"

#define CATCH_ERROR catch(const char* expression) {  std::cerr << "Error: " << expression << "\n"; }

template <class T>
class CircularBuffer
{
private:
    size_t capacity_, count_;
    std::unique_ptr<T[]> data_;
public:
    class CIterator: public std::iterator<std::random_access_iterator_tag, T>
    {
    private:
        T* ptr_;
    public:
        CIterator(T* p): ptr_(p) {}
        CIterator(const CIterator& other): ptr_(other.ptr_) {}

        bool operator==(CIterator const& other) { return ptr_ == other.ptr_; }
        bool operator!=(CIterator const& other) { return ptr_ != other.ptr_; }

        typename CIterator::reference operator*() const
        {
            return *ptr_;
        }

        CIterator& operator++()
        {
            ++ptr_;
            return *this;
        }

        CIterator& operator--()
        {
            --ptr_;
            return *this;
        }
    };

    CircularBuffer(): capacity_(0), count_(0) {} // constructor
    explicit CircularBuffer(size_t cap): capacity_(cap), count_(0) {} // constructor with capacity

    /* we don't need a custom destructor because of "smart pointer" application */

    void push_back(T);
    void push_front(T);
    void pop_back();
    void pop_front();
    void new_capacity(size_t);
    void print() const;
    bool full_buffer() const;

    T& operator[] (size_t);

    typedef CIterator iterator;
    typedef CIterator const const_iterator;

    iterator begin() { return iterator(data_.get()); }
    iterator end() { return iterator(data_.get() + capacity_); }

    const_iterator begin() const { return const_iterator(data_.get()); }
    const_iterator end() const { return const_iterator(data_.get() + count_); }
};


template <typename T>
bool CircularBuffer<T>::full_buffer() const { return capacity_ == count_; }

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

        auto iter = begin();
        int i = 0;

        while (iter != end())
        {
            std::cout << "[" << i + 1 << "] element" << ": " << *iter << "\n";
            ++iter;
            ++i;
        }
        std::cout << "\n";
    }
}

/* Механика вставки в конец: расширение буфера идет до тех пор, пока количество элементов в нем не станет равным
 * капасити, когда это будет так - перестанем вставлять элементы, сославшись на то, что буфер полон */

template <typename T>
void CircularBuffer<T>::push_back(T element)
{
    try
    {
        if (!capacity_)
            throw ZERO_CAPACITY;
        if (full_buffer())
            throw FULL;

        auto temp = std::make_unique<T[]>(count_ + 1);

        for (size_t i = 0; i < count_; ++i)
            temp[i] = data_[i];

        temp[count_] = element;
        data_ = std::move(temp); // or data_.reset() then std::move?
        ++count_;
    }

    CATCH_ERROR;
}

/* Механика удаления из буфера: удаляем последний элемент, капасити динамической памяти уменьшаем до количество_эл - 1 */

template <typename T>
void CircularBuffer<T>::pop_back()
{
    try
    {
        if (!count_)
            throw EMPTY;
        if (count_ > 1)
        {
            auto temp = std::make_unique<T[]>(count_ - 1);

            for (size_t i = 0; i < count_ - 1; ++i)
            {
                temp[i] = data_[i];
            }

            data_ = std::move(temp);
        }
        else
            data_.reset();

        --count_;
    }

    CATCH_ERROR;
}

/* Механика вставки в начало - сдвиг на элемент вправо, вставка на первое место - до полноты буфера */

template <typename T>
void CircularBuffer<T>::push_front(T element)
{
    try
    {
        if (!capacity_)
            throw ZERO_CAPACITY;
        if (full_buffer())
            throw FULL;

        auto temp = std::make_unique<T[]>(count_ + 1);

        for (size_t i = 1; i < count_ + 1; ++i)
            temp[i] = data_[i - 1];

        temp[0] = element;
        data_ = std::move(temp);
        ++count_;
    }

    CATCH_ERROR;
}

/* Удаление - удаляем первый, сдвиг влево до пустоты */

template <typename T>
void CircularBuffer<T>::pop_front()
{
    try
    {
        if (!count_)
            throw EMPTY;
        if (count_ > 1)
        {
            auto temp = std::make_unique<T[]>(count_ - 1);

            for (size_t i = 0; i < count_ - 1; ++i)
            {
                temp[i] = data_[i + 1];
            }

            data_ = std::move(temp);
        }
        else
            data_.reset();

        --count_;
    }

    CATCH_ERROR;
}

template <typename T>
void CircularBuffer<T>::new_capacity(size_t new_cap)
{
    try
    {
        if (new_cap <= 0)
            throw EXPAND_ERROR;

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

        return data_[index];
    }

    CATCH_ERROR;
}