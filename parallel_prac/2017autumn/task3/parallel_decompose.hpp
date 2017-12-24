#include <cstring>
#include "Point.hpp"
#include "decompose.hpp"
#include "parallel_sort.hpp"

//represents a 'vector' with given length (i.e. size) and ptr
typedef struct _points_domain_t {
    int size;
    point_domain_t *info;
} points_domain_t;

points_domain_t _parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain);


points_domain_t parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain)
{
    points_domain_t points_domain;

    //Trying predict max recursion depth to prevent array reallocation.
    //So, we would have enough space to store new
    //elements (points) due to redistribution.
    //We assume that every level of recursion causes at most
    //one new fictive element addition.
    //Allocating part_size + recursion depth, we can no worry about reallocation.
    //Just increment part_size by 1.
    //Recursion depth depends from k and num_procs.
    //Exactly, it equals to rounded up value of log(min(k, procs_num)).

    int min = comm.Get_size();
    if (k < min) min = k;

    int recursion_depth = 0;
    while (min > (1 << recursion_depth)) ++recursion_depth;

    //Now we can do everyting with this copy.
    //However, passed user's array will remain unchanged.
    Point *points_copy = new Point[part_size + recursion_depth];
    memcpy(points_copy, points, part_size * sizeof(Point));

    points_domain = _parallel_decompose(
            comm, datatype, points_copy,
            part_size, k, lowest_domain
        );

    delete [] points_copy;

    return points_domain;
};

points_domain_t _parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain)
{
    const int procs_num = comm.Get_size();
    const int rank = comm.Get_rank();

    points_domain_t points_domain;

    if (procs_num == 1 || k == 1) { // there is no more processes or domains
        points_domain.size = part_size;
        points_domain.info = new point_domain_t[part_size];
        decompose(points, points_domain.info, part_size, k, lowest_domain);
        return points_domain;
    }

    std::sort(points, points + part_size); // sort each local array
    parallel_sort(comm, datatype, points, part_size);

    //now we have global sorted array of points

    // searching global middle element's coordinates
    const long long global_length = part_size * procs_num;
    const int k1 = (k + 1) / 2;
    const int k2 = k - k1;
    const long long l1 = 1.0 * k1 / k * global_length;
    //const long long l2 = global_length - l1;

    const int middle_proc = l1 / part_size;
    const int middle_index = l1 % part_size;

    MPI::Cartcomm new_comm;
    int color; // 0 - left half, 1 - right half
    bool i_am_sender = rank == middle_proc;
    bool i_am_receiver;
    int count_to_send;
    Point *send_points_start;

    if (middle_proc > 0) {
        // we have to send to left
        i_am_receiver = rank < middle_proc;
        color = rank >= middle_proc;
        count_to_send = middle_index; // elements in position 0, ..., middle_index-1
        send_points_start = points; // send from begining
    } else { // middle_proc == 0
        // no way to send to left, only to right
        i_am_receiver = rank > middle_proc;
        color = rank > middle_proc;
        count_to_send = part_size - middle_index; // elements in position middle_index, ..., part_size-1
        send_points_start = points + middle_index; // send from middle_index and so on
    }

    // communicate only if required
    if (count_to_send > 0) {
        Point *send_buffer;

        if (i_am_sender) {
            send_buffer = new Point[procs_num];
            // +1 because we send to ourself too
            memcpy(send_buffer + 1, send_points_start, count_to_send * sizeof(Point));
        }

        Point received;
        comm.Scatter(send_buffer, 1, datatype,
            &received, 1, datatype, middle_proc);

        if (i_am_receiver) {
            points[part_size++] = received;
        }

        if (i_am_sender) {
            delete [] send_buffer;
        }
    }

    if (color == 0) {
        k = k1;
    } else { // color == 1
        k = k2;
        lowest_domain += k1;
    }

    new_comm = comm.Split(color, rank);

    Point::switch_sort_way();
    points_domain = _parallel_decompose(new_comm, datatype, points, part_size, k, lowest_domain);
    Point::switch_sort_way();

    new_comm.Free();

    return points_domain;
}
