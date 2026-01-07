#pragma once
#include "Simulation.h"

class Event;
using namespace std;
class StartIO : public Event{
public:
	StartIO(int theTime, ListItem *theProcess, Simulation *sim);
	void handleEvent();
};
