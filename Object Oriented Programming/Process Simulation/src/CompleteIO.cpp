
#include "CompleteIO.h"
#include "StartCPU.h"

void CompleteIO::handleEvent() {
	// either start new cpu burst, or exit
	Process *p = dynamic_cast<Process*>(process);
	if(p != nullptr){
		p->dq(); // dequeue used io burst
		sim->dqIO(); //remove process from io
		Event *e = nullptr;
		if(p->hasTime()) { //if still more cpu/io bursts
			//start cpu
			if(sim->cpuBusy()){
				cout << "Time	" << this->eventTime << ": Process " << p->getId()
					<< " finished IO burst, CPU is busy.\n";
				e = new StartCPU(sim->cpuTime(), process, sim);
				p->addWait(sim->cpuTime() - eventTime); // add wait time to process
			} else {
				cout << "Time	" << this->eventTime << ": Process " << p->getId()
					 << " finished IO burst, CPU is free.\n";
				e = new StartCPU(eventTime, process, sim); // create startIO for current time
			}
			sim->addEventCPU(process); // add process to cpuQueue
		} else { // exit process
			// exit
			cout << "Time	" << this->eventTime << ": Process " << p->getId() << " finished final IO burst, exiting.\n";
			e = new Exit(eventTime, process, sim); // will execute right away
		}

		p->setCurrent(e); // set current in p
		sim->addEvent(e); // add event at proper time in events
	}
}

CompleteIO::CompleteIO(int theTime, ListItem *theProcess, Simulation *sim) :
		Complete(theTime, theProcess, sim) {}