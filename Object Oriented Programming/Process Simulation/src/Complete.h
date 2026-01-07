#pragma once

#include "Event.h"
// this is a class to simplify the compareTo function

class Complete : public Event{
private:
public:
	Complete(int theTime, ListItem *theProcess, Simulation* sim);
	virtual void handleEvent() = 0;
	//int compareTo(ListItem *other);
};
