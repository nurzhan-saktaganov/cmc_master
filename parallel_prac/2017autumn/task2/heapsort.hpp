#pragma once
/*
Just copy-paste from here,
URL: http://www.geeksforgeeks.org/heap-sort/
*/

// C++ program for implementation of Heap Sort
#if __cplusplus < 201100L
#include <algorithm>
#else
#include <utility>
#endif

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
template<typename T>
void heapify(T *arr, const int n, const int i)
{
    int largest = i;  // Initialize largest as root
    int l = 2*i + 1;  // left = 2*i + 1
    int r = 2*i + 2;  // right = 2*i + 2

    // If left child is larger than root
    if (l < n && arr[largest] < arr[l]) {
        largest = l;
    }

    // If right child is larger than largest so far
    if (r < n && arr[largest] < arr[r]) {
        largest = r;
    }

    // If largest is not root
    if (largest != i)
    {
        std::swap(arr[i], arr[largest]);
        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

// main function to do heap sort
template<typename T>
void heap_sort(T *arr, const int n)
{
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i=n-1; i>=0; i--)
    {
        // Move current root to end
        std::swap(arr[0], arr[i]);

        // call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}
