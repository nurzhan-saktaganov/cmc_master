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

    // fill points
    srand(time(0));
    for (int counter = 0; counter < part_size; ++counter) {
        const long long index = r * part_size + counter;
        if (index >= length) break;

        const int i = index / n2;
        const int j = index % n2;

        points[counter].index = index;
        points[counter].x = i + 0.25 - 0.5 * rand() / RAND_MAX;
        points[counter].y = j + 0.25 - 0.5 * rand() / RAND_MAX;
    }

    // explicit mark phantom elements' index
    if (r == n - 1) {
        const int phantom_elements = (n - length % n) % n;
        for (int i = 0; i < phantom_elements; ++i){
            points[part_size - 1 - i].index = -1;
        }
    }

    MPI::Datatype datatype = Point::datatype();
    datatype.Commit();

    points_domain_t points_domain;
    points_domain = parallel_decompose(MPI::COMM_WORLD, datatype, points, part_size, k, 0);

    datatype.Free();

    //MPI::File file = MPI::File::Open(MPI::COMM_WORLD, filename,
    //        MPI::MODE_CREATE || MPI::MODE_WRONLY, MPI::INFO_NULL);

    if (r == 0) {
        cout<<"hello\n";
        for (int counter = 0; counter < points_domain.size; ++counter) {
            Point p = points_domain.info[counter].point;
            int domain = points_domain.info[counter].domain;

            const int i = p.index / n2;
            const int j = p.index % n2;

            cout<<i<<" "<<j<<" "<<p.x<<" "<<p.y<<" "<<domain<<"\n";
        }
    }



    //file.Close();

    // release resources
    points_domain.size = 0;
    delete [] points_domain.info;

    delete [] points;

    MPI::Finalize();

    return 0;
}
