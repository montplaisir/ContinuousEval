
all: evalqueue

CXXFLAGS = -O2 -Wall -Wextra

QueuePoint.o: QueuePoint.cpp QueuePoint.hpp
	g++ $(CXXFLAGS) -c QueuePoint.cpp -o QueuePoint.o -fopenmp

Queue.o: Queue.cpp Queue.hpp
	g++ $(CXXFLAGS) -c Queue.cpp -o Queue.o -fopenmp

evalqueue: QueuePoint.o Queue.o main.cpp
	g++ $(CXXFLAGS) main.cpp Queue.o -o evalqueue -fopenmp

clean:
	rm -f *.o evalqueue
