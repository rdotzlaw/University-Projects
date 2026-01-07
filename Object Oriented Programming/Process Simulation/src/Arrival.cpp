
#include "Arrival.h"

#include "StartCPU.h"

void Arrival::handleEvent() {

	Process *p = dynamic_cast<Process *>(process);
	Event *i = nullptr;
	if(p != nullptr){
		sim->log(p);
		if(sim->cpuBusy()){ //sim->cpuBusy()
			i = new StartCPU(sim->cpuTime(), process, sim); // place at end of events queue in sim
			cout << "Time	" << this->eventTime << ": Process " << p->getId() << " arrives in system: CPU is busy (waiting for execution at Time: " << sim->cpuTime() <<").\n";
			// add wait time to p
			p->addWait(sim->cpuTime() - eventTime);
		} else {
			cout << "Time	" << this->eventTime << ": Process " << p->getId() << " arrives in system: CPU is free (process begins execution).\n";
			i = new StartCPU(eventTime, process, sim); // place at current time
			// no wait time
		}
		p->setCurrent(i); // set current in p
		sim->addEvent(i); // add event at proper time in events
		sim->addEventCPU(process); // put the process into the cpuQueue

	}

	/*
	if(sim->cpuBusy()){ // cpu busy
		sim->addEventCPU(process); // put process onto waiting list
	} else {
		Process *p = dynamic_cast<Process *>(process);
		if(p != nullptr)
			sim->addEvent(new StartCPU(p->getTime(), process, sim));
	}*/

	//read next line of file, creating the next arrival event
	sim->read();

}

Arrival::Arrival(int theTime, ListItem *theProcess, Simulation *sim) :
	Event(theTime, theProcess, sim) {}

	/*
int Arrival::compareTo(ListItem *other) {
	Arrival *a = dynamic_cast<Arrival*>(other);
	if(a != nullptr){
		// both arrivals
	}
}
*/