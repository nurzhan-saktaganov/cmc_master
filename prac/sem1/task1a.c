#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TIME_SLICES 200
#define X_AXE_SLICES 10
#define T 1.0

void print_matrix(double **A, int M, int N){
    int i, j;
    for(i = 0; i < M + 1; i++){
        for(j = 0; j < N + 1; j++)
            printf("%lf ", A[i][j]);
        printf("\n");
    }

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
    return - 4.0 * (x - 0.5) * (x - 0.5) \
                * exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/;
}

/* from test function */
double analytical_solution(double x, double t)
{
    return exp(-(x - 0.5) * (x - 0.5) - t) /* exp(-t)*/ ;
}

/* task realization */
void explicit_method( \
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
    int i, k;

    for(k = 0; k < M + 1; k++)
        for(i = 0; i < N + 1; i++){
            if (abs(Y[k][i] - U[k][i]) > max_eps)
                max_eps = abs(Y[k][i] - U[k][i]);
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

    explicit_method(Y, M, N, /*T=*/T, /*mu=*/1.0, \
                    start_condition, \
                    left_border_condition, \
                    right_border_condition, \
                    heat_source_function);

    fill_true_values(U, M, N, /*T=*/T, analytical_solution);

    max_eps = test_answer(Y, U, M, N);

    //printf("max eps: %lf\n", max_eps);
    print_matrix(Y, M, N);

    /* deallocation */
    for(i = 0; i < M + 1; i++){
        free(Y[i]);
        free(U[i]);
    }

    free(Y);
    free(U);

    return 0;
}