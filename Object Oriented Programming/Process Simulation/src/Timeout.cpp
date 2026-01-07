
#include "Timeout.h"
#include "Simulation.h"
#include "StartCPU.h"
#include "ListItem.h"
Timeout::Timeout(int theTime, ListItem *theProcess, Simulation *sim) :
	Complete(theTime, theProcess, sim) {}

void Timeout::handleEvent() {
	// event runtime already reduced from startCPU
	// Queue startCPU event for after everything

	Process *p = dynamic_cast<Process *>(process);
	if(p != nullptr) {
		cout << "Time	" << this->eventTime << ": Process " << p->getId() << " has timed-out, moving process to end of queue.\n";
		// queue startCPU event at end of event queue, and move process to end of cpuQueue
		Event *i = new StartCPU(sim->cpuTime(), process, sim);
		p->addWait(sim->cpuTime() - eventTime);
		p->setCurrent(i);
		sim->addEventCPU(sim->dqCPU());
		sim->addEvent(i);

	} else {
		//cout << "ListItem in qCPU wasn't a process\n";
	}
}