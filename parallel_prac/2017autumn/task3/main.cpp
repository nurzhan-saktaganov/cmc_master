#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <mpi.h>

#include "decompose.hpp"

enum errors {
    not_enough_arguments = -1,
    invalid_arguments    = -2,
};

using namespace std;

int main(int argc, char *argv[])
{
    int n1, n2, k;

    if (argc < 4) return not_enough_arguments;

    try {
        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);
        k = atoi(argv[3]);
    } catch (invalid_argument) {
        return invalid_arguments;
    }

    if (n1 <= 0 || n2 <= 0 || k <= 0) return invalid_arguments;

    MPI::Init(argc, argv);

    //TODO do something

    MPI::Finalize();

    return 0;
}
