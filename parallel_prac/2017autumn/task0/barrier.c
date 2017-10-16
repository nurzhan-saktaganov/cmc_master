#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

void send(int receiver)
{
    int dummy;
    MPI_Send(&dummy, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
}

void receive(int sender)
{
    int dummy;
    MPI_Recv(&dummy, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void barrier(int rank, int size)
{
    int divider = 1;

    // Phase 1. Gather from all into process with rank = 0
    while (divider < size) {
        divider *= 2;
        // every iteration check who am i: sender or receiver
        if (rank % divider) {
            // I'm sender
            int send_to = rank - divider / 2;
            //printf("Ph1. Send %d => %d\n", rank, send_to);
            send(send_to);
            break;
        }

        int receive_from = rank + divider / 2;
        if (receive_from < size) {
            //printf("Ph1. Receive %d <= %d\n", rank, receive_from);
            receive(receive_from);
        }
    }

    // Phase 2.
    while (divider > 1) {
        if (rank % divider) {
            int receive_from = rank - divider / 2;
            //printf("Ph2. Receive %d <= %d\n", rank, receive_from);
            receive(receive_from);
        } else {
            int send_to = rank + divider / 2;
            if (send_to < size) {
                //printf("Ph2. Send %d => %d\n", rank, send_to);
                send(send_to);
            }
        }
        divider /= 2;
    }

    return;
}

int main(int argc, char* argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Required at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        sleep(1);
        printf("I'm a master. I'm first.\n");
        sleep(1);
    }

    barrier(rank, size);

    if (rank == size - 1) {
        printf("There wasn't any sleep. Nevertheles I'm the second one.\n");
    }

    MPI_Finalize();
    return 0;
}
