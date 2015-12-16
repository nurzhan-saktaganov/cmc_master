#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <math.h>

double analytical_solution(double x, double t);
double start_condition(double x);
double left_border_condition(double t);
double right_border_condition(double t);
double heat_source_function(double x, double t);
double thermal_conductivity_coefficient(double u);

double arithmetic_mean(double u0, double u1);
double geometric_mean(double u0, double u1);
double harmonic_mean(double u0, double u1);


double analytical_solution(double x, double t)
{
	return exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/ ;
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
	/* f(x,t) = 8 u^2 * x(x-1) */
	return 8.0 * x * (x - 1) *
			exp(-2.0 * ( (x - 0.5) * (x - 0.5) + t ));
}

double thermal_conductivity_coefficient(double u)
{
	return u;
}

double arithmetic_mean(double u0, double u1)
{
	return (u0 + u1) / 2.0;
}

double geometric_mean(double u0, double u1)
{
	return sqrt(u0 * u1);
}

double harmonic_mean(double u0, double u1)
{
	return 2.0 / (1.0 / u0 + 1.0 / u1);
}

#endif