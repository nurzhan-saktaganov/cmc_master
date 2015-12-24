#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>

/*
du/dt = mu * d^2u/dx^2 + f(x,t)
u(x, 0) - start_condition
u(0, x) - left_border_condition
u(1, x) - right_border_condition
f(x, t) - heat_source_function
u(x, t) - analytical_solution
the stability condition: mu * dt < dh^2
*/

double analytical_solution(double x, double t);
double start_condition(double x);
double left_border_condition(double t);
double right_border_condition(double t);
double heat_source_function(double x, double t);

double euler_explicit_parallel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double mu,
	double time, 
	int M, /* time slices */
	int N /* x-axe slices */);

int main(int argc, char **argv)
{
	double time_par;
	double max_eps_par;
	int proc_rank, proc_nums;

	double mu, T;
	int M, N;
	
	MPI_Init(&argc, &argv);

	mu = 1.0;
	T = 1.0;
	/* the stability condition: M >= 2 * mu* T * N^2 */
	M = 132098;
	N = 257;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_nums);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	
	time_par = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	max_eps_par = euler_explicit_parallel_test(
		analytical_solution,
		start_condition,
		left_border_condition,
		right_border_condition,
		heat_source_function,
		mu, T, M, N);
	MPI_Barrier(MPI_COMM_WORLD);
	time_par = MPI_Wtime() - time_par;

	if (proc_rank == 0){
		printf("x-axe slices: %d\n", M);
		printf("time slices: %d\n", N);
		printf("parallel algorithm time: %lf\n", time_par);
		printf("parallel algorithm error: %lf\n", max_eps_par);
	}

	MPI_Finalize();

	return 0;
}

double start_condition(double x)
{
	return exp(-(x - 0.5) * (x - 0.5));
}

double left_border_condition(double t)
{
	return exp(-0.25 - t);
}

double right_border_condition(double t)
{
	return exp(-0.25 - t);
}

double heat_source_function(double x, double t)
{
	/* for mu=1 */
	return 4 * x * (1 - x) \
				* exp(-(x - 0.5) * (x - 0.5) - t); // exp(-t)
}

double analytical_solution(double x, double t)
{
	return exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/ ;
}

inline double max(double a, double b){
	return a > b ? a : b;
}

double euler_explicit_parallel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double mu,
	double time, 
	int M, /* time slices */
	int N /* x-axe slices */)
{
	double dh, dt;
	double x, t, t_next;
	int i, k;
	double lyambda;
	double *lptl; /* local previous time layer */
	double *lctl; /* local current time layer */
	double *tmp_ptr;
	double max_eps, eps;

	int proc_rank, proc_nums;
	int local_size;
	int shift;

	MPI_Status status;
	
	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	lyambda = (dt / (dh * dh)) * mu;
	max_eps = 0.0;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_nums);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	local_size = (N - 1) / proc_nums;
	
	lctl = (double *) malloc((local_size + 2) * sizeof(double));
	lptl = (double *) malloc((local_size + 2) * sizeof(double));

	/* init start condition */
	shift = local_size * proc_rank;
	for(i = 1; i <= local_size; i++){
		x = (shift + i) * dh;
		lptl[i] = start_condition(x);
	}

	for(k = 0; k < M; k++){
		t = k * dt;
		t_next = (k + 1) * dt;

		if (proc_rank == 0){
			lptl[0] = left_border_condition(t);
		} else if (proc_rank == proc_nums - 1) {
			lptl[local_size + 1] = right_border_condition(t);
		}

		/* update borders */
		if (proc_rank % 2 == 0){
			if (proc_rank < proc_nums - 1){
				/* send to right */
				MPI_Send(&lptl[local_size], 1,
					MPI_DOUBLE, proc_rank + 1, 0,
					MPI_COMM_WORLD);
				/* receive from right */
				MPI_Recv(&lptl[local_size + 1], 1,
					MPI_DOUBLE, proc_rank + 1, 0,
					MPI_COMM_WORLD, &status);
			}
			if (proc_rank > 0) {
				/* send to left */
				MPI_Send(&lptl[1], 1,
					MPI_DOUBLE, proc_rank - 1, 0,
					MPI_COMM_WORLD);
				/* receive from left */
				MPI_Recv(&lptl[0], 1,
					MPI_DOUBLE, proc_rank - 1, 0,
					MPI_COMM_WORLD, &status);
			}
		} else { /* proc_rank % 2 == 1 */
			/* receive from left */
			MPI_Recv(&lptl[0], 1,
				MPI_DOUBLE, proc_rank - 1, 0,
				MPI_COMM_WORLD, &status);
			/* send to left */
			MPI_Send(&lptl[1], 1,
				MPI_DOUBLE, proc_rank - 1, 0,
				MPI_COMM_WORLD);
			if (proc_rank < proc_nums - 1) {
				/* receive from right */
				MPI_Recv(&lptl[local_size + 1], 1,
					MPI_DOUBLE, proc_rank + 1, 0,
					MPI_COMM_WORLD, &status);
				/* send to right */
				MPI_Send(&lptl[local_size], 1,
					MPI_DOUBLE, proc_rank + 1, 0,
					MPI_COMM_WORLD);
			}
		}

		for(i = 1; i <= local_size; i++){
			x = (shift + i) * dh;
			lctl[i] = lptl[i] + lyambda * (lptl[i - 1] - 2 * lptl[i] + lptl[i + 1])
						+ dt * heat_source_function(x, t);
			max_eps = max(max_eps, fabs(lctl[i] - analytical_solution(x, t_next)));
		}

		tmp_ptr = lptl;
		lptl = lctl;
		lctl = tmp_ptr;
	}

	free(lptl);
	free(lctl);

	MPI_Reduce(&max_eps, &eps, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	return eps;
}	