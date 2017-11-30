#include <algorithm>
#include <vector>

#include "Comparator.hpp"
#include "Schedule.hpp"

using namespace std;

Schedule::Schedule(const int n) : n(n), _steps(-1)
{
    //int hi = n > 2 ? multiple_of_4(n) : n;
    int hi = next_power_of_2(n);
    sort_schedule(0, hi - 1);
}

bool Schedule::check()
{
    vector<int> array(n);
    const int up_to = 1 << n;

    for (int bit_mask = 0; bit_mask < up_to; ++bit_mask) {
        init_array(array, bit_mask);
        perform(array);
        if (!is_sorted(array.begin(), array.end())) {
            return false;
        }
    }

    return true;
}

int Schedule::fragments()
{
    return n;
}

int Schedule::comparators()
{
    return _comparators.size();
}

int Schedule::steps()
{
    if (_steps != -1) return _steps;

    vector<int> linear_steps(n);

    for (Comparator &c : _comparators) {
        ++linear_steps[c.a];
        ++linear_steps[c.b];
    }

    return _steps = *(max_element(linear_steps.begin(), linear_steps.end()));
}

vector<Comparator>::iterator Schedule::begin()
{
    return _comparators.begin();
}

vector<Comparator>::iterator Schedule::end()
{
    return _comparators.end();
}

int Schedule::next_power_of_2(const int n)
{
    int power_of_2 = 1;
    while (power_of_2 < n) power_of_2 *= 2;
    return power_of_2;
}

int Schedule::multiple_of_4(const int n)
{
    return n + (4 - (n - 1) % 4 - 1);
}

void Schedule::comp_exchange(const int a, const int b)
{
    if (b < n) _comparators.push_back(Comparator(a, b));
}

void Schedule::sort_schedule(const int lo, const int hi)
{
    if (hi - lo < 1) return;

    const int mid = lo + (hi - lo) / 2;
    sort_schedule(lo, mid);
    sort_schedule(mid + 1, hi);
    merge_schedule(lo, hi, 1);
}

void Schedule::merge_schedule(const int lo, const int hi, const int step)
{
    if (hi - lo <= 2 * step) return comp_exchange(lo, lo + step);

    merge_schedule(lo, hi, 2 * step);
    merge_schedule(lo + step, hi, 2 * step);

    for (int i = lo + step; i < hi - step; i += 2 * step) {
        comp_exchange(i, i + step);
    }
}

void Schedule::init_array(vector<int> &array, int bit_mask)
{
    for (int &e: array) {
        e = bit_mask & 1;
        bit_mask >>= 1;
    }
}

void Schedule::perform(vector<int> &array)
{
    for (Comparator &c : _comparators) {
        if (array[c.a] > array[c.b]) swap(array[c.a], array[c.b]);
    }
}
