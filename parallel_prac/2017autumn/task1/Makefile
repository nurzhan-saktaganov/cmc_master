CXX=g++
CFLAGS=-Wall -std=c++11
EXECUTABLE=task1

all: Comparator.o Schedule.o
	$(CXX) $(CFLAGS) -o $(EXECUTABLE) Comparator.o Schedule.o main.cpp

Comparator.o:
	$(CXX) -c $(CFLAGS) -o Comparator.o Comparator.cpp

Schedule.o:
	$(CXX) -c $(CFLAGS) -o Schedule.o Schedule.cpp

clean:
	rm -rf *.o
