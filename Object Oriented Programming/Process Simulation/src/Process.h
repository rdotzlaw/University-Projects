
#pragma once

//#include "Simulation.h"
#include "ListItem.h"
#include <iostream>
/*
#include "Arrival.h"
#include "StartCPU.h"
#include "StartIO.h"
*/

using namespace std;

class Simulation;
class Event;
class Queue;
class Process : public ListItem {
	private:
		//int length;
		//int * list;
		int time;
		int arrivalTime;
		int waitTime;
		int exitTime;
		int id;
		Event *current;
		Queue *times;
		Simulation * sim;
	public:
		Process(Simulation* s, Queue *q, int t, int i);
		int getArrival() const;
		ListItem * dq();

		int compareTo(ListItem *other);


		void setCurrent(Event *e);

		int getFront();

	void setFront(int n);

	int getCurrentDone();

	int getId();

	bool hasTime();

	void addWait(int n);

	void setExit(int n);

	void out();

	Event *getCurrent();
};