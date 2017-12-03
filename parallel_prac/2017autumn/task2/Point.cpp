#include "Point.hpp"

Point::sort_way Point::sort_by = x_way;

void Point::set_sort_way(Point::sort_way sort_by)
{
    if (sort_by != x_way && sort_by != y_way) {
        throw sort_by;
    }
    Point::sort_by = sort_by;
}

bool Point::operator<(const Point &p) const
{
    if (this == &p || index == p.index) return false;
    if (index != -1 && p.index != -1) {
        return sort_by == x_way ? x < p.x : y < p.y;
    }
    return p.index == -1;
}
