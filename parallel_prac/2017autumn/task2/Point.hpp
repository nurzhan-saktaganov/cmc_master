#pragma once

class Point {
public:
    enum sort_way {x_way, y_way};
    static void set_sort_way(Point::sort_way sort_by);

    bool operator<(const Point &p) const;

    float x, y;
    int index;
private:
    static sort_way sort_by;
};
