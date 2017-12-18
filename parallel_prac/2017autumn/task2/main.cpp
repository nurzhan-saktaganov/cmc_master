#include <algorithm>
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>

#include "parallel_sort.hpp"
#include "Point.hpp"
#include "dh_sort.hpp"

enum errors {
    not_enough_arguments = -1,
    invalid_arguments    = -2,
};

int main(int argc, char *argv[])
{
    int n1, n2, num_threads;

    if (argc < 4) return not_enough_arguments;

    try {
        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);
        num_threads = atoi(argv[3]);
    } catch (invalid_argument) {
        return invalid_arguments;
    }

    if (n1 <= 0 || n2 <= 0 || num_threads <= 0) return invalid_arguments;

    omp_set_num_threads(num_threads);

    const int length = n1 * n2;

    MPI::Init(argc, argv);
    const int n = MPI::COMM_WORLD.Get_size();
    const int r = MPI::COMM_WORLD.Get_rank();

    const int part_size = length / n + (length % n > 0);

    Point *array = new Point[part_size];

    // initialize array with random elements
    srand(time(0));
    for (int i = 0; i < part_size; i++) {
        array[i].y = array[i].x = rand();
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

    parallel_sort(MPI::COMM_WORLD, datatype, array, part_size);

    MPI::COMM_WORLD.Barrier();
    duration += MPI::Wtime();

    const bool sorted = is_sorted(MPI::COMM_WORLD, datatype, array, part_size);

    datatype.Free();
    delete [] array;

    if (r == 0) {
        std::cout<<"Sorting "<<n1 * n2<<" elements on "<<n<<" processes took "<<duration<<" seconds.\n";
        if (sorted) {
            std::cout<<"Global array is sorted :)"<<std::endl;
        } else {
            std::cout<<"Global array is not sorted :("<<std::endl;
        }
    }

    MPI::Finalize();

    return 0;
}
