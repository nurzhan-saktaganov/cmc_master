#pragma once
#include <vector>
#include "Comparator.hpp"

class Schedule {
public:
    Schedule(const int n, const int filter = -1);
    bool check();
    int fragments();
    int comparators();
    int tacts();
    std::vector<Comparator>::iterator begin();
    std::vector<Comparator>::iterator end();
private:
    void comp_exchange(const int a, const int b);
    void knuth_sort(const int n);

//check schedule
    static void init_array(std::vector<int> &array, int bit_mask);
    void perform(std::vector<int> &array);

    std::vector<Comparator> _comparators;
    const int n;
    const int filter;
    int n_tacts; // for caching result
};
