#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <mpi.h>

#include "decompose.hpp"
#include "parallel_decompose.hpp"

enum errors {
    not_enough_arguments = -1,
    invalid_arguments    = -2,
};

using namespace std;

long long count_cuts(MPI::Intracomm &comm, points_domain_t points_domain, const int n1, const int n2);
int index_to_i(const long long index, const int n2);
int index_to_j(const long long index, const int n2);
long long coord_to_index(const int n1, const int n2, const int i, const int j);
long long left_neighbour(long long index, const int n1, const int n2);
long long right_neighbour(long long index, const int n1, const int n2);
long long top_neighbour(long long index, const int n1, const int n2);
long long bottom_neighbour(long long index, const int n1, const int n2);

void fill_points(
    const int part_size,
    const int procs_num,
    const int rank,
    const int n1,
    const int n2,
    Point *points);

void write_result(
    MPI::Intracomm &comm,
    const char *filename,
    points_domain_t points_domain,
    const int part_size,
    const int n1,
    const int n2,
    const int k);

int get_width(int number);

int main(int argc, char *argv[])
{
    long long n1, n2;
    int k;
    char *filename;

    if (argc < 5) return not_enough_arguments;

    try {
        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);
        k = atoi(argv[3]);
        filename = argv[4];
    } catch (invalid_argument) {
        return invalid_arguments;
    }

    if (n1 <= 0 || n2 <= 0 || k <= 0) return invalid_arguments;

    MPI::Init(argc, argv);

    const int procs_num = MPI::COMM_WORLD.Get_size();
    const int rank = MPI::COMM_WORLD.Get_rank();

    const long long length = n1 * n2;
    const long long part_size = length / procs_num + (length % procs_num > 0);

    Point *points = new Point[part_size];

    fill_points(part_size, procs_num, rank, n1, n2, points);

    MPI::Datatype datatype = Point::datatype();
    datatype.Commit();

    points_domain_t points_domain;

    if (procs_num == 1) {
        points_domain.k = k;
        points_domain.procs_num = procs_num;
        points_domain.size = part_size;
        points_domain.info = new point_domain_t[part_size];
    }

    MPI::COMM_WORLD.Barrier();
    double duration = -MPI::Wtime();

    if (procs_num == 1) {
        decompose(points, points_domain.info, part_size, k, 0);
    } else {
        points_domain = parallel_decompose(
                MPI::COMM_WORLD, datatype, points, part_size, k, 0
            );
    }

    MPI::COMM_WORLD.Barrier();
    duration += MPI::Wtime();

    datatype.Free();

    // There is no more need on points.
    delete [] points;

    write_result(MPI::COMM_WORLD, filename, points_domain, part_size, n1, n2, k);

    // Works only for p < k.
    long long cuts = count_cuts(MPI::COMM_WORLD, points_domain, n1, n2);

    if (rank == 0) {
        cout<<cuts<<"\n"<<duration<<endl;
    }

    // release resources
    points_domain.size = 0;
    delete [] points_domain.info;

    MPI::Finalize();

    return 0;
}

void fill_points(
    const int part_size,
    const int procs_num,
    const int rank,
    const int n1,
    const int n2,
    Point *points)
{
    const int length = n1 * n2;
    // fill points
    srand(time(0));
    for (int counter = 0; counter < part_size; ++counter) {
        const long long index = rank * part_size + counter;
        if (index >= length) break;

        const int i = index / n2;
        const int j = index % n2;

        points[counter].index = index;
        points[counter].x = i + 0.25 * (1.0 - 2.0 * rand() / RAND_MAX);
        points[counter].y = j + 0.25 * (1.0 - 2.0 * rand() / RAND_MAX);
    }

    // explicit mark phantom elements' index
    if (rank == procs_num - 1) {
        const int phantom_elements = (procs_num - length % procs_num) % procs_num;
        for (int i = 0; i < phantom_elements; ++i){
            points[part_size - 1 - i].index = -1;
        }
    }
}

void write_result(
    MPI::Intracomm &comm,
    const char *filename,
    points_domain_t points_domain,
    const int part_size,
    const int n1,
    const int n2,
    const int k)
{
    const int procs_num = comm.Get_size();
    const int rank = comm.Get_rank();

    MPI::File file = MPI::File::Open(comm, filename,
            MPI::MODE_CREATE | MPI::MODE_WRONLY, MPI::INFO_NULL);

    // Collect information about how many points in each process.
    int *procs_points = new int [procs_num];
    int my_points = 0;
    for (int i = 0; i < points_domain.size; ++i) {
        if (points_domain.info[i].point.index != -1) ++my_points;
    }
    comm.Allgather(&my_points, 1, MPI::INT, procs_points, 1, MPI::INT);
    // Now we have information how many points in each process.

    // How many lines skip before.
    int skip_lines = 0;
    for (int i = 0; i < rank; ++i) skip_lines += procs_points[i];

    // write point info
    // every line has format:
    // "%d %d %f %f %d\n"
    const int i_w = get_width(n1);
    const int j_w = get_width(n2);
    const int k_w = get_width(k);
    const int float_digits = 13; // max chars: -1.348767e-01
    const int separators = 5;

    // Length of each line in bytes.
    const int line_width = i_w + j_w + k_w + 2 * float_digits + separators;
    const int buff_size = 10 * (1 << 20); // 10MiB
    char *buffer = new char [buff_size];

    file.Set_view(1.0L * line_width * skip_lines, MPI::CHAR, MPI::CHAR, "native", MPI::INFO_NULL);

    int free_space = buff_size;
    char *write_position = buffer;

    for (int counter = 0; counter < points_domain.size; ++counter) {
        Point p = points_domain.info[counter].point;
        if (p.index == -1) continue;

        int domain = points_domain.info[counter].domain;
        const int i = p.index / n2;
        const int j = p.index % n2;

        if (free_space < line_width) {
            file.Write(buffer, buff_size - free_space, MPI::CHAR);
            free_space = buff_size;
            write_position = buffer;
        }
        sprintf(write_position, "%*d %*d %*e %*e %*d\n",
                i_w, i, j_w, j,
                float_digits, p.x,
                float_digits, p.y,
                k_w, domain
            );

        free_space -= line_width;
        write_position += line_width;
    }

    if (free_space != buff_size) {
        file.Write(buffer, buff_size - free_space, MPI::CHAR);
    }

    delete [] buffer;

    delete [] procs_points;

    file.Close();
}

int get_width(int number)
{
    int width = 1;
    while (number >= 10) {
        number /= 10;
        width++;
    }
    return width;
}

long long count_cuts(MPI::Intracomm &comm, points_domain_t points_domain, const int n1, const int n2)
{
    std::map<int, std::map<long long, bool> > has_node; // for each domain store indexes

    // only when k > p;

    for(int i = 0; i < points_domain.size; ++i) {
        Point p = points_domain.info[i].point;
        if (p.index == -1) continue;
        int domain = points_domain.info[i].domain;
        has_node[domain][p.index] = true;
    }

    long long cuts = 0;
    for(int i = 0; i < points_domain.size; ++i) {
        Point p = points_domain.info[i].point;
        if (p.index == -1) continue;
        int domain = points_domain.info[i].domain;

        long long neighbour;

        neighbour = left_neighbour(p.index, n1, n2);
        if (neighbour != -1 && !has_node[domain][neighbour]) {
            ++cuts;
        }

        neighbour = right_neighbour(p.index, n1, n2);
        if (neighbour != -1 && !has_node[domain][neighbour]) {
            ++cuts;
        }

        neighbour = top_neighbour(p.index, n1, n2);
        if (neighbour != -1 && !has_node[domain][neighbour]) {
            ++cuts;
        }

        neighbour = bottom_neighbour(p.index, n1, n2);
        if (neighbour != -1 && !has_node[domain][neighbour]) {
            ++cuts;
        }
    }

    long long total_cuts;
    comm.Allreduce(&cuts, &total_cuts, 1, MPI::LONG_LONG, MPI::SUM);

    return total_cuts / 2;
}

int index_to_i(const long long index, const int n2)
{
    return index / n2;
}

int index_to_j(const long long index, const int n2)
{
    return index % n2;
}

long long coord_to_index(const int n1, const int n2, const int i, const int j)
{
    if (i < 0 || n1 <= i || j < 0 || n2 <= j) return -1;
    return 1L * i * n2 + j;
}

long long left_neighbour(long long index, const int n1, const int n2)
{
    const int i = index_to_i(index, n2);
    const int j = index_to_j(index, n2);
    return coord_to_index(n1, n2, i, j - 1);
}

long long right_neighbour(long long index, const int n1, const int n2)
{
    const int i = index_to_i(index, n2);
    const int j = index_to_j(index, n2);
    return coord_to_index(n1, n2, i, j + 1);
}

long long top_neighbour(long long index, const int n1, const int n2)
{
    const int i = index_to_i(index, n2);
    const int j = index_to_j(index, n2);
    return coord_to_index(n1, n2, i - 1, j);
}

long long bottom_neighbour(long long index, const int n1, const int n2)
{
    const int i = index_to_i(index, n2);
    const int j = index_to_j(index, n2);
    return coord_to_index(n1, n2, i + 1, j);
}