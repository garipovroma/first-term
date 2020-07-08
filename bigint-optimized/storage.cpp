//
// Created by roma on 07.07.2020.
//
#include "storage.h"
#include "buffer.h"

storage::storage() : sz(0), small(true) {
    std::fill(static_mas, static_mas + SMALL_SIZE, 0u);
}

void storage::unshare() {
    buffer *new_data = new buffer();
    new_data->get_mas() = data->get_mas();
    data->dec_ref_counter();
    data = new_data;
}

storage::~storage() {
    if (small) {
        return;
    }
    data->dec_ref_counter();
    if (data->get_ref_counter() == 0) {
        delete data;
    }
}

std::vector<uint32_t> storage::get_mas_copy() const {
    if (small) {
        std::vector <uint32_t> res(sz);
        std::copy(static_mas, static_mas + sz, res.begin());
        return res;
    } else {
        return data->get_mas();
    }
}

bool storage::operator==(storage const& other) const {
    return (get_mas_copy()) == (other.get_mas_copy());
}

bool storage::check_ref_counter() {
    if (data->get_ref_counter() == 1) {
        return false;
    } else {
        unshare();
        return true;
    }
}

storage& storage::operator=(storage const& other) {
    if (*this == other) {
        return *this;
    }
    if (!small) {
        if (data->get_ref_counter() == 1) {
            delete data;
        } else {
            data->dec_ref_counter();
        }
    }
    if (other.small) {
        if (!small) {
            std::fill(static_mas, static_mas + SMALL_SIZE, 0u);
        }
        std::copy(other.static_mas, other.static_mas + other.sz, static_mas);
    } else {
        data = other.data;
        data->inc_ref_counter();
    }
    small = other.small;
    sz = other.sz;
    return *this;
}

storage::storage(storage const& other) {
    if (other.small) {
        std::fill(static_mas, static_mas + SMALL_SIZE, 0u);
        std::copy(other.static_mas, other.static_mas + other.sz, static_mas);
    } else {
        data = other.data;
        data->inc_ref_counter();
    }
    sz = other.sz;
    small = other.small;
}

void storage::reverse() {
    if (small) {
        std::reverse(static_mas, static_mas + sz);
    } else {
        check_ref_counter();
        std::reverse(data->get_mas().begin(), data->get_mas().end());
    }
}

const uint32_t& storage::operator[](size_t pos) const {
    if (small) {
        return static_mas[pos];
    } else {
        return (data->get_mas())[pos];
    }
}

uint32_t& storage::operator[](size_t pos) {
    if (small) {
        return static_mas[pos];
    } else {
        check_ref_counter();
        return (data->get_mas())[pos];
    }
}

void storage::erase(size_t l, size_t r) {
    if (small) {
        for (size_t i = l, j = r; i < sz && j < sz; i++, j++) {
            static_mas[i] = static_mas[j];
        }
        sz = sz - (r - l);
    } else {
        check_ref_counter();
        data->get_mas().erase(data->get_mas().begin() + l, data->get_mas().begin() + r);
        sz = data->get_mas().size();
    }
}

uint32_t const& storage::back() const {
    if (small) {
        return static_mas[sz - 1];
    } else {
        return data->get_mas().back();
    }
}

void storage::push_back(uint32_t val) {
    if (small && sz + 1 <= SMALL_SIZE) {
        static_mas[sz] = val;
        sz++;
    } else if (small && sz == SMALL_SIZE) {
        std::vector <uint32_t> cur_mas(sz + 1);
        std::copy(static_mas, static_mas + SMALL_SIZE, cur_mas.begin());
        data = new buffer(0);
        data->get_mas() = cur_mas;
        (data->get_mas())[sz] = val;
        sz++;
        small = false;
    } else {
        check_ref_counter();
        data->get_mas().push_back(val);
        sz = data->get_mas().size();
    }
}

size_t storage::size() const {
    if (small) {
        return sz;
    } else {
        return data->get_mas().size();
    }
}

uint32_t storage::back() {
    if (small) {
        return static_mas[sz - 1];
    } else {
        check_ref_counter();
        return data->get_mas().back();
    }
}

void storage::resize(size_t nw_size) {
    resize(nw_size, 0);
}

void storage::resize(size_t nw_size, uint32_t val) {
    if (small) {
        if (nw_size > sz && nw_size > SMALL_SIZE) {
            std::vector<uint32_t> vec = get_mas_copy();
            data = new buffer(nw_size);
            std::copy(vec.begin(), vec.end(), data->get_mas().begin());
            std::fill(data->get_mas().begin() + sz, data->get_mas().begin() + nw_size, val);
            sz = nw_size;
            small = false;
        } else if (nw_size > sz && nw_size <= SMALL_SIZE) {
            std::fill(static_mas + sz, static_mas + nw_size, val);
            sz = nw_size;
        } else if (nw_size <= sz) {
            sz = nw_size;
        }
    } else {
        check_ref_counter();
        sz = nw_size;
        data->get_mas().resize(nw_size, val);
    }
}