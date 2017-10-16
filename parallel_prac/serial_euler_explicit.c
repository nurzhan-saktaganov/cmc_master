#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

/*
du/dt = mu * d^2u/dx^2 + f(x,t)
u(x, 0) - start_condition
u(0, x) - left_border_condition
u(1, x) - right_border_condition\
f(x, t) - heat_source_function
u(x, t) - analytical_solution
*/

double analytical_solution(double x, double t);
double start_condition(double x);
double left_border_condition(double t);
double right_border_condition(double t);
double heat_source_function(double x, double t);

/*
mu * dt < dh^2
*/
double euler_explicit_serial_test(
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
	double time_ser;
	double max_eps_ser;

	double mu, T;
	int M, N;

	mu = 1.0;
	T = 1.0;
	/* the stability condition: M >= 2 * mu* T * N^2 */
	M = 132098;
	N = 257;

	time_ser = omp_get_wtime();
	max_eps_ser = euler_explicit_serial_test(
		analytical_solution,
		start_condition,
		left_border_condition,
		right_border_condition,
		heat_source_function,
		mu, T, M, N);
	time_ser = omp_get_wtime() - time_ser;

	printf("x-axe slices: %d\n", M);
	printf("time slices: %d\n", N);
	printf("serial algorithm time: %lf\n", time_ser);
	printf("serial algorithm error: %lf\n", max_eps_ser);
		
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

double euler_explicit_serial_test(
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
	double *ptl; /* previous time layer */
	double *ctl; /* current time layer */
	double *tmp_ptr;
	double max_eps;

	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	lyambda = (dt / (dh * dh)) * mu;
	max_eps = 0.0;

	ptl = (double *) malloc((N + 1) * sizeof(double));
	ctl = (double *) malloc((N + 1) * sizeof(double));

	for(i = 0; i < N + 1; i++){
		x = i * dh;
		ptl[i] = start_condition(x);
	}

	for(k = 0; k < M; k++){
		t = k * dt;
		t_next = (k + 1) * dt;

		ptl[0] = left_border_condition(t);
		ptl[N] = right_border_condition(t);

		for(i = 1; i < N; i++){
			x = i * dh;
			ctl[i] = ptl[i] + lyambda * (ptl[i - 1] - 2 * ptl[i] + ptl[i + 1])
						+ dt * heat_source_function(x, t);
			max_eps = max(max_eps, fabs(ctl[i] - analytical_solution(x, t_next)));
		}

		tmp_ptr = ptl;
		ptl = ctl;
		ctl = tmp_ptr;
	}

	free(ctl);
	free(ptl);

	return max_eps;
}
