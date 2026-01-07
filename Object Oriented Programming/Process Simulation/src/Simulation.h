#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include "Process.h"
#include "Arrival.h"

using namespace std;

class PQueue;
class Process;
class Arrival;
class Simulation {
	private:
		// you will need to add fields
		// including: Queues for CPU and IO, and priority queues for Events
		PQueue *processes; // for use with summary
		Queue *events;
		Queue *qCPU;
		Queue *qIO;
		//Process *CPU;
		//Process *IO;
		int cpuStep;
		int maxTime;
		int pId;
		ifstream * file;
		void step();


	public:
		//int globalTime;
		Simulation();

		// runSimulation -- start the simulation with the given filename.
		// Called by main.
		void runSimulation(char *fileName);

		// summary -- print a summary of all the processes, as shown in the
		// assignment.  Called by main.
		void summary();

		// you may need to add more methods
		int getMax() const;

		void read();

		bool cpuBusy();
		bool ioBusy();

		void addEvent(ListItem *i);
		void addEventCPU(ListItem *i);
		void addEventIO(ListItem *i);


	int ioTime();

	int cpuTime();

	ListItem *dqCPU();

	int cpuSteps();

	int cpuSteps(int n);

	ListItem *dqIO();


	void log(Process *pProcess);

	void setMax(int n);

	bool cpuBusy(int t);
};// class Simulation
