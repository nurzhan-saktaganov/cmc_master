#include <vector>
#include <iostream>
#include <cmath>

#include "Comparator.hpp"
#include "Schedule.hpp"

#if __STDC_VERSION__ > 201100L
#include <algorithm>
#else
template <class ForwardIterator>
bool is_sorted(ForwardIterator first, ForwardIterator last)
{
  if (first==last) return true;
  ForwardIterator next = first;
  while (++next!=last) {
    if (*next<*first) return false;
    ++first;
  }
  return true;
}
#endif


using namespace std;

Schedule::Schedule(const int n, const int filter) : n(n), filter(filter), n_tacts(-1)
{
    knuth_sort(n);
}

void Schedule::knuth_sort(const int n)
{
    int t = ceil(log2(n));
    int p = 1 << (t - 1);

    while (p > 0) {
        int q = 1 << (t - 1);
        int r = 0;
        int d = p;
        while (d > 0) {
            for (int i = 0; i < n - d; ++i){
                if ((i & p) == r) comp_exchange(i, i + d);
            }
            d = q - p;
            q /= 2;
            r = p;
        }
        p /= 2;
    }
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

int Schedule::tacts()
{
    if (n_tacts != -1) return n_tacts;

    for (int track = 0; track < n; track++){
        int current_tacts = 0;
        int current_track = track;

        vector<Comparator>::reverse_iterator rit = _comparators.rbegin();

        for(; rit != _comparators.rend(); ++rit){
            if (!rit->contains(current_track)) continue;
            current_track = rit->get_pair(current_track);
            current_tacts++;
        }

        if (current_tacts > n_tacts) n_tacts = current_tacts;
    }

    return n_tacts;
}

vector<Comparator>::iterator Schedule::begin()
{
    return _comparators.begin();
}

vector<Comparator>::iterator Schedule::end()
{
    return _comparators.end();
}

void Schedule::comp_exchange(const int a, const int b)
{
    if (b >= n) return;
    if (filter >= 0 && filter != a && filter != b) return;

    _comparators.push_back(Comparator(a, b));
}

void Schedule::init_array(vector<int> &array, int bit_mask)
{
    for (size_t i = 0; i < array.size(); ++i){
        array[i] = bit_mask & 1;
        bit_mask >>= 1;
    }
}

void Schedule::perform(vector<int> &array)
{
    vector<Comparator>::iterator it = _comparators.begin();
    for(; it != _comparators.end(); ++it){
        if (array[it->a] > array[it->b]) swap(array[it->a], array[it->b]);
    }
}
