#include "c_hash_set.h"

// Вставка в слот узла с данными (абсолютный хэш + данные).
// В случае успешного добавления в слот возвращает > 0.
// В случае наличия такого же элемента в слоте возвращает 0.
// В случае ошибки возвращает < 0.
static ptrdiff_t c_slot_push_unique(c_slot *const _slot,
                        const void *const _hash_and_data,
                        const size_t _size,
                        size_t (*_comp)(const void *const _a,
                                      const void *const _b))
{
    if (_slot == NULL) return -1;
    if (_hash_and_data == NULL) return -2;
    if (_size <= sizeof(size_t)) return -3;

    // Переберем все узлы и определим, есть ли среди них узлы с точно такими же данными.
    if (_slot->count > 0)
    {
        void *select_node = _slot->head;
        const size_t hash_d = *((size_t*)_hash_and_data);

        while (select_node != NULL)
        {
            // Сперва сравним хэши.
            const size_t hash_n = *((size_t*)((void**)select_node + 1));
            if (hash_d == hash_n)
            {
                // А затем сравним данные.
                const void *const data_d = (size_t*)_hash_and_data + 1;
                const void *const data_n = (size_t*)((void**)select_node + 1) + 1;
                if (_comp(data_d, data_n) > 0)
                {
                    // Если нашли узел, данные которого идентичны тем, что мы пытаемся вставить.
                    return 0;
                }
            }
        }
    }

    // Создадим узел, скопируем в него данные и вставим узел в начало слота.
    const size_t node_size = _size + sizeof(size_t);
    if (node_size < _size) return NULL;

    void *const new_node = malloc(node_size);
    if (new_node == NULL) return NULL;

    memcpy((void**)new_node + 1, _hash_and_data, _size);

    *((void**)new_node) = _slot->head;
    _slot->head = new_node;

    return 1;
}

// Создание пустого хэш-множества с заданным количеством слотов.
// Позволяет создавать хэш-таблицу с нулевым кол-вом слотов.
// В случае успеха возвращает указатель на созданное хэш-множество, иначе NULL.
c_hash_set *c_hash_set_create(size_t (*_hash_func)(const void *const _data),
                              size_t (*_comp_func)(const void *const _a,
                                                   const void *const _b),
                              const size_t _size_of_element,
                              const size_t _slots_count,
                              const float _max_load_factor)
{
    if (_hash_func == NULL) return NULL;
    if (_comp_func == NULL) return NULL;
    if (_size_of_element == 0) return NULL;
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

        new_slots = (c_slot*)malloc(sizeof(c_slot));
        if (new_slots == NULL) return NULL;

        for (size_t i = 0; i < )
    }
}
