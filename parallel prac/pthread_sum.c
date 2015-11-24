#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

typedef struct _worker_param {
	int threads_num;
	int thread_id;
	int array_size;
	int *array;
	long *local_sums;
} worker_param;

void generate_array(int *array, int size, int range)
{
	int i;
	
	srand(time(0));
	for(i = 0; i < size; i++){
		array[i] = (1.0 * rand() / RAND_MAX) * range;
	}	
	
	return;
}

void* worker(void *arg){
	int threads_num;
	int thread_id;
	int array_size;
	int *array;
	long *local_sum;
	int begin, end;
	int i;
	
	/* init params */
	threads_num = ((worker_param *) arg)->threads_num;
	thread_id = ((worker_param *) arg)->thread_id;
	array_size = ((worker_param *) arg)->array_size;
	array = ((worker_param *) arg)->array;
	local_sum = &(((worker_param *) arg)->local_sums[thread_id]);
	
	*local_sum = 0;
	
	begin = array_size * thread_id / threads_num;
	end = array_size * (thread_id + 1) / threads_num;
	
	for(i = begin; i < end; i++)
	{
		*local_sum += array[i];
	}
	
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	int array_size;
	int *array;
	int threads_num;
	long array_sum, *local_sums;
	int i;
	pthread_t *tids;
	worker_param *params;
	
	printf("Array size: ");
	scanf("%d", &array_size);
	printf("Threads num: ");
	scanf("%d", &threads_num);
	
	array = (int *) malloc(array_size * sizeof(int));
	generate_array(array, array_size, 100);
	
	tids = (pthread_t *) malloc(threads_num * sizeof(pthread_t));
	params = (worker_param *) malloc(threads_num * sizeof(worker_param));
	local_sums = (long *) malloc(threads_num * sizeof(long));
	

	for(i = 0; i < threads_num; i++){
		/* init param of thread */
		params[i].threads_num = threads_num;
		params[i].thread_id = i;
		params[i].array_size = array_size;
		params[i].array = array;
		params[i].local_sums = local_sums;

		pthread_create(&(tids[i]), NULL, worker, &(params[i]));
	}
	
	array_sum = 0;
	for(i = 0; i < threads_num; i++){
		pthread_join(tids[i], NULL);
		array_sum += local_sums[i];
	}

	printf("Parallel sum = %ld\n", array_sum);

	/* cheking the result */
	array_sum = 0;
	for(i = 0; i < array_size; i++){
		array_sum += array[i];
	}

	printf("Serial sum = %ld\n", array_sum);
	
	free(local_sums);
	free(params);
	free(tids);
	free(array);
	
	return 0;
}
