/*
    Файл реализации хэш-множества c_hash_set
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

#include "c_hash_set.h"

// Создание пустого хэш-множества с заданным количеством слотов.
// Позволяет создавать хэш-множество с нулевым кол-вом слотов.
// В случае успеха возвращает указатель на созданное хэш-множество, иначе NULL.
c_hash_set *c_hash_set_create(size_t (*const _hash_func)(const void *const _data),
                              size_t (*const _comp_func)(const void *const _a,
                                                   const void *const _b),
                              const size_t _slots_count,
                              const float _max_load_factor)
{
    if (_hash_func == NULL) return NULL;
    if (_comp_func == NULL) return NULL;
    if (_max_load_factor <= 0.0f) return NULL;

    c_hash_set_node **new_slots = NULL;

    if (_slots_count > 0)
    {
        const size_t new_slots_size = _slots_count * sizeof(c_hash_set_node*);
        if ( (new_slots_size == 0) ||
             (new_slots_size / _slots_count != sizeof(c_hash_set_node*)) )
        {
            return NULL;
        }

        new_slots = (c_hash_set_node**)malloc(new_slots_size);
        if (new_slots == NULL) return NULL;

        memset(new_slots, 0, new_slots_size);
    }

    c_hash_set *const new_hash_set = (c_hash_set*)malloc(sizeof(c_hash_set));
    if (new_hash_set == NULL)
    {
        free(new_slots);
        return NULL;
    }

    new_hash_set->hash_func = _hash_func;
    new_hash_set->comp_func = _comp_func;

    new_hash_set->slots_count = _slots_count;
    new_hash_set->nodes_count = 0;

    new_hash_set->max_load_factor = _max_load_factor;

    new_hash_set->slots = new_slots;

    return new_hash_set;
}

// Удаление хэш-множества.
// В случае успеха возвращает > 0, иначе < 0.
ptrdiff_t c_hash_set_delete(c_hash_set *const _hash_set,
                            void (*const _del_func)(void *const _data))
{
    if (c_hash_set_clear(_hash_set, _del_func) < 0)
    {
        return -1;
    }

    free(_hash_set->slots);

    free(_hash_set);

    return 1;
}

// Вставка данных в хэш-множество.
// Не позволяет вставлять NULL.
// В случае успешной вставки возвращает > 0.
// Если такие данные уже есть, они не вставляются, и функция возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_insert(c_hash_set *const _hash_set,
                            const void *const _data)
{
    if (_hash_set == NULL) return -1;
    if (_data == NULL) return -2;

    // Первым делом контролируем процесс увеличения количества слотов.

    // Если слотов нет вообще.
    if (_hash_set->slots_count == 0)
    {
        // Задаем новое количество слотов с некоторым запасом.
        const size_t new_slots_count = EXTENSION_FROM_ZERO;
        if (new_slots_count == 0) return -3;

        // Попытаемся расширить слоты.
        if (c_hash_set_resize(_hash_set, new_slots_count) < 0)
        {
            return -4;
        }
    } else {
        // Если слоты есть, то при достижении предела загруженности увеличиваем их количество.
        const float load_factor = (float)_hash_set->nodes_count / _hash_set->slots_count;
        if (load_factor >= _hash_set->max_load_factor)
        {
            // Определим новое количество слотов.
            size_t new_slots_count = _hash_set->slots_count * 1.75f;
            if (new_slots_count < _hash_set->slots_count)
            {
                return -5;
            }
            new_slots_count += 1;
            if (new_slots_count == 0)
            {
                return -6;
            }

            // Пытаемся расширить слоты.
            if (c_hash_set_resize(_hash_set, new_slots_count) < 0)
            {
                return -7;
            }
        }
    }
    // Проверим, имеются ли в хэш-множестве заданные данные.
    ptrdiff_t r_code = c_hash_set_check(_hash_set, _data);

    // Ошибка.
    if (r_code < 0) return -8;

    // Данные уже имеются.
    if (r_code > 0) return 0;

    // Вставляем данные в хэш-множество.

    // Попытаемся выделить память под узел.
    c_hash_set_node *const new_node = (c_hash_set_node*)malloc(sizeof(c_hash_set_node));
    if (new_node == NULL)
    {
        return -9;
    }

    // Заполним узел и вставим в нужный слот.

    // Неприведенный хэш вставляемых данных.
    const size_t hash = _hash_set->hash_func(_data);
    // Хэш вставляемых данных, приведенный к количеству слотов.
    const size_t presented_hash = hash % _hash_set->slots_count;

    // Записываем в узел неприведенный хэш данных.
    new_node->hash = hash;
    // Связываем узел с данными.
    new_node->data = (void*)_data;
    // Вставляем узел в слот.
    new_node->next = _hash_set->slots[presented_hash];
    _hash_set->slots[presented_hash] = new_node;

    ++_hash_set->nodes_count;

    return 1;
}

// Удаление из хэш-множества заданных данных, если они есть.
// В случае успешного удаления возвращает > 0.
// В случае, если данных нет, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_erase(c_hash_set *const _hash_set,
                           const void *const _data,
                           void (*const _del_func)(void *const _data))
{
    if (_hash_set == NULL) return -1;
    if (_data == NULL) return -2;
    if (_hash_set->nodes_count == 0) return 0;

    // Вычислим неприведенный хэш удаляемых данных.
    const size_t hash = _hash_set->hash_func(_data);

    // Вычислим приведенный хэш удаляемых данных.
    const size_t presented_hash = hash % _hash_set->slots_count;

    // Если требуемый слот пуст, значит элемента в хэш-множестве нет.
    if (_hash_set->slots[presented_hash] == NULL)
    {
        return 0;
    }

    // Просмотр слота на наличие требуемых данных.
    c_hash_set_node *select_node = _hash_set->slots[presented_hash],
                    *prev_node = NULL;

    while (select_node != NULL)
    {
        if (hash == select_node->hash)
        {
            if (_hash_set->comp_func(_data, select_node->data) > 0)
            {
                // Удаляем данный узел.

                // Ампутация узла из слота.
                if (prev_node != NULL)
                {
                    prev_node->next = select_node->next;
                } else {
                    _hash_set->slots[presented_hash] = select_node->next;
                }

                // Вызываем для данных узла функцию удаления, если она задана.
                if (_del_func != NULL)
                {
                    _del_func( select_node->data);
                }
                // Удаляем узел.
                free(select_node);

                --_hash_set->nodes_count;

                return 1;
            }
        }

        prev_node = select_node;
        select_node = select_node->next;
    }

    return 0;
}

// Задает хэш-множеству новое количество слотов.
// Позволяет расширить хэш-множество с нулем слотов.
// Если в хэш-множестве есть хотя бы один элемент, то попытка задать нулевое количество слотов считается ошибкой.
// Если хеш-множество перестраивается, функция возвращает > 0.
// Если хэш-множество не перестраивается, функция возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_resize(c_hash_set *const _hash_set,
                            const size_t _slots_count)
{
    if (_hash_set == NULL) return -1;

    if (_hash_set->slots_count == _slots_count) return 0;

    if (_slots_count == 0)
    {
        if (_hash_set->nodes_count != 0)
        {
            return -2;
        }

        free(_hash_set->slots);
        _hash_set->slots = NULL;

        _hash_set->slots_count = 0;

        return 1;
    } else {
        // Определим новый размер, необходимый под slots.
        const size_t new_slots_size = _slots_count * sizeof(c_hash_set_node*);
        if ( (new_slots_size == 0) ||
             (new_slots_size / _slots_count != sizeof(c_hash_set_node*)) )
        {
            return -3;
        }

        // Попытаемся выделить память под новые слоты.
        c_hash_set_node **const new_slots = (c_hash_set_node**)malloc(new_slots_size);
        if (new_slots == NULL)
        {
            return -4;
        }

        // Обнулим новые слоты.
        memset(new_slots, 0, new_slots_size);

        // Если есть узлы, которые необходимо перенести из старых слотов в новые.
        if (_hash_set->nodes_count > 0)
        {
            size_t count = _hash_set->nodes_count;
            for (size_t s = 0; (s < _hash_set->slots_count)&&(count > 0); ++s)
            {
                if (((void**)_hash_set->slots)[s] != NULL)
                {
                    c_hash_set_node *select_node = _hash_set->slots[s],
                                    *relocate_node;

                    while (select_node != NULL)
                    {
                        relocate_node = select_node;
                        select_node = select_node->next;

                        // Неприведенный хэш переносимого узла.
                        const size_t hash = relocate_node->hash;
                        // Хэш переносимого узла, приведенный к новому количеству слотов.
                        const size_t presented_hash = hash % _slots_count;

                        relocate_node->next = new_slots[presented_hash];
                        new_slots[presented_hash] = relocate_node;

                        --count;
                    }
                }
            }

        }

        free(_hash_set->slots);

        // Используем новые слоты.
        _hash_set->slots = new_slots;
        _hash_set->slots_count = _slots_count;

        return 2;
    }
}

// Функция, проверяющая наличие заданных данных в хэш-множестве.
// Если данные есть в хэш-множестве, возвращает > 0.
// Если данных нет, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_check(const c_hash_set * const _hash_set,
                           const void *const _data)
{
    if (_hash_set == NULL) return -1;
    if (_data == NULL) return -2;

    if (_hash_set->nodes_count == 0) return 0;

    // Неприведенный хэш искомых данных.
    const size_t hash = _hash_set->hash_func(_data);

    // Приведенный хэш искомых данных.
    const size_t presented_hash = hash % _hash_set->slots_count;

    c_hash_set_node *select_node = _hash_set->slots[presented_hash];

    while (select_node != NULL)
    {
        // Неприведенный хэш узла.
        if (hash == select_node->hash)
        {
            // Данные узла.
            if (_hash_set->comp_func(_data, select_node->data) > 0)
            {
                return 1;
            }
        }

        select_node = select_node->next;
    }

    return 0;
}

// Проходит по всем данным хэш-множества и выполняет над данными заданные действия.
// В случае успешного выполнения возвращает > 0.
// В случае, если в хэш-множестве нет данных, возвращает 0.
// В случае ошибки < 0.
ptrdiff_t c_hash_set_for_each(const c_hash_set *const _hash_set,
                              void (*const _func)(const void *const _data))
{
    if (_hash_set == NULL) return -1;
    if (_func == NULL) return -2;

    if (_hash_set->nodes_count == 0) return 0;

    size_t count = _hash_set->nodes_count;
    for (size_t s = 0; (s < _hash_set->slots_count)&&(count > 0); ++s)
    {
        if (_hash_set->slots[s] != NULL)
        {
            const c_hash_set_node *select_node = _hash_set->slots[s];
            while (select_node != NULL)
            {
                _func(select_node->data);
                select_node = select_node->next;
                --count;
            }
        }
    }

    return 1;
}

// Очищает хэш-множество ото всех элементов, сохраняя количество слотов.
// В случае успешного очищения, возвращает > 0.
// Если в хэш-множестве нет элементов, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_clear(c_hash_set *const _hash_set,
                           void (*const _del_func)(void *const _data))
{
    if (_hash_set == NULL) return -1;

    if (_hash_set->nodes_count == 0) return 0;

    size_t count = _hash_set->nodes_count;

    if (_del_func != NULL)
    {
        for (size_t s = 0; (s < _hash_set->slots_count)&&(count > 0); ++s)
        {
            if (_hash_set->slots[s] != NULL)
            {
                c_hash_set_node *select_node = _hash_set->slots[s],
                                *delete_node;
                while (select_node != NULL)
                {
                    delete_node = select_node;
                    select_node = select_node->next;

                    _del_func( delete_node->data );

                    free(delete_node);

                    --count;
                }

                _hash_set->slots[s] = NULL;
            }
        }
    } else {
        // Дублирование кода, для того, чтобы на каждом узле не проверять, задана ли
        // функция удаления данных узла.
        for (size_t s = 0; (s < _hash_set->slots_count)&&(count > 0); ++s)
        {
            if (_hash_set->slots[s] != NULL)
            {
                c_hash_set_node *select_node = _hash_set->slots[s],
                                *delete_node;
                while (select_node != NULL)
                {
                    delete_node = select_node;
                    select_node = select_node->next;

                    free(delete_node);

                    --count;
                }

                _hash_set->slots[s] = NULL;
            }
        }
    }

    _hash_set->nodes_count = 0;

    return 1;
}
