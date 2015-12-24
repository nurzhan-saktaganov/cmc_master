#ifndef NEWTON_METHOD_H
#define NEWTON_METHOD_H

#include <math.h>
#include <string.h>
#include "util.h"
#include "gauss_method.h"

double euclidean_norm(double *vec, int size)
{
	double norm;
	int i;
	
	norm = 0.0;
	for (i = 0; i < size; i++) {
		norm += vec[i] * vec[i];
	}
	
	return sqrt(norm);	
}

double distance_in_norm(
	double *vec1,
	double *vec2,
	double (norm *)(double * /*v*/, int /*size*/),
	int size)
{
	double distance;
	double *difference;
	int i;
	
	difference = (double *) malloc(size * sizeof(double));
	for (i = 0; i < size; i++) {
		difference[i] = vec1[i] - vec2[i];
	}
	
	distance = norm(difference, size);
	
	free(difference);
	
	return distance;	
}

void add_vectors(double *a, double *b, double *c, int size)
{
	int i;
	for (i = 0; i < size; i++){
		c[i] = a[i] + b[i];
	}
	return;
}
	

void newton_method(
	/* TODO think about params */
	double (norm *)(double * /* vec */, int /* size */),
	double *x_in,
	double *x_out,
	int size)
{
	double *x_prev;
	double *delta;
	double *x_curr;
	double eps;
	//double **Jacobian;
	//double *f_minus; /*-f(x)*/
	
	x_prev = (double *) malloc(size * sizeof(double));
	delta = (double *) malloc(size * sizeof(double));
	x_curr = (double *) malloc(size * sizeof(double));
	
	memcpy(x_curr, x_in, size * sizeof(double));	
	
	do {
		swap_ptrs((void **) &x_prev, (void **) &x_curr);
		
		/* TODO how to get Jacobian in point x_prev */
		// add code here
		
		/* TODO how to get right part of system of linear equations
		   in point x_prev, e.g. f_minus
		*/
		// add code here
		
		
		gauss_method(Jacobian, delta, f_minus, size);

		add_vectors(x_prev, delta, x_curr, size);
				
	} while (distance_in_norm(x_curr, x_prev, norm, size) >= eps * norm(x_prev, size));
	

	memcpy(x_out, x_curr, size * sizeof(double));
	
	free(x_prev);
	free(delta);
	free(x_curr);	
	
	return;
}

#endif
