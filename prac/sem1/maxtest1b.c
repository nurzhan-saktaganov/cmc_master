#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "aux.h"
#include "methods.h"

#define TIME_SLICES 20000
#define X_AXE_SLICES 20000
#define T 1.0
#define GiB *1024L*1024L*1024L
#define MAX_MEMORY 4 GiB

/* from test function */
double start_condition(double x)
{
	return exp(-(x - 0.5) * (x - 0.5));
}

/* from test function */
double left_border_condition(double t)
{
	return exp(-0.25 - t);
}

/* from test function */
double right_border_condition(double t)
{
	return exp(-0.25 - t);
}

/* from test function */
double heat_source_function(double x, double t)
{
	return 4 * x * (1 - x) \
				* exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/;
}

/* from test function */
double analytical_solution(double x, double t)
{
	return exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/ ;
}



int main(int argc, char **argv)
{
	double **Y;
	int i;
	int M = TIME_SLICES;
	int N = X_AXE_SLICES;

	unsigned long memory = (M * N + 2 * M + 7 * N - 2) * sizeof(double);

	if (memory > MAX_MEMORY){
		printf("Not enough memory:\n");
		printf("\tavailable = %.2lf GiB\n", MAX_MEMORY / (1.0 GiB));
		printf("\trequired = %.2lf GiB\n", memory / (1.0 GiB));
		return 0;
	}

	/* allocate memory */ 
	Y = (double **) calloc(M + 1, sizeof(double *));
	for(i = 0; i < M + 1; i++){
		Y[i] = (double *) calloc(N + 1, sizeof(double));
	}

	implicit_method_1(Y, M, N, /*T=*/T, /*mu=*/1.0, \
					start_condition, \
					left_border_condition, \
					right_border_condition, \
					heat_source_function);

	/* deallocation memory */
	for(i = 0; i < M + 1; i++){
		free(Y[i]);
	}

	free(Y);

	return 0;
}