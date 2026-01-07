
#include "CompleteCPU.h"


void CompleteCPU::handleEvent() {
	// either process will have more stuff in times - a startIO will be scheduled
	// or times will be empty, an exit event will be scheduled

	Process *p = dynamic_cast<Process*>(process);
	if(p != nullptr){
		p->dq(); // dequeue used cpu burst
		sim->dqCPU(); // remove process from cpu queue
		Event *e = nullptr;
		//Event *e = nullptr;
		if(p->hasTime()){
			// startIO
			if(sim->ioBusy()){ //io busy
				cout << "Time	" << this->eventTime << ": Process " << p->getId()
					 << " finished CPU burst, IO is busy.\n";
				e = new StartIO(sim->ioTime(), process, sim);
				p->addWait(sim->ioTime() - eventTime); // add wait time to process
			} else { // io free
				cout << "Time	" << this->eventTime << ": Process " << p->getId()
					 << " finished CPU burst, IO is free.\n";
				e = new StartIO(eventTime, process, sim); // create startIO for current time
			}
			sim->addEventIO(process); // add the process to the ioQueue
		} else {
			// exit
			cout << "Time	" << this->eventTime << ": Process " << p->getId() << " finished final CPU burst, exiting.\n";
			e = new Exit(eventTime, process, sim); // will execute right away
		}
		p->setCurrent(e); // set current in p
		sim->addEvent(e); // add event at proper time in events

	}
}

CompleteCPU::CompleteCPU(int theTime, ListItem *theProcess, Simulation *sim) :
	Complete(theTime, theProcess, sim) {

}