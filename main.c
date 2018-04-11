#include <stdio.h>
#include <stdlib.h>

#include "c_hash_set.h"

// Хэш-функция для нультерминированной строки однобайтовых символов.
size_t hash_func_s(const void *const _data)
{
    size_t hash = 0;
    const char *c = *((char**)_data);
    while(*c != 0)
    {
        hash += *(c++);
    }
    return hash;
}

// Функция сравнения нультерминированных строк однобайтовых символов.
size_t comp_func_s(const void *const _a,
                   const void *const _b)
{
    if ( (_a == NULL) ||
         (_b == NULL) )
    {
        return 0;
    }

    const char *const string_a = *((char**)_a);
    const char *const string_b = *((char**)_b);

    if (strcmp(string_a, string_b) == 0)
    {
        return 1;
    }

    return 0;
}

void print_func_s(const void *const _data)
{
    if (_data == NULL) return;

    const char *const string = *((char**)_data);

    printf("%s\n", string);

    return;
}

int main(int argc, char **argv)
{
    c_hash_set *hash_set = c_hash_set_create(hash_func_s,
                                             comp_func_s,
                                             sizeof(char*),
                                             4,
                                             0.5f);

    const char *const string_1 = "Apple";
    const char *const string_2 = "War";
    const char *const string_3 = "Blue";
    const char *const string_4 = "Computer";

    c_hash_set_insert(hash_set, &string_1);

    c_hash_set_insert(hash_set, &string_2);

    c_hash_set_insert(hash_set, &string_3);

    c_hash_set_insert(hash_set, &string_4);

    c_hash_set_for_each(hash_set, print_func_s);

    c_hash_set_resize(hash_set, 1000);

    c_hash_set_for_each(hash_set, print_func_s);

    c_hash_set_clear(hash_set, NULL);

    c_hash_set_for_each(hash_set, print_func_s);

    c_hash_set_delete(hash_set, NULL);

    getchar();
    return 0;
}

