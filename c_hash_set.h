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

/*
 *  Слот - это односвязный список.
 *
 *  Структура узла слота:
 *
 *  Что за данные:                   |___next____|____hash____|_________data___________|
 *  Представление:                   |___void*___|___size_t___|___uint8_t[data_size]___|
 *
 *  Указатель на узел указывает сюда ^
 */

typedef struct s_c_hash_set
{
    // Функция, генерирующая хэш на основе данных.
    size_t (*hash_func)(const void *const _data);
    // Функция детального сравнения данных.
    // В случае идентичности данных должна возвращать > 0, иначе 0.
    size_t (*comp_func)(const void *const _a,
                        const void *const _b);

    size_t data_size;
    size_t slots_count;
    size_t nodes_count;

    float max_load_factor;

    void *slots;
} c_hash_set;

c_hash_set *c_hash_set_create(size_t (*const _hash_func)(const void *const _data),
                              size_t (*const _comp_func)(const void *const _a,
                                                         const void *const _b),
                              const size_t _data_size,
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
