

#include "Process.h"
#include "Simulation.h"
#include "PQueue.h"
#include "IntItem.h"
#include "Event.h"
#include "Node.h"

Process::Process(Simulation *s, Queue *q, int t, int i) : sim(s), times(q) {
	current = nullptr;
	arrivalTime = t;
	waitTime = 0;
	exitTime = -1;
	id = i;
	time = t;
	//length = len;
	//list = l + sizeof(int); // cut off 1st value

	//Event *e = new Arrival(0, this, sim);
	//s->addEvent(e);
	//current = e;

	// times[0] is the time it will take for the current event to finish executing once it starts
	// current->eventTime is when the event will start executing

	/*
	int max = s->getMax();
	int i = 1;
	while(i < len){
		if(l[i] > 0){ // cpu burst
			Event *event = new StartCPU(arrivalTime, this, sim, l[i]);
			events->enqueue(event);
		} else if (l[i] < 0){ // io burst
			Event *event = new StartIO(arrivalTime, this, sim, -1 * l[i]);
			events->enqueue(event)
		}

		i++;
	} */
}

void Process::setCurrent(Event * e){
	//cout << "--- Current of pid: " << id << " now set to: " << e << "\n";
	current = e;
	time = e->getTime() + getFront();
}

Event * Process::getCurrent(){
	return current;
}

int Process::getArrival() const {
	return arrivalTime;
}


// used to remove the front item from times
ListItem * Process::dq(){
	return times->dequeue();
}

void Process::setFront(int n){
	times->setFront(new IntItem(n));
}

// get the time value at the front of the times list
int Process::getFront(){
	IntItem *i = dynamic_cast<IntItem*>(times->getFront());
	if(i != nullptr)
		return i->get();
	return 0;
}

int Process::compareTo(ListItem *other) {
	Process *p = dynamic_cast<Process*>(other);
	if(p != nullptr){
		// if equal, return 0;
		// if this < other, return < 0
		// if this > other, return > 0
		return this->arrivalTime - p->arrivalTime;
	} else {
		return 0;
	}
}

int Process::getCurrentDone() { // time of the current process completing
	if(getFront() < sim->getMax()) {

		return current->getTime() + getFront();

	} else {

		return current->getTime() + sim->getMax();
	}
}

bool Process::hasTime(){
	return !times->isEmpty();
}

int Process::getId(){
	return id;
}

void Process::addWait(int n){
	waitTime += n;
}

void Process::setExit(int n){
	exitTime = n;
}

void Process::out(){
	cout << "	" << id << "	" << arrivalTime << "	" << exitTime << "	" << waitTime << "\n";
}
