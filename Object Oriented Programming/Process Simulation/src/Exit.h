#pragma once
#include "Simulation.h"

class Event;
using namespace std;
class Exit : public Event{
public:
	Exit(int theTime, ListItem *theProcess, Simulation *sim);
	void handleEvent();
};


