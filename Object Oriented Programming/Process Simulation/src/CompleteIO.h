#pragma once

#include "Simulation.h"
#include "Complete.h"
class Event;

class CompleteIO : public Complete {
public:
	CompleteIO(int theTime, ListItem *theProcess, Simulation *sim);

	void handleEvent();
};

