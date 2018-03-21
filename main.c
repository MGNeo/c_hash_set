#include <stdio.h>
#include <stdlib.h>

#include "c_hash_set.h"

// Хэш функция для нультерминированной строки однобайтовых символов.
size_t hash_func(const void *const _data)
{
    size_t hash = 0;
    const char *c = _data;
    while(*c != 0)
    {
        hash += *c;
        ++c;
    }
    return hash;
}

// Функция сравнения нультерминированных строк однобайтовых символов.
size_t comp_func(const void *const _a,
                 const void *const _b)
{
    if ( (_a == NULL) ||
         (_b == NULL) )
    {
        return 0;
    }

    const char *const string_a = _a;
    const char *const string_b = _b;

    if (strcmp(string_a, string_b) == 0)
    {
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    c_hash_set *hash_set = c_hash_set_create(hash_func,
                                             comp_func,
                                             sizeof(char*),
                                             0,
                                             0.5f);
    //c_hash_set_delete(hash_set, NULL);

    c_hash_set_insert(hash_set, "hello");
    c_hash_set_insert(hash_set, "world");
    c_hash_set_insert(hash_set, "hello");
    // Вставляется адрес на строку, поправить.
    // Возможно, нужно подумать о введении функции копирования/переноса данных при добавлении?

    printf("nodes_count: %Iu\n", hash_set->nodes_count);

    getchar();
    return 0;
}
