#pragma once
#include <iostream>
#include <memory>

/* Данный буфер при наличии места в нем зачисляет в себя поступающие элементы, в случае заполненности
 * идет перезапись старых элементов более новыми, аналогичное происходит при удалении элементов из
 * буфера - очищаются более старые данные */

template <class T>
class CircularBuffer
{
private:
    size_t m_capacity;
    size_t m_back_index = 0;
    size_t m_front_index = 0;
    std::unique_ptr<T> m_buff;
public:
    /* constructor with capacity argument */

    CircularBuffer(size_t cap): m_capacity(cap), m_buff(std::unique_ptr<T>(new T[cap])) {}

    /* we don't need a custom destructor because of "smart pointer" application */

    void push_back(T);
    void push_front(T);
    void pop_back();
    void pop_front();
    void new_capacity();
    void print() const;

    T& get_front();
    T& get_back();
    T& operator[] (unsigned);
};

template <typename T>
void CircularBuffer<T>::print() const
{
    if (!m_capacity)
        std::cout << "Your circular buffer is empty!";
    else
    {
        std::cout << "Your circular buffer capacity: " << m_capacity << "." << "\n";
        std::cout << "Your circular buffer contains these elements: " << "\n";
        for (size_t i = 0; i < m_capacity; ++i)
        {
            std::cout << i + 1 << ": " << m_buff[i] << "\n";
        }
    }
}

template <typename T>
void CircularBuffer<T>::push_back(T element)
{
    m_buff[m_back_index] = element;

    if (m_back_index != m_capacity - 1)
        ++m_back_index;
    else
        m_back_index = 0;
}

template <typename T>
void CircularBuffer<T>::push_front(T element)
{
    m_buff[m_front_index] = element;

    if (m_front_index != m_capacity - 1)
        ++m_back_index;
    else
        m_back_index = 0;
}

template <typename T>
T& CircularBuffer<T>::get_back()
{
    if (m_buff[m_back_index] != nullptr)
        return m_buff[m_back_index];
}

template <typename T>
void CircularBuffer<T>::pop_back()
{
    m_buff[m_back_index] = nullptr;
}