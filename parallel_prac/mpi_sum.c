#include <stdio.h>
#include <mpi.h>

#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1024

void generate_array(int *array, int size, int range)
{
	int i;
	srand(time(0));
	for (i = 0; i < size; i++){
		array[i] = (1.0 * rand() / RAND_MAX) * range;
	}	
	return;
}

int main(int argc, char** argv)
{
	int mpi_proc_id, mpi_procs_num;
	int *array, array_size;
	int i;
	long sum, local_sum;
	int *local_array, local_array_size;
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_proc_id);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_procs_num);

	if (mpi_proc_id == 0) {
		array_size = ARRAY_SIZE;
		array = (int *) malloc(array_size * sizeof(int));
		generate_array(array, array_size, 100);
	}

	MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	local_array_size = array_size / mpi_procs_num;
	local_array = (int *) malloc(local_array_size * sizeof(int));

	MPI_Scatter(array, local_array_size, MPI_INT, local_array, 
			local_array_size, MPI_INT, 0, MPI_COMM_WORLD);

	local_sum = 0;
	for (i = 0; i < local_array_size; i++){
		local_sum += local_array[i];
	}

	MPI_Reduce(&local_sum, &sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

	free(local_array);

	if (mpi_proc_id == 0){
		printf("Parallel sum = %ld\n", sum);
	}

	/* checking the result */
	if (mpi_proc_id == 0){
		sum = 0;
		for (i = 0; i < array_size; i++){
			sum += array[i];
		}
		printf("Serail sum = %ld\n", sum);
		free(array);
	}

	MPI_Finalize();

	return 0;
}
