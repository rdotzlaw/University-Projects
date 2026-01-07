#pragma once
#include "Complete.h"
#include <iostream>
using namespace std;

class Timeout : public Complete{

public:
	Timeout(int theTime, ListItem *theProcess, Simulation *sim);

	void handleEvent();
};

