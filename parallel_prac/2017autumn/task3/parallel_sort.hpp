#pragma once

#include <mpi.h>
#include "Schedule.hpp"

template<typename T>
void merge_up(T *a, T *b, T *c, const int size)
{
    int i, j, k;
    i = j = k = 0;
    while (k < size) {
        if (a[i] < b[j]) {
            c[k++] = a[i++];
        } else {
            c[k++] = b[j++];
        }
    }
}

template<typename T>
void merge_down(T *a, T *b, T *c, const int size)
{
    int i, j, k;
    i = j = k = size - 1;
    while(k >= 0){
        if(a[i] < b[j]){
            c[k--] = b[j--];
        } else {
            c[k--] = a[i--];
        }
    }
}

template<typename T>
void perform_schedule(MPI::Comm &comm, MPI::Datatype &datatype, Schedule &schedule, const int rank, T *&array, const int part_size)
{
    T *send_buffer = array;
    T *recv_buffer = new T[part_size];
    T *merge_buffer = new T[part_size];

    std::vector<Comparator>::iterator it = schedule.begin();
    for(; it != schedule.end(); ++it){
        const int neighbour = it->get_pair(rank);
        comm.Sendrecv(
            send_buffer, part_size, datatype, neighbour, 0,
            recv_buffer, part_size, datatype, neighbour, 0
        );

        if (rank < neighbour) {
            merge_up(send_buffer, recv_buffer, merge_buffer, part_size);
        } else {
            merge_down(send_buffer, recv_buffer, merge_buffer, part_size);
        }
        std::swap(send_buffer, merge_buffer);
    }

    delete [] recv_buffer;
    delete [] merge_buffer;

    array = send_buffer;
}

template<typename T>
void parallel_sort(MPI::Comm &comm, MPI::Datatype &datatype, T *&array, const int part_size)
{
    const int n = comm.Get_size();
    const int r = comm.Get_rank();

    Schedule schedule(n, r);

    perform_schedule(comm, datatype, schedule, r, array, part_size);
}

template<typename T>
bool is_sorted(MPI::Comm &comm, MPI::Datatype &datatype, T *array, const int part_size)
{
    const int procs_num = comm.Get_size();
    const int rank = comm.Get_rank();

    // Checking whether local array is sorted
    bool local_array_sorted = true;
    for(int i = 0; i < part_size - 1; ++i){
        if (array[i + 1] < array[i]){
            local_array_sorted = false;
            break;
        }
    }

    // Checking whether borders are sorted
    T left_max;

    const int right_neighbour = (rank + 1) % procs_num;
    const int left_neighbour = (rank - 1) % procs_num;

    comm.Sendrecv(
        &array[part_size - 1], 1, datatype, right_neighbour, 0,
        &left_max, 1, datatype, left_neighbour, 0
    );

    bool borders_sorted = true;
    if (rank > 0 && array[0] < left_max) {
        borders_sorted = false;
    }

    // Checking whether global array is sorted
    const int local_sorted = local_array_sorted && borders_sorted;
    int sorted;

    comm.Allreduce(&local_sorted, &sorted, 1, MPI::INT, MPI::SUM);

    return sorted == procs_num;
}
