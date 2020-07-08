//
// Created by roma on 07.07.2020.
//

#include "buffer.h"

buffer::buffer(size_t sz) : ref_counter(1), mas(sz) {}

size_t buffer::get_ref_counter() const {
    return ref_counter;
}

size_t buffer::inc_ref_counter() {
    return ++ref_counter;
}

size_t buffer::dec_ref_counter() {
    return --ref_counter;
}

std::vector<uint32_t> &buffer::get_mas() {
    return mas;
}

buffer::buffer() : ref_counter(1), mas(std::vector<uint32_t>()) {}

buffer::buffer(std::vector<uint32_t> vec) : ref_counter(1), mas(std::move(vec)) {}

buffer::~buffer() = default;
