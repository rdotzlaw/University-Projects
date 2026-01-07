
#pragma once

//#include "StartCPU.h"
//#include "Simulation.h"
//#include "ListItem.h"
#include "Event.h"
#include <iostream>
class ListItem;
class Event;
class StartCPU;
class Arrival : public Event {
	private:

	public:
		Arrival(int theTime, ListItem *theProcess, Simulation *sim);
		void handleEvent();
		//int compareTo(ListItem *other);
};
