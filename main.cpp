#include <iostream>
#include <map>
#include "headers/myAllocator.h"
#include "headers/myvector.h"

// Вычисление факториала
int factorial(int n)
{
    if (n == 0)
        return 1;
    int x = 1;
    for (int i = 2; i <= n; ++i)
    {
        x *= i;
    }
    return x;
}

int main()
{
    std::map<int, int> defaultMap;

    // Заполнение ключ- i, значение факториал
    for (int i = 0; i < 10; ++i)
    {
        defaultMap[i] = factorial(i);
    }
    std::cout << "Standart map with default allocator:" << std::endl;

    // Вывод значений
    for (auto &m : defaultMap)
    {
        std::cout << m.first << " " << m.second << "\n";
    }

    std::cout << "\nMap with custom allocator (block size = 10)\n";
    myAllocator<std::pair<const int, int>> mapAlloc(10);
    std::map<int, int, std::less<int>, myAllocator<std::pair<const int, int>>> customMap(std::less<int>(), mapAlloc);

    // Заполнение с кастомным аллокатором
    for (int i = 0; i < 10; ++i)
    {
        customMap[i] = factorial(i);
    }

    // вывод значений
    for (auto &m : customMap)
    {
        std::cout << m.first << " " << m.second << "\n";
    }

    std::cout << "\nMyVector<int> with default allocator: ";
    MyVector<int> vec1;

    // Заполнение с дефолтным аллокатором .
    for (int i = 0; i < 10; ++i)
    {
        vec1.push_back(i);
    }

    // Вывод значений
    for (auto &value : vec1)
    {
        std::cout << value << " ";
    }

    std::cout << "\nMyVector<int> with custom allocator (block size = 10): ";
    MyVector<int, myAllocator<int>> vec2; // Вектор с кастомным аллокатором

    // Заполнение вектора с кастомным
    for (int i = 0; i < 10; ++i)
    {
        vec2.push_back(i);
    }

    // Вывод значений вектора
    for (auto &value : vec2)
    {
        std::cout << value << " ";
    }

    return 0;
}