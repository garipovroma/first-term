//
// Created by roma on 07.07.2020.
//


#ifndef BIGINT_BUFFER_H
#define BIGINT_BUFFER_H

#include <cstddef>
#include <vector>
#include <cstdint>

struct buffer {
public:
    buffer(size_t sz);
    buffer();
    buffer(std::vector<uint32_t> vec);
    ~buffer();
    size_t get_ref_counter() const;
    size_t inc_ref_counter();
    size_t dec_ref_counter();
    std::vector<uint32_t> &get_mas();
private:
    size_t ref_counter;
    std::vector<uint32_t> mas;
};

#endif //BIGINT_BUFFER_H

