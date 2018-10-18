
all: evalqueue

Queue.o: Queue.cpp Queue.hpp
	g++ -c Queue.cpp -o Queue.o -fopenmp

evalqueue: Queue.o main.cpp
	g++ main.cpp Queue.o -o evalqueue -fopenmp

clean:
	rm -f *.o evalqueue
