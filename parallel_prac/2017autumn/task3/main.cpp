#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <mpi.h>

#include "parallel_decompose.hpp"

enum errors {
    not_enough_arguments = -1,
    invalid_arguments    = -2,
};

using namespace std;

void fill_points(
    const int length,
    const int part_size,
    const int procs_num,
    const int rank,
    const int n2,
    Point *points);

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

    const int n = MPI::COMM_WORLD.Get_size();
    const int r = MPI::COMM_WORLD.Get_rank();

    const long long length = n1 * n2;
    const long long part_size = length / n + (length % n > 0);

    Point *points = new Point[part_size];

    fill_points(part_size, n, r, n1, n2, points);

    MPI::Datatype datatype = Point::datatype();
    datatype.Commit();

    MPI::COMM_WORLD.Barrier();
    double duration = -MPI::Wtime();

    points_domain_t points_domain = parallel_decompose(
            MPI::COMM_WORLD, datatype, points, part_size, k, 0
        );

    MPI::COMM_WORLD.Barrier();
    duration += MPI::Wtime();

    datatype.Free();

    MPI::File file = MPI::File::Open(MPI::COMM_WORLD, filename,
            MPI::MODE_CREATE || MPI::MODE_WRONLY, MPI::INFO_NULL);

    // to store how many points in each process.
    int *procs_points = new int [n];
    {
        int my_points = 0;
        for (int i = 0; i < points_domain.size; ++i) {
            if (points_domain.info[i].point.index != -1) ++my_points;
        }
        MPI::COMM_WORLD.Allgather(&my_points, 1, MPI::INT, procs_points, 1, MPI::INT);
        // Now we have information how many points in each process.
    }

    int skip_lines = 0;
    for (int i = 0; i < r; ++i) skip_lines += procs_points[i];

    for (int i = 0; i < n; ++i) {
        MPI::COMM_WORLD.Barrier();
        if (i != r) continue;

        for (int counter = 0; counter < points_domain.size; ++counter) {
            Point p = points_domain.info[counter].point;
            if (p.index == -1) continue;

            int domain = points_domain.info[counter].domain;
            const int i = p.index / n2;
            const int j = p.index % n2;

            cout<<i<<" "<<j<<" "<<p.x<<" "<<p.y<<" "<<domain<<"\n";
        }
    }

    delete [] procs_points;

    file.Close();

    // release resources
    points_domain.size = 0;
    delete [] points_domain.info;

    delete [] points;

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
