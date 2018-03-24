#include <stdio.h>
#include <stdlib.h>

#include "c_hash_set.h"

// Хэш-функция для нультерминированной строки однобайтовых символов.
size_t hash_func_s(const void *const _data)
{
    const uint8_t *c = *((uint8_t**)_data);
    const size_t hash = *((size_t*)c);
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

    const uint8_t *const string_a = *((uint8_t**)_a);// Это надо понять... )
    const uint8_t *const string_b = *((uint8_t**)_b);

    if (strcmp(string_a, string_b) == 0)
    {
        return 1;
    }

    return 0;
}

// Функция удаления строки.
void del_func_s(void *const _data)
{
    uint8_t *const data = *((uint8_t**)_data);
    free(data);
}

int main(int argc, uint8_t **argv)
{
    // Количество поисков.
    const size_t find_count = 1E6;

    // Количество вставляемых строк.
    const size_t strings_count = 1E6;

    // Минимальная длина строки (без учета нультерминатора).
    const size_t string_min_length = 32;

    // Максимальная длина строки (без учета нультерминатора).
    const size_t string_max_length = 256;

    // Число слотов хэш-множества.
    const size_t slots_count = strings_count * 2;

    // Фактор загрузки.
    const float max_load_factor = 0.9f;

    // Создание хэш-множества, предназначенного для хранения указателей на нультерминированные однобайтовые строки.
    c_hash_set *hash_set = c_hash_set_create(hash_func_s,
                                             comp_func_s,
                                             sizeof(uint8_t*),
                                             slots_count,
                                             max_load_factor);
    if (hash_set == NULL)
    {
        printf("c_hash_set_create() error! Program end.\n");
        getchar();
        return -1;
    }

    printf("Inserting...\n");
    // Вставка строк.
    srand(time(NULL));
    for (size_t i = 0; i < strings_count; ++i)
    {
        // Создаем строку.
        const size_t string_length = 1 + string_min_length + rand() % (string_max_length - string_min_length + 1);
        uint8_t *const string = (uint8_t*)malloc(string_length);
        if (string == NULL)
        {
            printf("malloc() error! Program end.\n");
            getchar();
            return -2;
        }

        // Заполняем строку случайными символами.
        for (size_t j = 0; j < string_length - 1; ++j)
        {
            string[j] = 1 + rand() % 255;
        }
        string[string_length - 1] = 0;

        // Заносим указатель на строку в хэш-множество.
        c_hash_set_insert(hash_set, &string);
    }

    // Покажем техническую информацию.
    printf("Inserting is completed!\n");
    printf("Nodes count: %Iu\n", hash_set->nodes_count);

    size_t max = 0;
    for (size_t s = 0; s < hash_set->slots_count; ++s)
    {
        if (hash_set->slots[s].count > max)
        {
            max = hash_set->slots[s].count;
        }
    }
    printf("Max length of string of the nodes: %Iu\n\n", max);

    // Строка, которую будем искать.
    uint8_t *find_string = (uint8_t*)malloc(string_max_length + 1);

    // Формиурем случайную строку и пытаемся найти, есть ли она в хэш-множестве.
    // Делаем это много-много раз.
    // Замеряем, сколько времени требуется на поиск.
    printf("Finding, %Iu string...\n", find_count);
    size_t t_1, t_2, t_sum = 0;
    size_t haved = 0,
           not_haved = 0;
    for (size_t i = 0; i < find_count; ++i)
    {
        const size_t string_length = 1 + string_min_length + rand() % (string_max_length - string_min_length + 1);
        for (size_t c = 0; c < string_length - 1; ++c)
        {
            find_string[c] = 1 + rand() % 255;
        }
        find_string[string_length - 1] = 0;
        t_1 = clock();
        if (c_hash_set_find(hash_set, &find_string) > 0)
        {
            ++haved;
        } else {
            ++not_haved;
        }
        t_2 = clock();
        t_sum += t_2 - t_1;
    }
    printf("Finding is completed!\n");

    // Выводим информацию тестирования.
    printf("Total time: %Iu ms.\n", t_sum);
    printf("haved: %Iu\n", haved);
    printf("not_haved: %Iu\n", not_haved);
    // Удаляем хэш-множество.

    c_hash_set_delete(hash_set, del_func_s);

    getchar();
    return 0;
}

