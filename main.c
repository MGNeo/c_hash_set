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
        hash += *c;
        ++c;
    }
    return hash;
}

// Хэш-функция для числа типа double.
size_t hash_func_d(const void *const _data)
{
    size_t hash = 0;

    const uint8_t *b = _data;

    for (size_t i = 0; i < sizeof(double); ++i)
    {
        hash += *b;
        ++b;
    }
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

    const char *const string_a = _a;
    const char *const string_b = _b;

    if (strcmp(string_a, string_b) == 0)
    {
        return 1;
    }

    return 0;
}

// Функция сравнения чисел типа double.
size_t comp_func_d(const void *const _a,
                   const void *const _b)
{
    if ( (_a == NULL) ||
         (_b == NULL) )
    {
        return 0;
    }

    const double a = *((double*)_a);
    const double b = *((double*)_b);

    if (a == b)
    {
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    // Создание хэш-множества, предназначенного для хранения указателей на нультерминированные однобайтовые строки.
    c_hash_set *hash_set = c_hash_set_create(hash_func_s,
                                             comp_func_s,
                                             sizeof(char*),
                                             0,
                                             0.5f);

    const char *const hello_string = "hello";
    const char *const world_string = "world";

    // Вставляем в хэш-множество несколько элементов.
    c_hash_set_insert(hash_set, &hello_string);
    c_hash_set_insert(hash_set, &world_string);
    c_hash_set_insert(hash_set, &hello_string);

    // Отобразим, сколько элементов содержится в хэш-множестве.
    printf("nodes_count: %Iu\n", hash_set->nodes_count);

    // Удаление хэш-множества.
    c_hash_set_delete(hash_set, NULL);

    // Создание хэш-множества, предназначенного для хранения элементов типа double.
    hash_set = c_hash_set_create(hash_func_d,
                                 comp_func_d,
                                 sizeof(double),
                                 0,
                                 0.5f);

    const double a_double = 3.1415;
    const double b_double = 7.1157;
    const double c_double = 99.1;

    // Вставляем в хэш-множество несколько элементов.
    c_hash_set_insert(hash_set, &a_double);
    c_hash_set_insert(hash_set, &b_double);
    c_hash_set_insert(hash_set, &a_double);
    c_hash_set_insert(hash_set, &c_double);

    // Отобразим, сколько элементов содержится в хэш-множестве.
    printf("nodes_count: %Iu\n", hash_set->nodes_count);

    // Удаление хэш-множества.
    c_hash_set_delete(hash_set, NULL);

    getchar();
    return 0;
}
