#include <algorithm>
#include <mpi.h>
#include <iostream>

#include "Point.hpp"
#include "Comparator.hpp"
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
void perform_schedule(Schedule &schedule, const int rank, T *&array, const int part_size, MPI::Datatype &datatype)
{
    T *send_buffer = array;
    T *recv_buffer = new T[part_size];
    T *merge_buffer = new T[part_size];

    vector<Comparator>::iterator it = schedule.begin();
    for(; it != schedule.end(); ++it){
        const int neighbour = it->get_pair(rank);
        MPI::COMM_WORLD.Sendrecv(
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
bool is_sorted(const int procs_num, const int rank, T *array, const int part_size, MPI::Datatype &datatype)
{
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

    MPI::COMM_WORLD.Sendrecv(
        &array[part_size - 1], 1, datatype, right_neighbour, 0,
        &left_max, 1, datatype, left_neighbour, 0
    );

    bool borders_sorted = true;
    if (rank > 0 && array[0] < left_max) {
        borders_sorted = false;
    }

    // Checking whether global array is sorted
    const bool local_sorted = local_array_sorted && borders_sorted;
    bool sorted;

    MPI::COMM_WORLD.Allreduce(&local_sorted, &sorted, 1, MPI::BOOL, MPI::LAND);

    return sorted;
}

int main(int argc, char *argv[])
{
    const int n1 = 100;
    const int n2 = 100;
    const int length = n1 * n2;

    MPI::Init(argc, argv);
    const int n = MPI::COMM_WORLD.Get_size();
    const int r = MPI::COMM_WORLD.Get_rank();

    const int part_size = length / n + (length % n > 0);

    Schedule schedule(n, r);

    Point *array = new Point[part_size];

    std::sort(array, array + part_size);

    MPI::Datatype datatype = Point::datatype();
    datatype.Commit();

    perform_schedule(schedule, r, array, part_size, datatype);
    const bool sorted = is_sorted(n, r, array, part_size, datatype);

    datatype.Free();
    delete [] array;

    if (r == 0) {
        if (sorted) {
            std::cout<<"Global array is sorted :)"<<std::endl;
        } else {
            std::cout<<"Global array is not sorted :("<<std::endl;
        }
    }

    MPI::Finalize();
}
