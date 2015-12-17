#ifndef METHODS_H
#define METHODS_H

#include <stdlib.h> 
#include <math.h>
#include "sweep.h"
#include "util.h"

typedef struct _ret_type {
	double time;
	double eps;
} ret_type;

int verify(double *ctl, double *vtl, int n, double eps);

/*
mu * dt < dh^2
*/
ret_type euler_explicit_automodel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double (* k_func)(double /*u*/),
	double (* hp_u)(double /*u0*/, double /*u1*/), /* half point u */
	double dt,
	double dh,
	double epsilon,
	double min_dt);

ret_type euler_implicit_automodel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double (* k_func)(double /*u*/),
	double (* hp_u)(double /*u0*/, double /*u1*/), /* half point u */
	double dt,
	double dh,
	double epsilon,
	double min_dt);

ret_type euler_non_linear_automodel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double (* k_func)(double /*u*/),
	double (* hp_u)(double /*u0*/, double /*u1*/), /* half point u */
	double dt,
	double dh,
	double epsilon,
	double min_dt);


int verify(double *ctl, double *vtl, int n, double eps)
{
	int i;
	for(i = 1; i < n; i++){
		if (fabs(ctl[i] - vtl[i]) > eps){
			return 0;
		}
	}

	return 1;
}

ret_type euler_explicit_automodel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double (* k_func)(double /*u*/),
	double (* hp_u)(double /*u0*/, double /*u1*/), /* half point u */
	double dt,
	double dh,
	double epsilon,
	double min_dt)
{
	double x, t, t_hp; /* t_hp - time half point */
	unsigned int i, n;
	unsigned long long k;
	unsigned int flag;
	double lambda, lambda_half, dt_half;
	double eps;
	double *ctl; /* current time layer */
	double *ntl; /* next time layer */
	double *htl; /* halt time layer */
	double *vtl; /* verify time layer */
	ret_type result;

	n = (int) (1.0 / dh + 0.5); /* x-axe slices */
	/* correcting dh */
	dh = 1.0 / n;
	dt_half = dt / 2.0;
	lambda = dt / (dh * dh);
	lambda_half = dt_half / (dh * dh);

	ctl = (double *) malloc((n + 1) * sizeof(double));
	ntl = (double *) malloc((n + 1) * sizeof(double));
	htl = (double *) malloc((n + 1) * sizeof(double));
	vtl = (double *) malloc((n + 1) * sizeof(double));

	for (i = 0; i < n + 1; i++){
		x = i * dh;
		ctl[i] = start_condition(x);
	}

	k = 0;
	flag = 0;
	while (flag < 2 && dt > min_dt){
		t = k * dt;
		t_hp = (2 * k + 1) * dt / 2.0;
		//t_next = (k + 1) * dt;

		ctl[0] = left_border_condition(t);
		ctl[n] = right_border_condition(t);

		for (i = 1; i < n; i++){
			x = i * dh;
			ntl[i] = ctl[i] + 
				lambda * (
					k_func(hp_u(ctl[i + 1], ctl[i])) * (ctl[i + 1] - ctl[i]) +  
					k_func(hp_u(ctl[i], ctl[i - 1])) * (ctl[i] - ctl[i - 1])
				) + dt * heat_source_function(x, t);
		}

		/* calculating half point layer */
		for (i = 1; i < n; i++){
			x = i * dh;
			htl[i] = ctl[i] + 
				lambda_half * (
					k_func(hp_u(ctl[i + 1], ctl[i])) * (ctl[i + 1] - ctl[i]) +  
					k_func(hp_u(ctl[i], ctl[i - 1])) * (ctl[i] - ctl[i - 1])
				) + dt_half * heat_source_function(x, t);
		}

		htl[0] = left_border_condition(t_hp);
		htl[n] = right_border_condition(t_hp);

		/* calculating verify layer by using half point layer */
		for (i = 1; i < n; i++){
			x = i * dh;
			vtl[i] = htl[i] + 
				lambda_half * (
					k_func(hp_u(htl[i + 1], htl[i])) * (htl[i + 1] - htl[i]) +  
					k_func(hp_u(htl[i], htl[i - 1])) * (htl[i] - htl[i - 1])
				) + dt_half * heat_source_function(x, t_hp);
		}

		if (verify(ctl, vtl, n, epsilon)){
			swap_ptrs((void **) &ctl, (void **) &ntl);
			flag = 0;
			k++;
		} else {
			flag++;
			dt /= 2.0;
			dt_half /= 2.0;
			lambda /= 2.0;
			lambda_half /= 2.0;
			k *= 2;
		}
	}

	free(vtl);
	free(htl);
	free(ntl);
	free(ctl);

	eps = 0.0;
	for (i = 1; i < n; i++){
		eps = get_max(eps, fabs(ctl[i] - analytical_solution(i * dh, k * dt)));
	}

	result.time = k * dt;
	result.eps = eps;

	return result;
}

ret_type euler_implicit_automodel_test(
	double (* analytical_solution)(double /*x*/, double /*t*/),
	double (* start_condition)(double /*x*/),
	double (* left_border_condition)(double /*t*/),
	double (* right_border_condition)(double /*t*/),
	double (* heat_source_function)(double /*x*/, double /*t*/),
	double (* k_func)(double /*u*/),
	double (* hp_u)(double /*u0*/, double /*u1*/), /* half point u */
	double dt,
	double dh,
	double epsilon,
	double min_dt)
{
	double x, t, t_hp, t_next;
	unsigned int i, n;
	unsigned long long k;
	unsigned int flag;
	double lambda, lambda_half, dt_half;
	double eps;
	double *ctl; /* current time layer */
	double *ntl; /* next time layer */
	double *htl; /* halt time layer */
	double *vtl; /* verify time layer */
	double *a, *b, *c, *f;
	ret_type result;

	n = (int) (1.0 / dh + 0.5); /* x-axe slices */
	/* correcting dh */
	dh = 1.0 / n;
	dt_half = dt / 2.0;
	lambda = dh * dh / dt;
	lambda_half = dh * dh / dt_half;

	ctl = (double *) malloc((n + 1) * sizeof(double));
	ntl = (double *) malloc((n + 1) * sizeof(double));
	htl = (double *) malloc((n + 1) * sizeof(double));
	vtl = (double *) malloc((n + 1) * sizeof(double));

	/* We have n + 1 points, and n - 1 of them are inner */
	a = (double *) malloc((n - 1) * sizeof(double));
	b = (double *) malloc((n - 1) * sizeof(double));
	c = (double *) malloc((n - 1) * sizeof(double));
	f = (double *) malloc((n - 1) * sizeof(double));

	for (i = 0; i < n + 1; i++){
		x = i * dh;
		ctl[i] = start_condition(x);
	}

	k = 0;
	flag = 0;
	while (flag < 2 && dt > min_dt) {
		t = k * dt;
		t_hp = (2 * k + 1) * dt / 2.0;
		t_next = (k + 1) * dt;

		ctl[0] = left_border_condition(t);
		ctl[n] = right_border_condition(t);

		for (i = 1; i < n; i++){
			x = i * dh;
			a[i - 1] = -k_func(hp_u(ctl[i - 1], ctl[i]));
			b[i - 1] = lambda + k_func(hp_u(ctl[i - 1], ctl[i])) + 
				k_func(hp_u(ctl[i] ,ctl[i + 1]));
			c[i - 1] = -k_func(hp_u(ctl[i], ctl[i + 1]));
			f[i - 1] = lambda * (ctl[i] + dt * heat_source_function(x, t_next));
		}

		a[0] = 0.0;
		c[n - 2] = 0.0;

		f[0] += k_func(hp_u(ctl[0], ctl[1])) * left_border_condition(t_next);
		f[n - 2] += k_func(hp_u(ctl[n - 1], ctl[n])) * right_border_condition(t_next);

		sweep(a, b, c, ntl + 1, f, n - 1);

		/* calculating half point layer */
		for (i = 1; i < n; i++){
			x = i * dh;
			a[i - 1] = -k_func(hp_u(ctl[i - 1], ctl[i]));
			b[i - 1] = lambda_half + k_func(hp_u(ctl[i - 1], ctl[i])) + 
				k_func(hp_u(ctl[i], ctl[i + 1]));
			c[i - 1] = -k_func(hp_u(ctl[i], ctl[i + 1]));
			f[i - 1] = lambda_half * (ctl[i] + dt_half * heat_source_function(x, t_hp));
		}

		a[0] = 0.0;
		c[n - 2] = 0.0;

		f[0] += k_func(hp_u(ctl[0], ctl[1])) * left_border_condition(t_hp);
		f[n - 2] += k_func(hp_u(ctl[n - 1], ctl[n])) * right_border_condition(t_hp);

		sweep(a, b, c, htl + 1, f, n - 1);

		htl[0] = left_border_condition(t_hp);
		htl[n] = right_border_condition(t_hp);

		/* calculating verify layer by using half point layer */
		for (i = 1; i < n; i++){
			x = i * dh;
			a[i - 1] = -k_func(hp_u(htl[i - 1], htl[i]));
			b[i - 1] = lambda_half + k_func(hp_u(htl[i - 1], htl[i])) + 
				k_func(hp_u(htl[i],htl[i + 1]));
			c[i - 1] = -k_func(hp_u(htl[i], htl[i + 1]));
			f[i - 1] = lambda_half * (htl[i] + dt_half * heat_source_function(x, t_next));
		}

		a[0] = 0.0;
		c[n - 2] = 0.0;

		f[0] += k_func(hp_u(htl[0], htl[1])) * left_border_condition(t_next);
		f[n - 2] += k_func(hp_u(htl[n - 1], htl[n])) * right_border_condition(t_next);

		sweep(a, b, c, vtl + 1, f, n - 1);

		if (verify(ctl, vtl, n, epsilon)){
			swap_ptrs((void **) &ctl, (void **) &ntl);
			flag = 0;
			k++;
		} else {
			flag++;
			dt /= 2.0;
			dt_half /= 2.0;
			lambda = dh * dh / dt;
			lambda_half = dh * dh / dt_half;
			k *= 2;
		}
	}


	free(f);
	free(c);
	free(b);
	free(a);

	free(vtl);
	free(htl);
	free(ntl);
	free(ctl);

	eps = 0.0;
	for (i = 1; i < n; i++){
		eps = get_max(eps, fabs(ctl[i] - analytical_solution(i * dh, k * dt)));
	}

	result.time = k * dt;
	result.eps = eps;

	return result;
}

#endif