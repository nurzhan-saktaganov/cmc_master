#include "Comparator.hpp"

int Comparator::get_pair(const int x) const
{
    if (x != a && x != b) throw x;
    return x == b ? a : b;
}

bool Comparator::contains(const int x) const
{
    return x == a || x == b;
}
