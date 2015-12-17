#ifndef UTIL_H
#define UTIL_H

void swap_ptrs(void **ptr1, void **ptr2)
{
	void *tmp_ptr = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp_ptr;
	return;
}

void swap_double(double *a, double *b)
{
	double tmp = *a;
	*a = *b;
	*b = tmp;
	return;
}

double get_max(double a, double b)
{
	return a > b ? a : b;
}

#endif