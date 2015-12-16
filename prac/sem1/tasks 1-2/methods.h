#ifndef METHODS_H
#define METHODS_H

#include <string.h>
#include <stdlib.h>
#include "tridiagonal.h"

/* task realization */
void explicit_method_1( \
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
	double alfa;

	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	alfa = (dt / (dh * dh)) * mu;

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

	/* calculate inner values */
	for(k = 0; k < M; k++)
		for(i = 1; i < N; i++){
			t = k * dt;
			x = i * dh;
			Y[k + 1][i] = Y[k][i] \
							+ alfa * (Y[k][i + 1] - 2 * Y[k][i] + Y[k][i - 1]) \
							+ dt * heat_source_function(x, t);
		}

	return;
}

/* task realization */
void implicit_method_1( \
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

	/* init 'tridiagonal' matrix */
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

/* task realization */
void explicit_method_2( \
						double **Y /* Y[t][x], Y[t][x] instead of Y[x][t] for better memory alignment */, \
						int M, /* time slices */ \
						int N, /* x-axe slices */ \
						double time /* T */, \
						double (* mu)(double /*x*/, double /*t*/), \
						double (* start_condition)(double /*x*/), \
						double (* left_border_condition)(double /*t*/), \
						double (* right_border_condition)(double /*t*/), \
						double (* heat_source_function)(double /*x*/, double /*t*/) \
					) 
{
	double dh, dt;
	double x, t;
	int i, k;
	double alfa;
	double x_l, x_r; /* x_l - x left, x_r - x right */

	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	alfa = dt / (dh * dh);

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

	/* calculate inner values */
	for(k = 0; k < M; k++) {
		for(i = 1; i < N; i++){
			t = k * dt;
			x = i * dh;
			x_l = dh / 2.0 * (2 * i - 1);
			x_r = dh / 2.0 * (2 * i + 1);
			Y[k + 1][i] = Y[k][i] \
							+ alfa * ( \
										mu(x_r, t) * Y[k][i + 1] \
										- ( mu(x_r, t) + mu(x_l, t)) * Y[k][i] \
										+ mu(x_l, t) * Y[k][i - 1] \
									) \
							+ dt * heat_source_function(x, t);
		}
	}

	return;
}

/* task realization */
void implicit_method_2( \
						double **Y /* Y[t][x], Y[t][x] instead of Y[x][t] for better memory alignment */, \
						int M, /* time slices */ \
						int N, /* x-axe slices */ \
						double time /* T */, \
						double (* mu)(double /*x*/, double /*t*/), \
						double (* start_condition)(double /*x*/), \
						double (* left_border_condition)(double /*t*/), \
						double (* right_border_condition)(double /*t*/), \
						double (* heat_source_function)(double /*x*/, double /*t*/) \
					) 
{
	double dh, dt;
	double x, t;
	int i, k;
	double alfa;
	/* for tridiagonal algorithm */
	double *a, *b, *c, *y, *f;
	double x_l, x_r; /*x left, x right */

	/* init variables */
	dh = 1.0 / N;
	dt = time / M;
	alfa = dh * dh / dt;

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

	for(k = 1; k < M + 1; k++){
		t = k * dt;

		for(i = 1; i < N ; i++){
			x = i * dh;
			x_l = dh / 2.0 * (2 * i - 1);
			x_r = dh / 2.0 * (2 * i + 1);

			a[i - 1] = -mu(x_l, t);
			b[i - 1] = alfa + mu(x_r, t) + mu(x_l, t);
			c[i - 1] = -mu(x_r, t);
			f[i - 1] = alfa * (Y[k - 1][i] + dt * heat_source_function(x, t));
		}

		a[0] = 0.0;
		c[N - 2] = 0.0;

		f[0] += mu(dh / 2.0, t) * Y[k][0] /* left_border_condition(t)*/;
		f[N - 2] += mu((2 * N - 1) * dh / 2.0, t) * Y[k][N]/*right_border_condition(t) */;

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

#endif