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

// build schedule
void make_schedule(std::vector<comparator> &schedule, int n);
void sort_schedule(std::vector<comparator>& schedule, int lo, int hi, int max_line);
void oddeven_merge_schedule(std::vector<comparator>& schedule, int lo, int hi, int r, int max_line);

// print report
int count_parallel_steps(std::vector<comparator>& schedule, int n);
void print_report(std::vector<comparator> &schedule, int n);

// checking functions
void check_scheduler();
void check_schedule(std::vector<comparator>& schedule, int n);
void init_array(int *array, int n, int bit_mask);
bool is_sorted(int *array, int n);
void perform_schedule(std::vector<comparator>& schedule, int *array);

void show_usage()
{
    std::cout<<"Usage: ./schedule n\n"
            <<"\tn - fragments number"<<std::endl;
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

    int max_check_length = 24;
    // check_scheduler(max_check_length);

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

void print_report(std::vector<comparator> &schedule, int n)
{
    std::cout<<n<<" 0 0"<<"\n";
    for (comparator &c : schedule) {
        std::cout<<c.a<<" "<<c.b<<"\n";
    }

    std::cout<<schedule.size()<<"\n"
        <<count_parallel_steps(schedule, n)<<std::endl;
}

//
void check_scheduler(int max_length)
{
    for (int n = 1; n <= max_length; ++n) {
        std::cout<<"Check for size "<<n<<std::endl;
        std::vector<comparator> schedule;
        make_schedule(schedule, n);
        check_schedule(schedule, n);
    }
}

void check_schedule(std::vector<comparator>& schedule, int n)
{
    int up_to = 1 << n;
    auto array = new int[n];

    for (int bit_mask = 0; bit_mask < up_to; ++bit_mask){
        init_array(array, n, bit_mask);
        perform_schedule(schedule, array);
        if (!is_sorted(array, n)) {
            std::cerr<<"Got error for size "<<n<<std::endl;
            exit(0);
        }
    }
}

void init_array(int *array, int n, int bit_mask)
{
    for (int i = 0; i < n; i++) {
        array[i] = bit_mask & 1;
        bit_mask >>= 1;
    }
}

bool is_sorted(int *array, int n)
{
    for (int i = 0; i < n - 1; ++i){
        if (array[i] > array[i + 1]) return false;
    }
    return true;
}

void perform_schedule(std::vector<comparator>& schedule, int *array)
{
    for (comparator &c : schedule) {
        if (array[c.a] > array[c.b]) {
            std::swap(array[c.a], array[c.b]);
        }
    }
}
