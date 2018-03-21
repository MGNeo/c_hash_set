#include "c_hash_set.h"

// Создание пустого хэш-множества с заданным количеством слотов.
// Позволяет создавать хэш-таблицу с нулевым кол-вом слотов.
// В случае успеха возвращает указатель на созданное хэш-множество, иначе NULL.
c_hash_set *c_hash_set_create(size_t (*_hash_func)(const void *const _data),
                              size_t (*_comp_func)(const void *const _a,
                                                   const void *const _b),
                              const size_t _data_size,
                              const size_t _slots_count,
                              const float _max_load_factor)
{
    if (_hash_func == NULL) return NULL;
    if (_comp_func == NULL) return NULL;
    if (_data_size == 0) return NULL;
    if (_max_load_factor < 0.0f) return NULL;

    c_slot *new_slots = NULL;

    if (_slots_count > 0)
    {
        const size_t new_slots_size = _slots_count * sizeof(c_slot);
        if ( (new_slots_size == 0) ||
             (new_slots_size / _slots_count != sizeof(c_slot)) )
        {
            return NULL;
        }

        new_slots = (c_slot*)malloc(new_slots_size);
        if (new_slots == NULL) return NULL;

        for (size_t i = 0; i < _slots_count; ++i)
        {
            new_slots[i].count = 0;
            new_slots[i].head = NULL;
        }
    }

    c_hash_set *const new_hash_set = (c_hash_set*)malloc(sizeof(c_hash_set));
    if (new_hash_set == NULL)
    {
        free(new_slots);
        return NULL;
    }

    new_hash_set->hash_func = _hash_func;
    new_hash_set->comp_func = _comp_func;

    new_hash_set->data_size = _data_size;
    new_hash_set->slots_count = _slots_count;
    new_hash_set->nodes_count = 0;

    new_hash_set->max_load_factor = _max_load_factor;

    new_hash_set->slots = new_slots;

    return new_hash_set;
}

// Удаление хэш-множества.
// В случае успеха возвращает > 0, иначе < 0.
ptrdiff_t c_hash_set_delete(c_hash_set *const _hash_set,
                            void (*_del_func)(void *const _data))
{
    if (_hash_set == NULL) return -1;

    if (_hash_set->nodes_count > 0)
    {
        if (_del_func != NULL)
        {
            for (size_t i = 0; i < _hash_set->nodes_count; ++i)
            {
                if (_hash_set->slots[i].count > 0)
                {
                    void *select_node = _hash_set->slots[i].head,
                         *delete_node;
                    while(select_node != NULL)
                    {
                        delete_node = select_node;
                        select_node = *((void**)select_node);

                        _del_func((void**)delete_node + 1);
                        free(delete_node);
                    }
                }
            }
            free(_hash_set->slots);
        } else {
            for (size_t i = 0; i < _hash_set->nodes_count; ++i)
            {
                if (_hash_set->slots[i].count > 0)
                {
                    void *select_node = _hash_set->slots[i].head,
                         *delete_node;
                    while(select_node != NULL)
                    {
                        delete_node = select_node;
                        select_node = *((void**)select_node);

                        free(delete_node);
                    }
                }
            }
            free(_hash_set->slots);
        }
    }

    free(_hash_set);

    return 1;
}

// Вставка данных в хэш-множество.
// В случае успешной вставки возвращает > 0.
// В случае наличия точно таких же данных в хэш-множестве возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_hash_set_insert(c_hash_set *const _hash_set,
                        const void *const _data)
{
    if (_hash_set == NULL) return -1;
    if (_data == NULL) return -2;

    // Сперва определим, есть ли необходимость перестроить (расширить) таблицу хэш-множества.
    size_t rebuild = 0;

    if (_hash_set->slots_count == 0)
    {
        rebuild = 1;
    } else {
        const float load_factor = (float)_hash_set->nodes_count / _hash_set->slots_count;
        if (load_factor >= _hash_set->max_load_factor)
        {
            rebuild = 1;
        }
    }

    // Если нужно, то перестраиваем.
    if (rebuild == 1)
    {
        // Определяем новое количество слотов.
        size_t new_slots_count = _hash_set->slots_count * 1.75f;
        if (new_slots_count < _hash_set->slots_count)
        {
            return -4;
        }
        new_slots_count += 1;
        if (new_slots_count == 0)
        {
            return -5;
        }
        // Определим количество памяти, необходимое под новые слоты.
        const size_t new_slots_size = new_slots_count * sizeof(c_slot);
        if ( (new_slots_size == 0) ||
             (new_slots_size / new_slots_count != sizeof(c_slot)) )
        {
            return -6;
        }

        // Пытаемся выделить память под новые слоты.
        c_slot *const new_slots = (c_slot*)malloc(new_slots_size);
        if (new_slots == NULL)
        {
            return -7;
        }

        // Обнулим новые слоты.
        for (size_t i = 0; i < new_slots_count; ++i)
        {
            new_slots[i].count = 0;
            new_slots[i].head = NULL;
        }

        // Если в старых слотах есть узлы, то переносим их в новые слоты.
        if (_hash_set->nodes_count > 0)
        {
            size_t count = _hash_set->nodes_count;
            for (size_t s = 0; (s < _hash_set->slots_count) && (count > 0); ++s)
            {
                if (_hash_set->slots[s].count > 0)
                {
                    void *select_node = _hash_set->slots[s].head,
                         *relocate_node;
                    while(select_node != NULL)
                    {
                        relocate_node = select_node;
                        select_node = *((void**)select_node);
                        // Неприведенный хэш переносимого узла.
                        const size_t hash = *((size_t*)((void**)relocate_node + 1));
                        // Хэш переносимого узла, приведенный к количеству новых слотов.
                        const size_t presented_hash = hash % new_slots_count;

                        // Переносим узел в новый слот.
                        *((void**)relocate_node) = new_slots[s].head;
                        new_slots[s].head = relocate_node;
                        ++new_slots[s].count;
                    }
                }
            }

            // Удаляем старые слоты.
            free(_hash_set->slots);
        }
        _hash_set->slots = new_slots;
        _hash_set->slots_count = new_slots_count;
    }

    // Неприведенный хэш вставляемых данных.
    const size_t hash = _hash_set->hash_func(_data);

    // Приведенный хэш вставляемых данных.
    const size_t presented_hash = hash % _hash_set->slots_count;

    // Проверим, имеется ли уже такой элемент в слоте.
    void *select_node = _hash_set->slots[presented_hash].head;
    while(select_node != NULL)
    {
        // Реальный хэш данных просматриваемого узла.
        const size_t hash_n = *((size_t*)((void**)select_node + 1));
        if (hash == hash_n)
        {
            // Данные просматриваемого узла.
            const void *const data_n = ((size_t*)((void**)select_node + 1)) + 1;
            // Сравним детально.
            if (_hash_set->comp_func(_data, data_n) > 0)
            {
                return 0;
            }
        }

        select_node = *((void**)select_node);
    }

    // Если такого элемента в слоте нет, вставляем его туда.

    // Определим размер формируемого узла.
    const size_t new_node_size = sizeof(void*) + sizeof(size_t) + _hash_set->data_size;
    if (new_node_size < _hash_set->data_size)
    {
        return -8;
    }

    // Попытаемся сформировать узел.
    void *const new_node = malloc(new_node_size);
    if (new_node == NULL)
    {
        return NULL;
    }

    // Копируем в узел неприведенный хэш.
    *((size_t*)((void**)new_node + 1)) = hash;

    // Копируем вузел данные.
    memcpy(((size_t*)((void**)new_node + 1) + 1), _data, _hash_set->data_size);

    // Встраиваем узел в слот.
    *((void**)new_node) = _hash_set->slots[presented_hash].head;
    _hash_set->slots[presented_hash].head = new_node;
    ++_hash_set->slots[presented_hash].count;

    ++_hash_set->nodes_count;

    return 1;
}
