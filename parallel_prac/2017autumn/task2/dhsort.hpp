#pragma once

#if __cplusplus < 201100L
#include <algorithm>
#else
#include <utility>
#endif

#include <cstring>
#include "mergesort.hpp"
#include "heapsort.hpp"

template<typename T>
void dh_sort(T *array, const int n, const int border = 1e5)
{
    if (n <= border){
        heap_sort(array, n);
        return;
    }

    // сортируем части
    for(int i = 0; i < n; i += border){
        const int size = std::min(n, i + border) - i;
        heap_sort(array + i, size);
    }

    // сортируемый массив
    T *a = array;
    // вспомогательный массив
    T *b = new T[n];

    // размер объединяемых фрагментов
    for(int i = border; i < n; i *= 2){
        // начало первого из объединяемых фрагментов
        for(int j = 0; j < n; j += 2*i){
            // начало второго из объединяемых фрагментов
            const int r = j + i;

            const int n1 = std::max(std::min(i, n - j), 0);
            const int n2 = std::max(std::min(i, n - r), 0);

            merge(a + j, a + r, b + j, n1, n2);
        }
        std::swap(a, b);
    }

    std::swap(a, b);

    // копирование, если результат размещен
    // не в основном, а во вспомогательном массиве
    if (b != array) {
        std::memcpy(array, b, n * sizeof(T));
    } else {
        b = a;
    };

    delete [] b;
}
