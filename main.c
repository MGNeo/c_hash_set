#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "c_hash_set.h"

// Функция генерации хэша по элементу-строке.
size_t hash_data_s(const void *const _data)
{
    if (_data == NULL)
    {
        return 0;
    }

    size_t hash = 0;

    const char *c = _data;

    while (*c != 0)
    {
        hash += *(c++);
    }

    return hash;
}

// Функция детального сравнения элементов-строк хэш-множества.
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

// Функция печати элемента-строки.
void print_data_s(const void *const _data)
{
    if (_data == NULL)
    {
        return;
    }

    const char *const data = (char*)_data;

    printf("%s\n", data);
}

int main(int argc, char **argv)
{
    size_t error;
    c_hash_set *hash_set;

    // Пытаемся создать хэш-множество.
    hash_set = c_hash_set_create(hash_data_s, comp_data_s, 10, 0.5f, &error);
    // Если не удалось создать хэш-множество, покажем ошибку.
    if (hash_set == NULL)
    {
        printf("create error: %Iu\n", error);
        printf("Program end.\n");
        getchar();
        return -1;
    }

    // Вставим в хэш-множество несколько строк.
    const char *const string_1 = "War";
    const char *const string_2 = "Underworld";
    const char *const string_3 = "Pain";
    {
        const ptrdiff_t r_code = c_hash_set_insert(hash_set, string_1);
        // Покажем результат операции.
        printf("insert[%s]: %Id\n", string_1, r_code);
    }
    {
        const ptrdiff_t r_code = c_hash_set_insert(hash_set, string_2);
        // Покажем результат операции.
        printf("insert[%s]: %Id\n", string_2, r_code);
    }
    {
        const ptrdiff_t r_code = c_hash_set_insert(hash_set, string_3);
        // Покажем результат операции.
        printf("insert[%s]: %Id\n", string_3, r_code);
    }
    {
        const ptrdiff_t r_code = c_hash_set_insert(hash_set, string_1);
        // Покажем результат операции.
        printf("insert[%s]: %Id\n", string_1, r_code);
    }

    // При помощи обхода хэш-множества выведем содержимое каждого элемента.
    {
        const ptrdiff_t r_code = c_hash_set_for_each(hash_set, print_data_s);
        // Если возникла ошибка, покажем ее.
        if (r_code < 0)
        {
            printf("for each error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -2;
        }
    }

    // Удалим из хэш-множества один элемент.
    {
        const ptrdiff_t r_code = c_hash_set_erase(hash_set, string_1, NULL);
        // Покажем результат операции.
        printf("erase[%s]: %Id\n", string_1, r_code);
    }

    // Попытаемся удалить тот же элемент еще раз.
    {
        const ptrdiff_t r_code = c_hash_set_erase(hash_set, string_1, NULL);
        // Покажем результат операции.
        printf("erase[%s]: %Id\n", string_1, r_code);
    }

    // При помощи обхода хэш-множества выведем содержимое каждого элемента.
    {
        const ptrdiff_t r_code = c_hash_set_for_each(hash_set, print_data_s);
        // Если возникла ошибка, покажем ее.
        if (r_code < 0)
        {
            printf("for each error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -3;
        }
    }

    // Удалим хэш-множество.
    {
        const ptrdiff_t r_code = c_hash_set_delete(hash_set, NULL);
        // Если произошла ошибка, покажем это.
        if (r_code < 0)
        {
            printf("delete error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -4;
        }
    }

    return 0;
}

