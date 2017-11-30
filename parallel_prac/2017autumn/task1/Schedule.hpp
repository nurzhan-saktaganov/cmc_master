#pragma once
#include <vector>
#include "Comparator.hpp"

using namespace std;

class Schedule {
public:
    Schedule(const int n);
    bool check();
    int fragments();
    int comparators();
    int steps();
    vector<Comparator>::iterator begin();
    vector<Comparator>::iterator end();
private:
//utils
    static int next_power_of_2(const int n);
    static int multiple_of_4(const int n);
//build schedule
    void comp_exchange(const int a, const int b);
    void sort_schedule(const int lo, const int hi);
    void merge_schedule(const int lo, const int hi, const int step);

//check schedule
    static void init_array(vector<int> &array, int bit_mask);
    void perform(vector<int> &array);

    vector<Comparator> _comparators;
    const int n;
    int _steps;
};
