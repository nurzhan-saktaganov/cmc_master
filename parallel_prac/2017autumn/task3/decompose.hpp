#pragma once

#include <algorithm>
#include "Point.hpp"

//struct represents that point with given index
//belongs to given domain
typedef struct _point_domain_t {
    long long index;
    int domain;
} point_domain_t;

/*
points - given array of points
points_domain - elements belonging
length - length of points array
k - wanted domains count
lowest_domain - lowest available domain number
*/
void decompose(
    Point *points,
    point_domain_t *points_domain,
    const int length,
    const int k,
    const int lowest_domain)
{
    if (k == 1) {
        for (int i = 0; i < length; ++i) {
            points_domain[i].domain = lowest_domain;
            points_domain[i].index = points[i].index;
        }
        return;
    }

    std::sort(points, points + length);

    const int k1 = (k + 1) / 2;
    const int k2 = k - k1;
    const int l1 = 1.0 * k1/k * length;
    const int l2 = length - l1;

    Point::switch_sort_way();
    decompose(points, points_domain, l1, k1, lowest_domain);
    decompose(points + l1, points_domain + l1, l2, k2, lowest_domain + k1);
    Point::switch_sort_way();
}
