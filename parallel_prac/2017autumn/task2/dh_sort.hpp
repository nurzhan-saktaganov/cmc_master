#pragma once

#if __cplusplus < 201100L
#include <algorithm>
#else
#include <utility>
#endif

#include <cstring>
#include <omp.h>

#include "heapsort.hpp"

//слияние упорядоченных фрагментов
template<typename T>
void merge(T *a, T *b, T *c, const int size_a, const int size_b)
{
#pragma omp task firstprivate(a, b, c)
    {
        // first half
        const int size = (size_a + size_b) / 2;

        int ia = 0, ib = 0;

        for (int i = 0; i < size; ++i){
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
#pragma omp task firstprivate(a, b, c)
    {
        // second half
        int ia = size_a - 1, ib = size_b - 1;

        for (int i = size_a + size_b - 1; i >= (size_a + size_b) / 2; --i){
            if (ia < 0) {
                c[i] = b[ib--];
            } else if (ib < 0) {
                c[i] = a[ia--];
            } else if (b[ib] < a[ia]) {
                c[i] = a[ia--];
            } else {
                c[i] = b[ib--];
            }
        }
    }
#pragma omp taskwait
}

template<typename T>
void dh_sort(T *array, const int n, const int border = 1e5)
{
    if (n <= border){
        heap_sort(array, n);
        return;
    }

    // сортируемый массив
    T *a = array;
    // вспомогательный массив
    T *b = new T[n];

#pragma omp parallel firstprivate(array, a, b, n, border)
    {
        // сортируем части
        #pragma omp for
        for(int i = 0; i < n; i += border){
            const int size = std::min(n, i + border) - i;
            heap_sort(array + i, size);
        }

        // размер объединяемых фрагментов
        //#pragma omp master
        for(int i = border; i < n; i *= 2){
            // начало первого из объединяемых фрагментов
            #pragma omp for
            for(int j = 0; j < n; j += 2*i){
                // начало второго из объединяемых фрагментов
                const int r = j + i;

                const int n1 = std::max(std::min(i, n - j), 0);
                const int n2 = std::max(std::min(i, n - r), 0);

                merge(a + j, a + r, b + j, n1, n2);
            }
            std::swap(a, b);
        }

    // копирование, если результат размещен
    // не в основном, а во вспомогательном массиве
    #pragma omp master
        {
            std::swap(a, b);
            if (b != array) {
                std::memcpy(array, b, n * sizeof(T));
            }
        }
    }

    delete [] b;
}
