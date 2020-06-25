#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
#include <cstddef>
#include <cstring>

template<typename T>
struct vector {
    typedef T* iterator;
    typedef const T* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void push_back_realloc(T const&);

    void set_capacity(size_t new_capacity);

    static void destroy_all(T* dst, size_t size);

    static void copy_construct_all(T* dst, T* src, size_t size);

    void clear_all();

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector() : data_(nullptr), size_(0), capacity_(0) {}

template<typename T>
void vector<T>::copy_construct_all(T* dst, T* src, size_t size) {
    size_t i = 0;
    try {
        for (; i < size; i++) {
            new(dst + i) T(src[i]);
        }
    } catch (...) {
        destroy_all(dst, i);
        throw;
    }
}

template<typename T>
void vector<T>::destroy_all(T* dst, size_t size) {
    for (size_t i = size; i != 0; i--) {
        dst[i - 1].~T();
    }
}

template<typename T>
void vector<T>::clear_all() {
    destroy_all(data_, size_);
    operator delete(data_);
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
vector<T>::vector(const vector& other) : vector() {
    if (other.capacity_ != 0) {
        T* new_data = static_cast<T*>(operator new(other.size_ * sizeof(T)));
        try {
            copy_construct_all(new_data, other.data_, other.size_);
        } catch (...) {
            operator delete(new_data);
            throw;
        }
        data_ = new_data;
        capacity_ = other.size_;
        size_ = other.size_;
    }
}

template<typename T>
void vector<T>::swap(vector& other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
vector<T>& vector<T>::operator=(const vector& other) {
    if (this == &other) {
        return *this;
    }
    vector<T> tmp(other);
    swap(tmp);
    return *this;
}

template<typename T>
vector<T>::~vector() {
    clear_all();
}

template<typename T>
T& vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const& vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
T* vector<T>::data() {
    return data_;
}

template<typename T>
T const* vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T& vector<T>::front() {
    return data_[0];
}

template<typename T>
T const& vector<T>::front() const {
    return data_[0];
}

template<typename T>
T& vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const& vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
bool vector<T>::empty() const {
    return (size_ == 0);
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return (data_ + size_);
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return (data_ + size_);
}

template<typename T>
void vector<T>::set_capacity(size_t new_capacity) {
    if (new_capacity != 0) {
        T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
        try {
            copy_construct_all(new_data, data_, size_);
        } catch (...) {
            operator delete(new_data);
            throw;
        }
        destroy_all(data_, size_);
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    } else {
        clear_all();
    }
}

template<typename T>
void vector<T>::push_back(const T& val) {
    if (size_ != capacity_) {
        new(data_ + size_) T(val);
        size_++;
    } else {
        push_back_realloc(val);
    }
}

template<typename T>
void vector<T>::pop_back() {
    data_[size_ - 1].~T();
    size_--;
}

template<typename T>
void vector<T>::reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
        set_capacity(new_capacity);
    }
}

template<typename T>
void vector<T>::clear() {
    destroy_all(data_, size_);
    size_ = 0;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (capacity_ > size_) {
        set_capacity(size_);
    }
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T& val) {
    ptrdiff_t old_size = size_;
    ptrdiff_t int_pos = pos - data_;
    push_back(T(val));
    for (ptrdiff_t i = old_size - 1; i >= int_pos; i--) {
        std::swap(data_[i + 1], data_[i]);
    }
    return data_ + int_pos;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    ptrdiff_t iterations_num = std::min(end() - first, end() - last);
    ptrdiff_t pos_first = first - begin();
    ptrdiff_t pos_last = last - begin();
    for (ptrdiff_t i = 0; i < iterations_num; i++) {
        std::swap(data_[pos_first + i], data_[pos_last + i]);
    }
    for (ptrdiff_t i = 0; i < last - first; i++) {
        pop_back();
    }
    return begin() + pos_first;
}

template<typename T>
void vector<T>::push_back_realloc(const T& val) {
    if (capacity_ == 0) {
        set_capacity(1);
        new(data_) T(val);
        size_ = 1;
    } else {
        T copy(val);
        set_capacity(2 * capacity_);
        push_back(copy);
    }
}

#endif
