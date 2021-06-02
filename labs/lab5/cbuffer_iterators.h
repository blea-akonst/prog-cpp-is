#pragma once

template <class T>
class CIterator: public std::iterator<std::random_access_iterator_tag, T>
{
private:
    T* iterator;
public:
    explicit CIterator(T* p): iterator(p) {}
    CIterator(const CIterator& other): iterator(other.iterator) {}

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

    inline T *operator->() const { return iterator; }
    inline T &operator[](type i) const { return iterator[i]; }
    inline bool operator>(const CIterator &other) const { return iterator > other.iterator; }
    inline bool operator<(const CIterator &other) const { return iterator < other.iterator; }
    inline bool operator>=(const CIterator &other) const { return iterator >= other.iterator; }
    inline bool operator<=(const CIterator &other) const { return iterator <= other.iterator; }
    bool operator==(CIterator const& other) { return iterator == other.iterator; }
    bool operator!=(CIterator const& other) { return iterator != other.iterator; }
};