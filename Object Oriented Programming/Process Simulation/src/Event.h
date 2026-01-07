#pragma once
#include "ListItem.h"
//#include "Process.h"


class Simulation;
class Process;
class Arrival;
class ListItem;

class Event: public ListItem {
private:
	//int compareHelper(ListItem *pItem); // not really needed, keep in case change implementation
protected:
	Simulation *sim;
	int eventTime;



	ListItem *process;
public:
	// constructor, with pointer to the process that is being handled, and the simulation.
	Event(int theTime, ListItem *theProcess, Simulation* sim);

	// pure virtual method - to handle the current event when it is removed from the queue.
	virtual void handleEvent() = 0;
	int getTime() const;
	ListItem* getProcess();
	// compareTo - used to order Events.
	// advanced stuff shifted to subclasses
	virtual int compareTo(ListItem *other);




};// class Event
