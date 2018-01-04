#include <cstring>
#include "Point.hpp"
#include "decompose.hpp"
#include "parallel_sort.hpp"

//represents a 'vector' with given length (i.e. size) and ptr
typedef struct _points_domain_t {
    int k;
    int procs_num;
    int size;
    point_domain_t *info;
} points_domain_t;

points_domain_t _parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain,
    int *sendcounts_buffer, // for using in Scatterv
    int *displacements_buffer // for using in Scatterv
);

int calculate_max_part_size(int procs_num, int part_size, int k);

points_domain_t parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain)
{
    points_domain_t points_domain;

    const int procs_num = comm.Get_size();
    const int max_part_size = calculate_max_part_size(procs_num, part_size, k);

    Point *points_copy = new Point[max_part_size];
    memcpy(points_copy, points, part_size * sizeof(Point));

    //Now we can do everyting with this copy.
    //However, passed user's array will remain unchanged.

    int *sendcounts_buffer = new int [procs_num];
    int *displacements_buffer = new int [procs_num];

    points_domain = _parallel_decompose(
            comm, datatype, points_copy,
            part_size, k, lowest_domain,
            sendcounts_buffer,
            displacements_buffer
        );

    delete [] sendcounts_buffer;
    delete [] displacements_buffer;

    delete [] points_copy;

    return points_domain;
};

points_domain_t _parallel_decompose(
    MPI::Intracomm &comm,
    MPI::Datatype &datatype,
    Point *points,
    int part_size,
    int k,
    int lowest_domain,
    int *sendcounts_buffer, // for using in Scatterv
    int *displacements_buffer // for using in Scatterv
)
{
    const int procs_num = comm.Get_size();
    const int rank = comm.Get_rank();

    points_domain_t points_domain;

    if (procs_num == 1 || k == 1) { // there is no more processes or domains
        points_domain.k = k;
        points_domain.procs_num = procs_num;
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
    int send_count;
    int recv_procs;
    Point *send_points_start;

    if (middle_proc > 0) {
        // we have to send to left
        i_am_receiver = rank < middle_proc;
        color = rank >= middle_proc;
        send_count = middle_index; // elements in position 0, ..., middle_index-1
        send_points_start = points; // send from begining
        recv_procs = middle_proc;
    } else { // middle_proc == 0
        // no way to send to left, only to right
        i_am_receiver = rank > middle_proc;
        color = rank > middle_proc;
        send_count = part_size - middle_index; // elements in position middle_index, ..., part_size-1
        send_points_start = points + middle_index; // send from middle_index and so on
        recv_procs = procs_num - 1;
    }

    // communicate only if required
    if (send_count > 0) {
        memset(sendcounts_buffer, 0, procs_num * sizeof(int));
        memset(displacements_buffer, 0, procs_num * sizeof(int));

        int offset = middle_proc == 0;

        // init sendcounts
        for (int i = 0; i < recv_procs; ++i) {
            sendcounts_buffer[i + offset] = send_count / recv_procs;
        }

        for (int i = 0; i < send_count % recv_procs; ++i) {
            sendcounts_buffer[i + offset] += 1;
        }

        // init displacements
        displacements_buffer[offset] = 0;
        for (int i = 1; i < recv_procs; ++i) {
            displacements_buffer[i + offset] =
                displacements_buffer[i-1 + offset]
                + sendcounts_buffer[i-1 + offset];
        }

        comm.Scatterv(
            send_points_start, // we send from here
            sendcounts_buffer,
            displacements_buffer,
            datatype,
            points + part_size, // we receive to this buffer
            sendcounts_buffer[rank], // we receive this amount of points
            datatype,
            middle_proc
        );

        if (i_am_sender) {
            // make sent elements phantom
            for (int i = 0; i < send_count; ++i) send_points_start[i].index = -1;
        }

        // in all receivers indentify phantom elements
        int recv_n = 0;
        int phantom_elements = 0;
        if (i_am_receiver) {
            recv_n = send_count / recv_procs + (send_count % recv_procs > 0);
            part_size += recv_n;
            phantom_elements = recv_n - sendcounts_buffer[rank];
        }

        for (int i = 0; i < phantom_elements; ++i) {
            points[part_size-1-i].index = -1;
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
    points_domain = _parallel_decompose(
        new_comm, datatype, points,
        part_size, k, lowest_domain,
        sendcounts_buffer,
        displacements_buffer
    );
    Point::switch_sort_way();

    new_comm.Free();

    return points_domain;
}

/*      Trying predict max memory amount to prevent array reallocation.
    So, algorithm is recursive. In worst case we have to redistribute
    send_n := part_size-1 elements from sender. There is always only one sender
    in each recursion step. Let's consider that the amount of receivers is recv_p.
    It means, every receiver has to add at most ceil(send_n / recv_p) elements.
    There are at least recv_p := n_procs / 2 receivers, we finally get new part_size
    for each receiver: part_size := part_size + ceil(send_n / (n_procs / 2)).
    Let's consider that we already have max recursion depth in recursion_depth variable.
    Then we can describe algorithm in pseudocode.

    def max_part_size(part_size, recursion_depth, n_procs):
        while recursion_depth > 0:
            send_n = part_size - 1
            recv_p = n_procs / 2
            part_size += ceil(send_n / recv_p)
            n_procs = ceil(n_procs / 2)
        return part_size

    Recursion depth depends from k and n_procs.
    Exactly, it equals to rounded up value of log(min(k, procs_num)):

    recursion_depth := ceil(log(min(k, n_procs), base=2)).

    But it's too roughly. So, by modelying part_size changing,
    we can get exact value of max part size.
    */
int calculate_max_part_size(int procs_num, int part_size, int k)
{
    if (procs_num == 1 || k == 1) return part_size;
    // searching global middle element's coordinates
    const long long global_length = part_size * procs_num;
    const int k1 = (k + 1) / 2;
    const int k2 = k - k1;
    const long long l1 = 1.0 * k1 / k * global_length;
    //const long long l2 = global_length - l1;

    const int middle_proc = l1 / part_size;
    const int middle_index = l1 % part_size;

    int procs1, procs2;
    int p1_size, p2_size;
    p1_size = p2_size = part_size;

    if (middle_proc > 0) {
        const int send_n = middle_index;
        const int recv_p = middle_proc;
        const int recv_n = send_n / recv_p + (send_n % recv_p > 0);
        p1_size += recv_n;

        procs1 = middle_proc;
        procs2 = procs_num - procs1;
    } else {// middle_proc == 0
        const int send_n = part_size - middle_index;
        const int recv_p = procs_num - 1;
        const int recv_n = send_n / recv_p + (send_n % recv_p > 0);
        p2_size += recv_n;

        procs1 = 1;
        procs2 = procs_num - 1;
    }

    p1_size = calculate_max_part_size(procs1, p1_size, k1);
    p2_size = calculate_max_part_size(procs2, p2_size, k2);

    part_size = p1_size > p2_size ? p1_size : p2_size;
    return part_size;
}
