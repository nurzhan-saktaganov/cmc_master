#pragma once

class Comparator {
public:
    Comparator(const int a = -1, const int b = -1) : a(a), b(b){}
    int get_pair(const int x) const;
    int a, b;
};
