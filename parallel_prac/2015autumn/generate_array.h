#ifndef GENERATE_ARRAY_H
#define GENERATE_ARRAT_H
#include <stdlib.h>
#include <time.h>

void generate_array(int *array, int size, int range)
{
	srand(time(0));
	for(int i = 0; i < size; i++){
		array[i] = (1.0 * rand() / RAND_MAX) * range;
	}
}

#endif
