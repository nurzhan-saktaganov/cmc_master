#include <stdexcept>
#include <algorithm>
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <stdint.h>

#include "parallel_sort.hpp"
#include "Point.hpp"
#include "dh_sort.hpp"

enum errors {
    not_enough_arguments = -1,
    invalid_arguments    = -2,
};

int main(int argc, char *argv[])
{
    int64_t n1, n2;
    int num_threads;

    if (argc < 4) return not_enough_arguments;

    try {
        n1 = atoll(argv[1]);
        n2 = atoll(argv[2]);
        num_threads = atoi(argv[3]);
    } catch (std::invalid_argument) {
        return invalid_arguments;
    }

    if (n1 <= 0 || n2 <= 0 || num_threads <= 0) return invalid_arguments;

    omp_set_num_threads(num_threads);
    num_threads = omp_get_max_threads();

    const int64_t length = n1 * n2;

    MPI::Init(argc, argv);
    const int n = MPI::COMM_WORLD.Get_size();
    const int r = MPI::COMM_WORLD.Get_rank();

    const int64_t part_size = length / n + (length % n > 0);

    Point *array = new Point[part_size];

    // initialize array with random elements
    srand(time(0));
    for (int counter = 0; counter < part_size; ++counter) {
        const long long index = r * part_size + counter;
        if (index >= length) break;

        const int i = index / n2;
        const int j = index % n2;

        array[counter].index = index;
        array[counter].x = i + 0.25 * (1.0 - 2.0 * rand() / RAND_MAX);
        array[counter].y = j + 0.25 * (1.0 - 2.0 * rand() / RAND_MAX);
    }

    // explicit mark phantom elements' index
    if (r == n - 1) {
        const int phantom_elements = (n - length % n) % n;
        for (int i = 0; i < phantom_elements; ++i){
            array[part_size - 1 - i].index = -1;
        }
    }

    MPI::Datatype datatype = Point::datatype();
    datatype.Commit();

    MPI::COMM_WORLD.Barrier();
    double duration = -MPI::Wtime();

    dh_sort(array, part_size);
    if (n > 1 ) {
        parallel_sort(MPI::COMM_WORLD, datatype, array, part_size);
    }

    MPI::COMM_WORLD.Barrier();
    duration += MPI::Wtime();

    const bool sorted = is_sorted(MPI::COMM_WORLD, datatype, array, part_size);

    datatype.Free();
    delete [] array;

    if (r == 0) {
        std::cout<<"Sorting "<<n1 * n2<<" elements on "<<n<<" processes ("<<num_threads<<" threads each) took "<<duration<<" seconds.\n"
            <<"sizeof(float): "<<sizeof(float)<<"\n"
            <<"sizeof(long long): "<<sizeof(long long)<<"\n"
            <<"sizeof(Point): "<<sizeof(Point)<<"\n";

        if (sorted) {
            std::cout<<"Global array is sorted :)"<<std::endl;
        } else {
            std::cout<<"Global array is not sorted :("<<std::endl;
        }
    }

    MPI::Finalize();

    return 0;
}
