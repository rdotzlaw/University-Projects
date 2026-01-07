#pragma once
#include "Complete.h"
//#include "ListItem.h"
//#include "Simulation.h"
#include "StartIO.h"
#include "Exit.h"
#include <iostream>

using namespace std;

class CompleteCPU : public Complete{
	public:

	CompleteCPU(int theTime, ListItem *theProcess, Simulation *sim);

	void handleEvent();
};

