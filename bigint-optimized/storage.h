//
// Created by roma on 06.07.2020.
//

#ifndef BIGINT_STORAGE_H
#define BIGINT_STORAGE_H

#include <algorithm>
#include "buffer.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <iterator>

struct storage {
    storage();

    storage(storage const& other);

    storage& operator=(storage const& other);

    ~storage();

    void push_back(uint32_t val);
    size_t size() const;
    uint32_t& back();
    void resize(size_t nw_size, uint32_t val);
    void resize(size_t nw_size);
    void reverse();
    const uint32_t& operator[](size_t pos) const;
    uint32_t& operator[](size_t pos);

    uint32_t const& back() const;
    void erase(size_t l, size_t r);
    bool operator==(storage const& other) const ;
    std::vector<uint32_t> get_mas_copy() const;
    static const size_t SMALL_SIZE = 8;
private:
    void unshare();
    void delete_current_buffer();
    void check_ref_counter();
private:
    size_t sz;
    bool small;
    union {
        buffer* data;
        uint32_t static_mas[SMALL_SIZE];
    };
};

#endif //BIGINT_STORAGE_H
