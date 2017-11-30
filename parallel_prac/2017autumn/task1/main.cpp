#include <iostream>
#include <string>
#include <stdexcept>
#if __STDC_VERSION__ <= 201100L
    #include <cstdlib>
#endif

#include "Schedule.hpp"

using namespace std;

void print_report(Schedule &schedule);

void usage(const string &program)
{
    cout<<"Usage: "<<program<<" n [m]\n\t"
        <<"n - fragments numbers\n\t"
        <<"m - check up to length m"<<endl;
}

int main(int argc, char *argv[])
{
    string program(argv[0]);
    if (argc < 2 || argc > 3) {
        usage(program);
        return 0;
    }

    int n, m;

    try {
        n = atoi(argv[1]);
        m = argc == 3 ? atoi(argv[2]) : 0;
    } catch (invalid_argument) {
        usage(program);
        return 0;
    }

    if (n > 1) {
        Schedule schedule(n);
        print_report(schedule);
    }

    for (int i = 1; i <= m; ++i){
        cout<<"Check for size: "<<i<<endl;
        Schedule schedule(i);
        if (!schedule.check()) {
            cerr<<"Got error for size: "<<i<<endl;
        }
    }

    return 0;
}

void print_report(Schedule &schedule)
{
    cout<<schedule.fragments()<<" 0 0"<<"\n";
    for(vector<Comparator>::iterator it = schedule.begin(); it != schedule.end(); ++it){
        cout<<it->a<<" "<<it->b<<"\n";
    }

    cout<<schedule.comparators()<<"\n"
        <<schedule.steps()<<endl;
}
