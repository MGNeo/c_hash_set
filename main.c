#include <stdio.h>
#include <stdlib.h>

#include "c_hash_set.h"

// Проверка возвращаемых значений не производится для упрощения.

// Функция генерации хэша.
size_t hash_data_s(const void *const _data)
{
    if (_data == NULL) return 0;

    size_t hash = 0;
    const char *c = _data;

    while (*c != 0)
    {
        hash += *(c++);
    }

    return hash;
}

// Функция детального сравнения данных хэш-множества.
size_t comp_data_s(const void *const _data_a,
                   const void *const _data_b)
{
    if ( (_data_a == NULL) || (_data_b == NULL) )
    {
        return 0;
    }

    const char *const data_a = (char*)_data_a;
    const char *const data_b = (char*)_data_b;

    if (strcmp(data_a, data_b) == 0)
    {
        return 1;
    }

    return 0;
}

// Функция печати данных хэш-множества.
void print_data_s(const void *const _data)
{
    if (_data == NULL) return;

    const char *const data = (char*)_data;
    printf("%s\n", data);

    return;
}

int main(int argc, char **argv)
{
    // Создание хэш-множества.
    c_hash_set *hash_set = c_hash_set_create(hash_data_s,
                                             comp_data_s,
                                             4,
                                             0.5f);

    // Вставка в хэш-множество нескольких строк.
    const char *const string_a = "Hello";
    const char *const string_b = "War";
    const char *const string_c = "Blue";
    const char *const string_d = "Zoo";

    c_hash_set_insert(hash_set, string_a);
    c_hash_set_insert(hash_set, string_b);
    c_hash_set_insert(hash_set, string_c);
    c_hash_set_insert(hash_set, string_d);

    // Вывод содержимого хэш-множества.
    c_hash_set_for_each(hash_set, print_data_s);
    printf("\n");

    // Удаление из хэш-множества одного элемента.
    c_hash_set_erase(hash_set, string_a, NULL);

    // Вывод содержимого хэш-множества.
    c_hash_set_for_each(hash_set, print_data_s);
    printf("\n");

    // Проверка, имеется ли в хэш-множестве заданные элементы.
    ptrdiff_t r_code;
    r_code = c_hash_set_check(hash_set, string_a);
    printf("string_a[%s]: %Iu\n", string_a, r_code);
    r_code = c_hash_set_check(hash_set, string_b);
    printf("string_b[%s]: %Iu\n", string_b, r_code);
    r_code = c_hash_set_check(hash_set, string_c);
    printf("string_c[%s]: %Iu\n", string_c, r_code);
    r_code = c_hash_set_check(hash_set, string_d);
    printf("string_d[%s]: %Iu\n", string_d, r_code);

    c_hash_set_delete(hash_set, NULL);

    // Обдумать последствия того, что в хэш-множество не может быть вставлен
    // узел, который не может указывать на отсутствующие данные (NULL).
    getchar();
    return 0;
}

