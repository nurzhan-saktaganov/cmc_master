CXX=mpicxx.mpich
CFLAGS=-Wall -std=c++98
EXECUTABLE=task2
LD=-fopenmp -lpthread

all: Comparator.o Schedule.o Point.o
	$(CXX) $(CFLAGS) -o $(EXECUTABLE) Comparator.o Schedule.o Point.o main.cpp $(LD)

Comparator.o:
	$(CXX) -c $(CFLAGS) -o Comparator.o Comparator.cpp

Schedule.o:
	$(CXX) -c $(CFLAGS) -o Schedule.o Schedule.cpp

Point.o:
	$(CXX) -c $(CFLAGS) -o Point.o Point.cpp

testdhsort:
	g++ -Wall test_dhsort.cpp -o testdhsort $(LD)

clean:
	rm -rf *.o testdhsort
