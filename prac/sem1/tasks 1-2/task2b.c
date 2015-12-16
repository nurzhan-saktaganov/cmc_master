#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "aux.h"
#include "methods.h"

#define TIME_SLICES 450
#define X_AXE_SLICES 10
#define T 1.0

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
	return - (1.0 + 4 * (pow(x, 4.0) - 2 * pow(x, 2.0) - 0.5 * x + 0.1875) / (1.0 + t) ) \
			* exp(-(x - 0.5) * (x - 0.5) - t);
}

/* from test function */
double analytical_solution(double x, double t)
{
	return exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/ ;
}

double mu_function(double x, double t)
{
	return (x + 0.5) * (x + 0.5) / (1.0 + t);
}

int main(int argc, char **argv)
{
	double **Y, **U;
	double max_eps;
	int i;
	int M = TIME_SLICES;
	int N = X_AXE_SLICES;

	/* allocate memory */ 
	Y = (double **) calloc(M + 1, sizeof(double *));
	for(i = 0; i < M + 1; i++){
		Y[i] = (double *) calloc(N + 1, sizeof(double));
	}


	U = (double **) calloc(M + 1, sizeof(double *));
	for(i = 0; i < M + 1; i++){
		U[i] = (double *) calloc(N + 1, sizeof(double));
	}

	implicit_method_2(Y, M, N, /*T=*/T, \
					mu_function, \
					start_condition, \
					left_border_condition, \
					right_border_condition, \
					heat_source_function);

	fill_true_values(U, M, N, /*T=*/T, analytical_solution);

	max_eps = test_answer(Y, U, M, N);

	printf("max eps: %lf\n", max_eps);
	
	switch (argc) {
		case 3:
			print_matrix(argv[2], U, M, N);
			/* no break */
		case 2:
			print_matrix(argv[1], Y, M, N);
			/* no break */
		default:
			break;
	}
	
	/* deallocation memory */
	for(i = 0; i < M + 1; i++){
		free(Y[i]);
		free(U[i]);
	}

	free(Y);
	free(U);

	return 0;
}