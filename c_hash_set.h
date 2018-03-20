#ifndef C_HASH_SET_H
#define C_HASH_SET_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <stddef.h>

typedef struct s_c_slot
{
    size_t count;
    void *head;
} c_slot;

typedef struct s_c_hash_set
{
    size_t (*hash_func)(const void *const _data);
    size_t (*comp_func)(const void *const _a,
                        const void *const _b);

    size_t size_of_element;
    size_t slots_count;
    size_t nodes_count;

    float max_load_factor;

    c_slot *slots;
} c_hash_set;

c_hash_set *c_hash_set_create(size_t (*_hash_func)(const void *const _data),
                              size_t (*_comp_func)(const void *const _a,
                                                   const void *const _b),
                              const size_t _size_of_elements,
                              const size_t _slots_count,
                              const float _max_load_factor);

#endif
