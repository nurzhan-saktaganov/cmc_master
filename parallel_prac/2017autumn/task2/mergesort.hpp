#pragma once

#if __cplusplus < 201100L
#include <algorithm>
#else
#include <utility>
#endif

//слияние упорядоченных фрагментов
template<typename T>
void merge(T *a, T *b, T *c, const int size_a, const int size_b)
{
    int ia = 0, ib = 0;

    for (int i = 0; i < size_a + size_b; ++i){
        if (ia >= size_a) {
            c[i] = b[ib++];
        } else if (ib >= size_b) {
            c[i] = a[ia++];
        } else if (a[ia] < b[ib]) {
            c[i] = a[ia++];
        } else {
            c[i] = b[ib++];
        }
    }
}

/*
Нерекурсивный алгоритм сортировки слиянием.
Источник: "Параллельные алгоритмы сортировки больших
объемов данных", 2004, М.В. Якобовский.
URL: http://lira.imamod.ru/FondProgramm/Sort/ParallelSort.pdf
*/
template<typename T>
void merge_sort(T *array, const int n)
{
    // сортируемый массив
    T *a = array;
    //вспомогательный массив
    T *b = new T[n];

    //размер объединяемых фрагментов
    for(int i = 1; i < n; i *= 2){
        //начало первого из объединяемых фрагментов
        for(int j = 0; j < n; j += 2*i){
            //начало второго из объединяемых фрагментов
            const int r = j + i;

            const int n1 = std::max(std::min(i, n - j), 0);
            const int n2 = std::max(std::min(i, n - r), 0);
            merge(a + j, a + r, b + j, n1, n2);
        }
        std::swap(a, b);
    }

    std::swap(a, b);

    //копирование, если результат размещен
    //не в основном, а во вспомогательном массиве
    if (b != array) {
        std::memcpy(array, b, n * sizeof(T));
    } else {
        b = a;
    };

    delete [] b;
}
