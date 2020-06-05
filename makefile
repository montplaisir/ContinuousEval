
all: evalqueue

QueuePoint.o: QueuePoint.cpp QueuePoint.hpp
	g++ -c QueuePoint.cpp -o QueuePoint.o -fopenmp

Queue.o: Queue.cpp Queue.hpp
	g++ -c Queue.cpp -o Queue.o -fopenmp

evalqueue: QueuePoint.o Queue.o main.cpp
	g++ main.cpp Queue.o -o evalqueue -fopenmp

clean:
	rm -f *.o evalqueue
