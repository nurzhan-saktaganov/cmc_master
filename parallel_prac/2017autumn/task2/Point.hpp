#pragma once

#include <mpi.h>

class Point {
public:
    enum sort_way {x_way, y_way};
    static void set_sort_way(Point::sort_way sort_by);

    bool operator<(const Point &p) const;

    static MPI::Datatype datatype();

    float x, y;
    long long index;
private:
    static sort_way sort_by;
    static MPI::Datatype _datatype;
};
