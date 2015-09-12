#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M 100
#define N 10
#define T 1.0

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
                        int time_slices_count, /* M */ \
                        int x_axe_slices_count, /* N */ \
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
    dh = 1.0 / (x_axe_slices_count - 1);
    dt = time / (time_slices_count - 1);
    alfa = (dt / (dh * dh)) * mu;

    printf("alfa = %lf\n", alfa);

    /* init start condition */
    for(i = 0; i < x_axe_slices_count; i++){
        x = i * dh;
        Y[0][i] = start_condition(x);
    }

    /* init border conditions */
    for(k = 0; k < time_slices_count; k++){
        t = k * dt;
        Y[k][0] = left_border_condition(t);
        Y[k][x_axe_slices_count - 1] = right_border_condition(t);
    }

    /* calculate inner values */
    for(k = 0; k < time_slices_count - 1; k++)
        for(i = 1; i < x_axe_slices_count - 1; i++){
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
                        int time_slices_count, \
                        int x_axe_slices_count, \
                        double time, \
                        double (* analytical_solution)(double /*x*/, double /*t*/)\
                    )
{
    int i, k;
    double x, t;
    double dh, dt;

    dt = time / (time_slices_count - 1);
    dh = 1.0 / (x_axe_slices_count - 1);

    for( k = 0; k < time_slices_count; k++)
        for( i = 0; i < x_axe_slices_count; i++){
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
                    int time_slices_count, \
                    int x_axe_slices_count \
                )
{
    double max_eps = 0.0;
    int i, k;

    for(k = 0; k < time_slices_count; k++)
        for(i = 0; i < x_axe_slices_count; i++){
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
    int time_slices_count = M + 1;
    int x_axe_slices_count = N + 1;

    /* allocate memory */ 
    Y = (double **) calloc(time_slices_count, sizeof(double *));
    for(i = 0; i < time_slices_count; i++){
        Y[i] = (double *) calloc(x_axe_slices_count, sizeof(double));
    }


    U = (double **) calloc(time_slices_count, sizeof(double *));
    for(i = 0; i < time_slices_count; i++){
        U[i] = (double *) calloc(x_axe_slices_count, sizeof(double));
    }

    explicit_method(Y, time_slices_count, x_axe_slices_count, /*T=*/T, /*mu=*/1.0, \
                    start_condition, \
                    left_border_condition, \
                    right_border_condition, \
                    heat_source_function);

    fill_true_values(U, time_slices_count, x_axe_slices_count, /*T=*/T, analytical_solution);

    max_eps = test_answer(Y, U, time_slices_count, x_axe_slices_count);

    printf("max eps: %lf\n", max_eps);


    /* deallocation */
    for(i = 0; i < time_slices_count; i++){
        free(Y[i]);
        free(U[i]);
    }

    free(Y);
    free(U);

    return 0;
}