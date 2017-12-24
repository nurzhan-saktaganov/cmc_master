#include <mpi.h>
#include "Point.hpp"

Point::sort_way Point::sort_by = x_way;
MPI::Datatype Point::_datatype = MPI::DATATYPE_NULL;

void Point::set_sort_way(Point::sort_way sort_by)
{
    if (sort_by != x_way && sort_by != y_way) {
        throw sort_by;
    }
    Point::sort_by = sort_by;
}

void Point::switch_sort_way()
{
    switch (sort_by) {
        case x_way:
            sort_by = y_way;
            break;
        case y_way:
            sort_by = x_way;
            break;
    }
}

bool Point::operator<(const Point &p) const
{
    if (this == &p || index == p.index) return false;
    if (index != -1 && p.index != -1) {
        return sort_by == x_way ? x < p.x : y < p.y;
    }
    return p.index == -1;
}

MPI::Datatype Point::datatype()
{
    if (_datatype != MPI::DATATYPE_NULL) return _datatype;

    const int count = 3;
    const int block_lengths[] = {1, 1, 1};

    Point p;
    const MPI::Aint base = MPI::Get_address(&p);
    const MPI::Aint displacements[] = {
        MPI::Get_address(&p.x) - base,
        MPI::Get_address(&p.y) - base,
        MPI::Get_address(&p.index) - base,
    };

    const MPI::Datatype types[] = {MPI::FLOAT, MPI::FLOAT, MPI::LONG_LONG};

    _datatype = MPI::Datatype::Create_struct(count, block_lengths, displacements, types);

    return _datatype;
}
