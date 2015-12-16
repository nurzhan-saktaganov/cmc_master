#ifndef SWEEP_H
#define SWEEP_H

#include <stdlib.h>

/* Tridiagonal matrix algorithm: Ax = f */

void sweep(
	double *a, /* diagonal a, a[0] == 0 */
	double *b, /* diagonal b */
	double *c, /* diagonal c, c[size-1] == 0 */
	double *x,
	double *f,
	int size)
{
	double *alfa;
	double *beta;
	double tmp;
	int i;

	alfa = (double *) malloc((size + 1) * sizeof(double));
	beta = (double *) malloc((size + 1) * sizeof(double));

	alfa[0] = 0.0;
	beta[0] = 0.0;

	for(i = 0; i < size; i++){
		tmp = a[i] * alfa[i] + b[i];

		alfa[i + 1] = -c[i] / tmp;
		beta[i + 1] = (f[i] - a[i] * beta[i]) / tmp;
	}

	tmp = 0.0; /* tmp is used as prev_x*/
	for(i = size; i > 0; i--){
		x[i - 1] = alfa[i] * tmp + beta[i];
		tmp = x[i - 1];
	}

	free(beta);
	free(alfa);

	return;
}

#endif