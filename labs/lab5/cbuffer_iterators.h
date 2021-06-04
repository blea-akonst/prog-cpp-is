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

    inline T &operator*() const { return *iterator; }                                   // non-const
    inline type operator-(const CIterator &it) const { return iterator - it.iterator; }
    inline CIterator operator+(type it) const { return CIterator(iterator + it); }
    inline CIterator operator-(type it) const { return CIterator(iterator - it); }
    friend inline CIterator operator+(type left, const CIterator &right) { return CIterator(left + right.iterator); }
    friend inline CIterator operator-(type left, const CIterator &right) { return CIterator(left - right.iterator); }

    inline CIterator &operator+=(type it) { iterator += it; return *this; }
    inline CIterator &operator-=(type it) { iterator -= it; return *this; }
    inline CIterator &operator++() { ++iterator; return *this; }
    inline CIterator &operator--() { --iterator; return *this; }

    inline T *operator->() const { return iterator; }
    inline T &operator[](type i) const { return iterator[i]; }
    inline bool operator>(const CIterator &other) const { return iterator > other.iterator; }
    inline bool operator<(const CIterator &other) const { return iterator < other.iterator; }
    inline bool operator>=(const CIterator &other) const { return iterator >= other.iterator; }
    inline bool operator<=(const CIterator &other) const { return iterator <= other.iterator; }
    bool operator==(CIterator const& other) { return iterator == other.iterator; }
    bool operator!=(CIterator const& other) { return iterator != other.iterator; }
};


template <class T>
class CIteratorConst: public std::iterator<std::random_access_iterator_tag, T, const T>
{
private:
    T* iterator;
public:
    explicit CIteratorConst(T* p): iterator(p) {}
    CIteratorConst(const CIteratorConst& other): iterator(other.iterator) {}

    using type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

    inline const T &operator*() const { return *iterator; }                                    // const!
    inline type operator-(const CIteratorConst &it) const { return iterator - it.iterator; }
    inline CIteratorConst operator+(type it) const { return CIteratorConst(iterator + it); }
    inline CIteratorConst operator-(type it) const { return CIteratorConst(iterator - it); }
    friend inline CIteratorConst operator+(type left, const CIteratorConst &right) { return CIteratorConst(left + right.iterator); }
    friend inline CIteratorConst operator-(type left, const CIteratorConst &right) { return CIteratorConst(left - right.iterator); }

    inline CIteratorConst &operator+=(type it) { iterator += it; return *this; }
    inline CIteratorConst &operator-=(type it) { iterator -= it; return *this; }
    inline CIteratorConst &operator++() { ++iterator; return *this; }
    inline CIteratorConst &operator--() { --iterator; return *this; }

    inline T *operator->() const { return iterator; }
    inline T &operator[](type i) const { return iterator[i]; }
    inline bool operator>(const CIteratorConst &other) const { return iterator > other.iterator; }
    inline bool operator<(const CIteratorConst &other) const { return iterator < other.iterator; }
    inline bool operator>=(const CIteratorConst &other) const { return iterator >= other.iterator; }
    inline bool operator<=(const CIteratorConst &other) const { return iterator <= other.iterator; }
    bool operator==(CIteratorConst const& other) { return iterator == other.iterator; }
    bool operator!=(CIteratorConst const& other) { return iterator != other.iterator; }
};