
#include "StartCPU.h"

StartCPU::StartCPU(int t, ListItem *p, Simulation *s) :
	Event(t, p, s) {}


void StartCPU::handleEvent() {
	Process *p = dynamic_cast<Process *>(process);
	Event *e = nullptr;
	if(p != nullptr) { // most likely redundant, but whatever
		if (p->getFront() > sim->getMax()) { // the execution time for the current CPUBurst > max time

			int i = p->getFront();
			//cout << p->getFront();
			p->setFront(i - sim->getMax()); // sets front value in times list to -= sim->getMax()
			//cout << " : " << p->getFront() << "\n";
			sim->cpuSteps(sim->getMax()); // increase cpuStep by maxTime
			cout << "Time	" << this->eventTime << ": Process " << p->getId();
			cout << " begins CPU burst "<<"of " << p->getFront()<< " (will timeout; needs " << i << " units total).\n";

			// schedule timeout event for when cpu times out eventTime + p->front()

			e = new Timeout(eventTime + sim->getMax(), process, sim); // eventTime + sim->getMax() is when the cpu times out
			//sim->dqCPU();

		} else { // not a timeout
			// schedule completeCPU with eventTime at the time of completion
			// schedule at eventTime + p->getFront()
			// Reminder: eventTime is when the startCPU event takes place

			cout << "Time	" << this->eventTime << ": Process " << p->getId() << " begins CPU burst ";
			cout << "of " << p->getFront() << " units (will finish at Time: "<< eventTime + p->getFront() <<").\n";
			sim->cpuSteps(p->getFront());
			e = new CompleteCPU(eventTime + p->getFront(), process, sim);
			p->setFront(0); // dequeue value in completeCPU
		}
		p->setCurrent(e); // set current in p
		sim->addEvent(e); // add event at proper time in events


	}

}

