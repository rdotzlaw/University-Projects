#pragma once

#include "CompleteCPU.h"
//#include "Simulation.h"
#include "Timeout.h"
#include <iostream>
#include "Event.h"
class ListItem;
using namespace std;
class StartCPU : public Event{
	public:
		StartCPU(int t, ListItem * p, Simulation * s);
		void handleEvent();
};
