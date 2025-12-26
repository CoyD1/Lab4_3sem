#pragma once
#include <vector>
#include <memory>
#include <limits>
#include <iostream>

template <typename T> // Шаблонный класс для аллокатора, который работает с типом T
class myAllocator
{
private:
    size_t capacity;            // максимальная вместимость текущего пула
    size_t index;               // индекс текущего свободного элемента
    size_t blockSize;           // размер пула (сколько элементов в одном блоке)
    T *curBlock;                // указатель на текущий пул памяти
    std::vector<T *> blocks;    // вектор указателей на все блоки памяти
    std::vector<T *> vecOfFree; // вектор для хранения свободных слотов, которые можно переиспользоват

public:
    using value_type = T;                   // Тип объектов, с которыми работает аллокатор
    using pointer = T *;                    // Тип указателя, который возвращает аллокатор
    using const_pointer = const T *;        // Константный указатель на объект
    using reference = T &;                  // Ссылка на объект типа T
    using const_reference = const T &;      // Константная ссылка на объект типа T
    using size_type = std::size_t;          // Тип для размеров
    using difference_type = std::ptrdiff_t; // Тип для разницы между указателями (нужен для итераторов)

    template <class U> // Шаблонная структура для перепривязки (rebind), чтобы использовать аллокатор с другим типом
    struct rebind
    {
        using other = myAllocator<U>; // Указываем, что аллокатор для типа U — это myAllocator<U>
    };

    // Конструктор по умолчанию. Параметр Blocksize указывает размер блока памяти (по умолчанию 1)
    explicit myAllocator(size_t Blocksize = 1)
        : blockSize(Blocksize ? Blocksize : 1), capacity(0), index(0), curBlock(nullptr) {}

    // Конструктор копирования для аллокатора другого типа (например, для работы с контейнерами разных типов)
    template <typename U>
    myAllocator(const myAllocator<U> &other)
        : capacity(0), index(0), blockSize(other.getBlockSize()), curBlock(nullptr) {}

    // Деструктор, который вызывает метод free_all для освобождения всех ресурсов
    ~myAllocator()
    {
        free_all(); // Освобождаем все выделенные блоки памяти при уничтожении аллокатора
    }

    // Метод для создания объекта в уже выделенной памяти
    template <typename U, typename... Args>
    void construct(U *p, Args &&...args)
    {
        ::new ((void *)p) U(std::forward<Args>(args)...); // Создаем объект в уже выделенной памяти
    }

    // Метод для уничтожения объекта (вызывает деструктор, но не удаляет память)
    template <typename U>
    void destroy(U *p)
    {
        p->~U(); // Вызываем деструктор для объекта
    }

    // Метод для выделения памяти
    T *allocate(size_t n)
    {
        if (n == 0) // Если запрашиваем 0 элементов, возвращаем nullptr
            return nullptr;

        if (n == 1) // Если запрашиваем один элемент
        {
            if (!vecOfFree.empty()) // Если есть свободные элементы, забираем их из vecOfFree
            {
                T *ptr = vecOfFree.back(); // Берем последний освобожденный слот
                vecOfFree.pop_back();      // Удаляем его из списка
                return ptr;
            }
            // Если текущий блок пуст или все слоты в нем заняты, выделяем новый блок
            if (curBlock == nullptr || index >= capacity)
            {
                expand(); // Расширяем пул памяти, если необходимо
            }
            T *ptr = curBlock + index; // Указываем на следующий свободный элемент
            ++index;                   // Увеличиваем индекс
            return ptr;                // Возвращаем указатель на свободное место
        }
        else
        {
            // Если запрашиваем больше одного элемента, выделяем блок памяти через оператор new
            return static_cast<T *>(::operator new(n * sizeof(T)));
        }
    }

    // Метод для расширения пула памяти
    void expand()
    {
        T *new_block = static_cast<T *>(::operator new(blockSize * sizeof(T))); // Выделяем новый блок памяти
        blocks.push_back(new_block);                                            // Добавляем новый блок в список блоков
        curBlock = new_block;                                                   // Указываем, что новый блок теперь текущий
        index = 0;                                                              // Индекс сбрасывается, так как мы только что выделили новый блок
        capacity = blockSize;                                                   // Устанавливаем вместимость блока
    }

    // Метод для освобождения всех блоков памяти
    void free_all()
    {
        for (T *block : blocks) // Для каждого блока в векторе блоков
        {
            ::operator delete(block); // Освобождаем память, используя delete
        }
        blocks.clear();     // Очищаем вектор блоков
        vecOfFree.clear();  // Очищаем вектор свободных слотов
        index = 0;          // Сбрасываем индекс
        capacity = 0;       // Сбрасываем вместимость
        curBlock = nullptr; // Убираем указатель на текущий блок
    }

    // Метод для освобождения памяти для одного элемента или блока
    void deallocate(T *p, size_t n) noexcept
    {
        size_t bytes = n * sizeof(T); // Рассчитываем количество байт
        if (n == 1)                   // Если освободили один элемент
        {
            vecOfFree.push_back(p); // Добавляем элемент в список свободных слотов
        }
        else
        {
            ::operator delete(p); // Если освобождаем блок, используем стандартный delete
        }
    }

    size_t getBlockSize() const
    {
        return blockSize;
    }

    // Операторы сравнения для аллокаторов
    bool operator==(const myAllocator &) const noexcept { return true; }
    bool operator!=(const myAllocator &) const noexcept { return false; }
};
