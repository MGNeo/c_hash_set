﻿/*
    Заголовочный файл хэш-множества c_hash_set
    Разработка, отладка и сборка производилась в:
    ОС: Windows 10/x64
    IDE: Code::Blocks 17.12
    Компилятор: default Code::Blocks 17.12 MinGW

    Разработчик: Глухманюк Максим
    Эл. почта: mgneo@yandex.ru
    Место: Российская Федерация, Самарская область, Сызрань
    Дата: 11.04.2018
    Лицензия: GPLv3
*/

#ifndef C_HASH_SET_H
#define C_HASH_SET_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <stddef.h>

// Количество слотов, задаваемое хэш-множеству с нулем слотов при автоматическом расширении.
#define EXTENSION_FROM_ZERO ( (size_t) 1024 )

typedef struct s_c_hash_set_node
{
    struct s_c_hash_set_node *next;
    size_t hash;
    void *data;
} c_hash_set_node;

typedef struct s_c_hash_set
{
    // Функция, генерирующая хэш на основе данных.
    size_t (*hash_func)(const void *const _data);
    // Функция детального сравнения данных.
    // В случае идентичности данных должна возвращать > 0, иначе 0.
    size_t (*comp_func)(const void *const _a,
                        const void *const _b);

    size_t slots_count;
    size_t nodes_count;

    float max_load_factor;

    c_hash_set_node **slots;
} c_hash_set;

c_hash_set *c_hash_set_create(size_t (*const _hash_func)(const void *const _data),
                              size_t (*const _comp_func)(const void *const _a,
                                                         const void *const _b),
                              const size_t _slots_count,
                              const float _max_load_factor);

ptrdiff_t c_hash_set_delete(c_hash_set *const _hash_set,
                            void (*const _del_func)(void *const _data));

ptrdiff_t c_hash_set_insert(c_hash_set *const _hash_set,
                            const void *const _data);

ptrdiff_t c_hash_set_erase(c_hash_set *const _hash_set,
                           const void *const _data,
                           void (*const _del_func)(void *const _data));

ptrdiff_t c_hash_set_resize(c_hash_set *const _hash_set,
                            const size_t _slots_count);

ptrdiff_t c_hash_set_check(const c_hash_set *const _hash_set,
                           const void *const _data);

ptrdiff_t c_hash_set_for_each(const c_hash_set *const _hash_set,
                              void (*const _func)(const void *const _data));

ptrdiff_t c_hash_set_clear(c_hash_set *const _hash_set,
                           void (*const _del_func)(void *const _data));

#endif
