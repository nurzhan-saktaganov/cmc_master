#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tridiagonal.h"

#define TIME_SLICES 200 
#define X_AXE_SLICES 10
#define T 1.0

/* print matrix A(M,N) in file file_name */
void print_matrix(char *file_name, double **A, int M, int N){
	FILE *file;
	int i, j;

	file = fopen(file_name, "w");

	for(i = 0; i < M + 1; i++){
		for(j = 0; j < N + 1; j++) {
			fprintf(file, "%lf ", A[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);

	return;
}

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

/* task realization */
void implicit_method( \
						double **Y /* Y[t][x], Y[t][x] instead of Y[x][t] for better memory alignment */, \
						int M, /* time slices */ \
						int N, /* x-axe slices */ \
						double time /* T */, \
						double mu, \
						double (* start_condition)(double /*x*/), \
						double (* left_border_condition)(double /*t*/), \
						double (* right_border_condition)(double /*t*/), \
						double (* heat_source_function)(double /*x*/, double /*t*/) \
					) 
{
	double dh, dt;
	double x, t;
	int i, k;
	double lambda;
	/* for tridiagonal algorithm */
	double *a, *b, *c, *y, *f;

	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	lambda = dh * dh / (mu * dt);

	/* We have N + 1 points, and N - 1 of them is inner */
	a = (double *) malloc((N - 1) * sizeof(double));
	b = (double *) malloc((N - 1) * sizeof(double));
	c = (double *) malloc((N - 1) * sizeof(double));
	y = (double *) malloc((N - 1) * sizeof(double));
	f = (double *) malloc((N - 1) * sizeof(double));

	/* init start condition */
	for(i = 0; i < N + 1; i++){
		x = i * dh;
		Y[0][i] = start_condition(x);
	}

	/* init border conditions */
	for(k = 0; k < M + 1; k++){
		t = k * dt;
		Y[k][0] = left_border_condition(t);
		Y[k][N] = right_border_condition(t);
	}

	/* */
	for(i = 0; i < N - 1; i++){
		a[i] = -1.0;
		b[i] = 2 + lambda;
		c[i] = -1.0;
	}

	a[0] = 0.0;
	c[N - 2] = 0.0;

	for(k = 1; k < M + 1; k++){
		t = k * dt;
		for(i = 1; i < N ; i++){
			x = i * dh;
			f[i - 1] = lambda * (Y[k - 1][i] + dt * heat_source_function(x, t));
		}

		f[0] += Y[k][0] /* left_border_condition(t)*/;
		f[N - 2] += Y[k][N]/*right_border_condition(t) */;

		tridiagonal(a, b, c, y, f, N - 1);
		
		memcpy(Y[k] + 1, y, (N - 1) * sizeof(double));
	}

	free(f);
	free(y);
	free(c);
	free(b);
	free(a);

	return;
}

/* for test */
void fill_true_values(\
						double **U/* U[t][x], U[t][x] instead of U[x][t] for better memory alignment */, \
						int M, \
						int N, \
						double time, \
						double (* analytical_solution)(double /*x*/, double /*t*/)\
					)
{
	int i, k;
	double x, t;
	double dh, dt;

	dt = time / M;
	dh = 1.0 / N;

	for( k = 0; k < M + 1; k++)
		for( i = 0; i < N + 1; i++){
			t = k * dt;
			x = i * dh;
			U[k][i] = analytical_solution(x, t);
		}
	
	return;
}

/* max abs difference */
double test_answer(\
					double **Y, \
					double **U, \
					int M, \
					int N \
				)
{
	double max_eps = 0.0;
	double local_eps;
	int i, k;

	for(k = 0; k < M + 1; k++){
		for(i = 0; i < N + 1; i++){
			local_eps = fabs(Y[k][i] - U[k][i]);
			max_eps = (max_eps > local_eps ? max_eps : local_eps);
		}
	}

	return max_eps;
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

	implicit_method(Y, M, N, /*T=*/T, /*mu=*/1.0, \
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