CXX=mpicxx.mpich
CFLAGS=-Wall -std=c++98
EXECUTABLE=task3.exe
LD=

all: Comparator.o Schedule.o Point.o
	$(CXX) $(CFLAGS) -o $(EXECUTABLE) Comparator.o Schedule.o Point.o main.cpp $(LD)

Comparator.o:
	$(CXX) -c $(CFLAGS) -o Comparator.o Comparator.cpp

Schedule.o:
	$(CXX) -c $(CFLAGS) -o Schedule.o Schedule.cpp

Point.o:
	$(CXX) -c $(CFLAGS) -o Point.o Point.cpp

clean:
	rm -rf *.o

