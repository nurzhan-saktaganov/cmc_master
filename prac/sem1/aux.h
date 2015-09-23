#ifndef AUX_H
#define AUX_H

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

#endif