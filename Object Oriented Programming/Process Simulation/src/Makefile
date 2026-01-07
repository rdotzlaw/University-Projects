

objects = Event.o Simulation.o Process.o Queue.o PQueue.o Node.o IntItem.o Arrival.o CompleteCPU.o CompleteIO.o Timeout.o Complete.o StartCPU.o StartIO.o Exit.o# .. etc .. put a list of your .o files here
# Simulation.o Process.o Node.o IntItem.o PQueue.o Queue.o Event.o Arrival.o StartCPU.o StartIO.o Timeout.o Complete.o CompleteCPU.o CompleteIO.o Exit.o
# this rule will build A2 as the executable from the object files
all: $(objects)
		clang++ --std=c++11 A2main.cpp -o A2 $(objects)

# this rule will create the test file
test: $(objects)
	clang++ --std=c++11 test.cpp -o test $(objects)

# this rule will build a .o file from a .cpp file. 
%.o: %.cpp
		clang++ -c $< -o $@ --std=c++11
clean:
	rm -f *.o test A2