#pragma once
#include <vector>
#include "Comparator.hpp"

using namespace std;

class Schedule {
public:
    Schedule(const int n, const int filter = -1);
    bool check();
    int fragments();
    int comparators();
    int tacts();
    vector<Comparator>::iterator begin();
    vector<Comparator>::iterator end();
private:
    void comp_exchange(const int a, const int b);
    void knuth_sort(const int n);

//check schedule
    static void init_array(vector<int> &array, int bit_mask);
    void perform(vector<int> &array);

    vector<Comparator> _comparators;
    const int n;
    const int filter;
    int n_tacts; // for caching result
};
