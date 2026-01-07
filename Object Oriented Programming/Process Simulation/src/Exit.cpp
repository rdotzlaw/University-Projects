
#include "Exit.h"
#include "Process.h"

void Exit::handleEvent() {
	// process is in neither cpu queue or io queue
	Process *p = dynamic_cast<Process *>(process);
	if(p != nullptr){
		p->setExit(eventTime);
		cout << "Time	" << this->eventTime << ": Process " << p->getId()
			 << " finished all events, ending execution.\n";
		//sim->log(p);
	}
}

Exit::Exit(int theTime, ListItem *theProcess, Simulation *sim) :
	Event(theTime, theProcess, sim) {}