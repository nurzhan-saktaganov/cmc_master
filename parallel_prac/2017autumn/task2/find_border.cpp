#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <cstring>

#include "mergesort.hpp"
#include "heapsort.hpp"

using namespace std;

double timedifference_microsec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1e6 + (t1.tv_usec - t0.tv_usec);
}

int main(int argc, char *argv[])
{
    int begin = 10000;
    int end = 1000000;
    int step = 10000;

    struct timeval t0;
    struct timeval t1;
    double duration1;
    double duration2;

    srand(time(0));

    int *pattern = new int [end];

    for (int i = 0; i < end; ++i) pattern[i] = rand();


    int *a = new int [end];
    int *b = new int [end];

    cout<<"#size"<<"\t"<<"mergesort time"<<"\t"<<"heapsort time"<<"\n";
    for (int size = begin; size < end; size += step)
    {
        memcpy(a, pattern, size * sizeof(int));
        memcpy(b, pattern, size * sizeof(int));

        gettimeofday(&t0, 0);
        merge_sort(a, size);
        gettimeofday(&t1, 0);
        duration1 = timedifference_microsec(t0, t1);


        gettimeofday(&t0, 0);
        heap_sort(b, size);
        gettimeofday(&t1, 0);
        duration2 = timedifference_microsec(t0, t1);

        cout<<size<<"\t"<<duration1<<"\t"<<duration2<<"\n";
    }

    delete [] a;
    delete [] b;
    delete [] pattern;

    return 0;
}

