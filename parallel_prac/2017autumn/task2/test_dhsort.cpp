#include <iostream>
#include <cstdlib>
#include <ctime>

#include <omp.h>
#include "dh_sort.hpp"

#if __cplusplus > 201100L
#include <algorithm>
#else
template <class ForwardIterator>
bool is_sorted(ForwardIterator first, ForwardIterator last)
{
    if (first==last) return true;
    ForwardIterator next = first;
    while (++next!=last) {
        if (*next<*first) return false;
        ++first;
    }
    return true;
}
#endif

using namespace std;

int main(int argc, char *argv[])
{
    const int size = 10000000;
    const int threads[] = {1, 2, 4, 8};
    int *a = new int[size];
    srand(time(0));
    for (int i = 0; i < size; ++i){
        a[i] = rand();
    }

    int *b = new int[size];

    for (uint i = 0; i < sizeof(threads) / sizeof(int); ++i){
        omp_set_num_threads(threads[i]);
        memcpy(b, a, size * sizeof(int));
        double t = -omp_get_wtime();
        dh_sort(b, size);
        t += omp_get_wtime();

        const bool is_ok = is_sorted(b, b + size);
        std::cout<<"Sort "<<size<<" elements using "<<threads[i]<<" threads in "<<t<<" sec. Sorted: "<<is_ok<<std::endl;
    }

    delete [] a;
    delete [] b;
    return 0;
}