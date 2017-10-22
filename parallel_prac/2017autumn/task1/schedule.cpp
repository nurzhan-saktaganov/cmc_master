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
void sort_schedule(std::vector<comparator>& schedule, int lo, int hi);
void oddeven_merge_schedule(std::vector<comparator>& schedule, int lo, int hi, int r);

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
	sort_schedule(schedule, 0, n - 1);
}

void sort_schedule(std::vector<comparator>& schedule, int lo, int hi)
{
	if (hi - lo < 1) return;
	
	int mid = lo + (hi - lo) / 2;
	sort_schedule(schedule, lo, mid);
	sort_schedule(schedule, mid + 1, hi);
	oddeven_merge_schedule(schedule, lo, hi, 1);
}

void oddeven_merge_schedule(std::vector<comparator>& schedule, int lo, int hi, int r)
{
	int step = r * 2;
	if (step >= hi - lo) {
		schedule.push_back(comparator(lo, lo + r));
		return;
	}
	
	oddeven_merge_schedule(schedule, lo, hi, step);
    oddeven_merge_schedule(schedule, lo + r, hi, step);
    
    for (int i = lo + r; i < hi - r; i += step) {
		schedule.push_back(comparator(i, i + r));
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
