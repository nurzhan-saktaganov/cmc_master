#ifndef GAUSS_METHOD
#define GAUSS_METHOD

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

/*
Ax = b
*/
int gauss_method(double **A_in, double *x, double *b_in, int size)
{
	int i, j, k;
	double **A, *b;
	double coeff;
	int column_max_ind;

	
	for (k = 0; k < size; k++){
		if (A_in[k][k] == 0.0) {
			return -1;
		}
	}
	
	/* creating local copy of A_in */
	A = (double **) malloc(size * sizeof(double *));
	for (i = 0; i < size; i++) {
		A[i] = (double *) malloc(size * sizeof(double));
		memcpy(A[i], A_in[i], size * sizeof(double));
	}

	/* creating local copy of b_in */
	b = (double *) malloc(size * sizeof(double));
	memcpy(b, b_in, size * sizeof(double));

	/* forward step */
	for (j = 0; j < size; j++){
		/* finding current column max */
		column_max_ind = j;
		for (i = j + 1; i < size; i++){
			if (fabs(A[i][j]) > fabs(A[j][j])){
				column_max_ind = i;
			}
		}

		if (column_max_ind != j){
			/* swap rows */
			swap_ptrs((void **) &A[j], (void **) &A[column_max_ind]);
			swap_double(&b[j], &b[column_max_ind]);
		}

		for (i = j + 1; i < size; i++){
			coeff = A[i][j] / A[j][j];
			for (k = j; k < size; k++){
				A[i][k] -= coeff * A[j][k];
			}
			b[i] -= coeff * b[j];
		}
	}

	/* backward step */
	for (i = size - 1; i >= 0; i--){
		x[i] = b[i];
		for (j = i + 1; j < size; j++){
			x[i] -= A[i][j] * x[j];
		}
		x[i] /= A[i][i];
	}

	free(b);
	for (i = 0; i < size; i++) {
		free(A[i]);
	}
	free(A);

	return 0;
}

#endif