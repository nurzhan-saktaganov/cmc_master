#include <algorithm>
#include <mpi.h>

#include "Point.hpp"
#include "Comparator.hpp"
#include "Schedule.hpp"

float x(int i, int j);
float y(int i, int j);
void init_grid(Point *p, int n1, int n2, float (*x)(int, int), float (*y)(int, int));

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
void perform_schedule(const int rank, Schedule &schedule, T *&array, MPI::Datatype datatype, const int part_size)
{
    T *recv_buffer = new T[part_size];
    T *merge_buffer = new T[part_size];
    T *send_buffer = array;

    int neighbour;

    vector<Comparator>::iterator it = schedule.begin();
    for(; it != schedule.end(); ++it){
        neighbour = it->get_pair(rank);
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

    perform_schedule(r, schedule, array, MPI::INT, part_size);

    delete [] array;

    MPI::Finalize();
}


float x(int i, int j)
{
    return i + j;
}

float y(int i, int j)
{
    return i - j;
}

void init_grid(Point *p, int n1, int n2, float (*x)(int, int), float (*y)(int, int))
{
    for (int i = 0; i < n1; ++i){
        for(int j = 0; j < n2; ++j){
            int index = i * n2 + j;
            p[index].index = index;
            p[index].x = x(i, j);
            p[index].y = y(i, j);
        }
    }
}
