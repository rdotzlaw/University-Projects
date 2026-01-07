

#include "StartIO.h"
#include "CompleteIO.h"

void StartIO::handleEvent() {
	Process *p = dynamic_cast<Process*>(process);
	Event *e = nullptr; // event to add to the events queue
	if(p != nullptr) {

		// no timeout, will always finish

		cout << "Time	" << this->eventTime << ": Process " << p->getId() << " begins IO burst ";
		cout << "of " << p->getFront() << " units (will finish at Time: "<< eventTime + p->getFront() <<").\n";
		e = new CompleteIO(eventTime + p->getFront(), process, sim);
		p->setFront(0); // dequeue value in completeIO

		p->setCurrent(e);
		sim->addEvent(e);
		sim->addEventIO(process);

	}
}

StartIO::StartIO(int theTime, ListItem *theProcess, Simulation *sim) :
	Event(theTime, theProcess, sim) {}
