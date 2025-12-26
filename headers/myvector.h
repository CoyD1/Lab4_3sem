#pragma once

#include <cstddef>
#include <algorithm>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class MyVector
{
private:
    // обёртка которая даёт единый интерфейс для любого аллокатора даже если он не реализует все методы
    using AllocTraits = std::allocator_traits<Allocator>;

    T *data = nullptr;   // указатель на начало массива
    size_t length = 0;   // сколько элементов реально хранится
    size_t capacity = 0; // резервированная емкость
    Allocator alloc;     // экземпляр аллокатора

    void resize()
    {
        size_t new_capacity = capacity ? capacity * 2 : 1;
        T *new_data = AllocTraits::allocate(alloc, new_capacity);

        // Переносим старые элементы
        for (size_t i = 0; i < length; ++i)
        {
            AllocTraits::construct(alloc, new_data + i, std::move(data[i]));
        }

        // Удаляем старые
        for (size_t i = 0; i < length; ++i)
        {
            AllocTraits::destroy(alloc, data + i);
        }
        if (data != nullptr)
            AllocTraits::deallocate(alloc, data, capacity);

        data = new_data;
        capacity = new_capacity;
    }

public:
    // для совместимости
    using value_type = T;             // тип элементов в этом контейнере
    using allocator_type = Allocator; // какой аллокатор использует контейнер
    using size_type = size_t;         // тип используемый для размеров
    using reference = T &;            // для ссылок
    using const_reference = const T &;
    using pointer = T *; // для указателей
    using const_pointer = const T *;

    // Итераторы
    T *begin() { return data; }
    T *end() { return data + length; }
    const T *begin() const { return data; }
    const T *end() const { return data + length; }

    explicit MyVector(const Allocator &alloc = Allocator())
        : alloc(alloc) {}

    ~MyVector()
    {
        clear();
        if (data)
            AllocTraits::deallocate(alloc, data, capacity);
    }

    void push_back(const T &value)
    {
        if (length == capacity)
        {
            resize();
        }
        AllocTraits::construct(alloc, data + length, value);
        ++length;
    }

    size_t size() const { return length; }
    size_t get_capacity() const { return capacity; }
    bool empty() const { return length == 0; }

    void clear()
    {
        for (size_t i = 0; i < length; ++i)
        {
            AllocTraits::destroy(alloc, data + i);
        }
        length = 0;
    }
};