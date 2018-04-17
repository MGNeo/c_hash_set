#include <stdio.h>
#include <stdlib.h>

#include "c_hash_set.h"

// Проверка возвращаемых значений не производится для упрощения.

// Функция генерации хэша.
size_t hash_func_s(const void *const _data)
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
size_t comp_func_s(const void *const _a,
                   const void *const _b)
{
    if ( (_a == NULL) || (_b == NULL) )
    {
        return 0;
    }

    const char *const a = (char*)_a;
    const char *const b = (char*)_b;

    if (strcmp(a, b) == 0)
    {
        return 1;
    }

    return 0;
}

// Функция печати данных хэш-множества.
void func_print_s(const void *const _data)
{
    if (_data == NULL) return;
    
    const char *const data = (char*)_data;
    printf("%s\n", data);
    
    return;
}

int main(int argc, char **argv)
{
    // Создание хэш-множества.
    c_hash_set *hash_set = c_hash_set_create(hash_func_s,
                                             comp_func_s,
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
    c_hash_set_for_each(hash_set, func_print_s);
    printf("\n");

    // Удаление из хэш-множества одного элемента.
    c_hash_set_erase(hash_set, string_a, NULL);

    // Вывод содержимого хэш-множества.
    c_hash_set_for_each(hash_set, func_print_s);
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

