#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

double simpson_method(
	double (* func)(double /*x*/),
	double a,
	double b,
	int n)
{
	int i;
	double h;
	double integral_sum;
	/*x_{n}, x_{n + 1}, x_{n + 1/2}*/
	double x_curr, x_next, x_half; 

	h = (b - a) / n;

	integral_sum = 0.0;

#pragma omp parallel for \
private(x_curr, x_next, x_half) \
firstprivate(a, h) \
reduction(+: integral_sum)
	for(i = 0; i < n; i++){
		x_curr = a + i * h;
		x_next = a + (i + 1) * h;
		x_half = (x_curr + x_next) / 2.0;
		integral_sum += func(x_curr) + 4 * func(x_half) + func(x_next);
	}

	return integral_sum * h / 6.0;
}

double func1(double x)
{
	return pow(x, 3);
}

double check1(double a, double b)
{
	return (pow(b, 4) - pow(a, 4)) / 4.0;
}

double func2(double x)
{
	return x * x;
}

double check2(double a, double b)
{
	return (pow(b, 3) - pow(a, 3)) / 3.0;
}

double func3(double x)
{
	return cos(x);
}

double check3(double a, double b)
{
	return sin(b) - sin(a);
}

void test(
	double (* func)(double /*x*/),
	double (* check)(double /*a*/, double /*b*/),
	double a,
	double b,
	int n,
	char *func_text)
{
	double simpson, exact;

	simpson = simpson_method(func, a, b, n);
	exact = check(a, b);
	printf("------------------------\n");
	printf("%s simpson: %lf\n", func_text, simpson);
	printf("%s exact: %lf\n", func_text, exact);
	printf("absolute error = %lf\n", fabs(simpson - exact));
	if (fabs(exact) > 0.0) {
		printf("relative error = %lf\n", fabs(simpson - exact) / exact);
	}

	return;
}

int main(int argc, char **argv)
{
	double a, b;
	int n;

	a = -30;
	b = 30;
	n = 1000;

	printf("a = %lf\n", a);
	printf("b = %lf\n", b);
	printf("n = %d\n", n);
	printf("h = %lf\n", (b - a) / n);

	test(func1, check1, a, b, n, "x^3");
	test(func2, check2, a, b, n, "x^2");
	test(func3, check3, a, b, n, "cosx");

	return 0;
}