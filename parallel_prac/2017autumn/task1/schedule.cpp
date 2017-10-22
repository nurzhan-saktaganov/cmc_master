#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class comparator {
public:
    comparator() {}
    comparator(int a, int b) : a(a), b(b){}
    int a, b;
};

void make_schedule(std::vector<comparator> &schedule, int n);
void sort_schedule(std::vector<comparator>& schedule, int lo, int hi, int max_line);
void oddeven_merge_schedule(std::vector<comparator>& schedule, int lo, int hi, int r, int max_line);

int count_parallel_steps(std::vector<comparator>& schedule, int n);
void print_report(std::vector<comparator> &schedule, int n);

void show_usage()
{
    std::cout<<"Usage: ./schedule n\n"
            <<"\tn - fragments number"<<std::endl;
}

void print_report(std::vector<comparator> &schedule, int n)
{
    std::cout<<n<<" 0 0"<<"\n";
    for (comparator &c : schedule) {
        std::cout<<c.a<<" "<<c.b<<"\n";
    }

    std::cout<<schedule.size()<<"\n"
        <<count_parallel_steps(schedule, n)<<std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        show_usage();
        return 0;
    }

    int n;

    try {
        n = std::atoi(argv[1]);
    } catch (std::invalid_argument) {
        show_usage();
        return 0;
    }

    std::vector<comparator> schedule;
    make_schedule(schedule, n);
    print_report(schedule, n);

    return 0;
}

void make_schedule(std::vector<comparator> &schedule, int n)
{
    //next power of 2
    int r = 1;
    while (r < n) r *= 2;
    sort_schedule(schedule, 0, r - 1, n - 1);
}

void sort_schedule(std::vector<comparator>& schedule, int lo, int hi, int max_line)
{
    if (hi - lo < 1) return;

    int mid = lo + (hi - lo) / 2;
    sort_schedule(schedule, lo, mid, max_line);
    sort_schedule(schedule, mid + 1, hi, max_line);
    oddeven_merge_schedule(schedule, lo, hi, 1, max_line);
}

void oddeven_merge_schedule(std::vector<comparator>& schedule, int lo, int hi, int r, int max_line)
{
    int step = r * 2;
    if (step >= hi - lo) {
        if (lo + r <= max_line) schedule.push_back(comparator(lo, lo + r));
        return;
    }

    oddeven_merge_schedule(schedule, lo, hi, step, max_line);
    oddeven_merge_schedule(schedule, lo + r, hi, step, max_line);
    
    for (int i = lo + r; i < hi - r; i += step) {
        if (i + r <= max_line) schedule.push_back(comparator(i, i + r));
    }
}

int count_parallel_steps(std::vector<comparator>& schedule, int n)
{
    std::vector<int> linear_steps(n);

    for (comparator &c : schedule) {
        ++linear_steps[c.a];
        ++linear_steps[c.b];
    }

    return *(std::max_element(linear_steps.begin(), linear_steps.end()));
}
