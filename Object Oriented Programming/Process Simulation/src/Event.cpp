
#include "Event.h"


#include "Process.h"
#include "Complete.h"
#include "Arrival.h"

// for dynamic casting in compareTo (not needed now, check subclasses for implementation)
//#include "Arrival.h"
//#include "CompleteCPU.h"
//#include "CompleteIO.h"
//#include "Timeout.h"

/**** Event implementation */
Event::Event(int theTime, ListItem *theProcess, Simulation *theSim)
	 :eventTime(theTime), process(theProcess), sim (theSim) {}


	 /* shifted to subclasses
int Event::compareTo(ListItem *other){


	// order based on event time
	// if event time is equal
	//		arrivals happen before all events
	//		complete & timeouts happen before starts
	//		then order remaining events by their process arrival time
	// return <0 for this < other
	// return >0 for this > other
	// return =0 for this = other

	Event *e = dynamic_cast<Event*>(other);
	if(e != nullptr){
		if(this->eventTime == e->getTime()){
			// see if one is an arrival
			Arrival *a1 = dynamic_cast<Arrival*>(this);
			Arrival *a2 = dynamic_cast<Arrival*>(other);
			if(a1 != nullptr && a2 != nullptr){
				// helper call
				return compareHelper(a2);

			} else if(a2 != nullptr){
				return 1; // a2 arrival, a1 not, a2 < a1
			} else if(a1 != nullptr){ //
				return -1; // a1 arrival, a2 not, a1 < a2
			} else { // neither are arrivals
				// check if one is a complete event (completeCPU, completeIO, timeout)
				// all the complete events are equivalent
				CompleteCPU * c1 = dynamic_cast<CompleteCPU *>(this);
				CompleteCPU * c2 = dynamic_cast<CompleteCPU *>(other);

				CompleteIO * i1 = dynamic_cast<CompleteIO *>(this);
				CompleteIO * i2 = dynamic_cast<CompleteIO *>(other);

				Timeout * t1 = dynamic_cast<Timeout*>(this);
				Timeout * t2 = dynamic_cast<Timeout*>(other);
				// if both are complete events
				if((c1 != nullptr || i1 != nullptr || t1 != nullptr) && (c2 != nullptr || i2 != nullptr || t2 != nullptr)){
					return compareHelper(other);
				} else if((c1 != nullptr || i1 != nullptr || t1 != nullptr)){
					return -1; // this is smaller
				} else if(c2 != nullptr || i2 != nullptr || t2 != nullptr){
					return 1; // this is bigger
				} else {
					return compareHelper(other);
				}
			}

		} else { //event times are different
			return this->eventTime - e->getTime();
		}
	} else {
		return 0;
	}


	// put comment open here

	Event *e = dynamic_cast<Event *>(other);
	if(this->eventTime == e->getTime()){
		// see if one is arrival
		Arrival *a1 = dynamic_cast<Arrival *>(this);
		Arrival *a2 = dynamic_cast<Arrival *>(other);
		if(a1 != nullptr)
			return -1; // a1 goes first
		else if(a2 != nullptr)
			return 1; // a2 goes first
		else { // compare process arrival times
			Process *p = dynamic_cast<Process *>(process);
			if(p != nullptr){
				return p->compareTo(e->process);
			} else { // should never happen
				return 0;
			}
		}
	} else { // if times aren't equal
		return this->eventTime - e->getTime();
	}
	 // put comment close here

} */

int Event::getTime() const {
	return eventTime;
}

int Event::compareTo(ListItem *other) {
	// this is really convoluted, but its the simplest way i could think of

	// order by event time
	// lower event time goes first
	// if even time equal
	//	then order is: complete < arrival < everything else
	// 	for everything else with equal time, return this->process->compareTo(other->process)
	Event *e = dynamic_cast<Event*>(other);
	if(e != nullptr) {
		if (eventTime == e->getTime()) {
			// check for Complete type
			Arrival *a1 = dynamic_cast<Arrival *>(this);
			Arrival *a2 = dynamic_cast<Arrival *>(other);
			if(a1 != nullptr && a2 == nullptr){ // only this is Arrival
				return -1;
			} else if(a1 == nullptr && a2 != nullptr){ //only other is Arrival
				return 1;
			} else if(a1 != nullptr && a2 != nullptr){ //both Arrival
				return this->process->compareTo(a2->getProcess()); // compare process arrival times
			} else { //neither are arrival
				//check for Complete type
				Complete *c1 = dynamic_cast<Complete*>(this);
				Complete *c2 = dynamic_cast<Complete*>(other);
				if(c1 != nullptr && c2 == nullptr){ // this is Complete
					return -1;
				} else if(c1 == nullptr && c2 != nullptr){ // other is Complete
					return 1;
				} else if(c1 != nullptr && c2 != nullptr){ // both are Complete
					return this->process->compareTo(c2->getProcess()); // compare process arrival times
				} else { // neither are complete or arrival types
					return this->process->compareTo(e->getProcess());
				}
			}
		} else { //time unequal
			return this->eventTime - e->getTime();
		}

	} else { // other is not an event, shouldn't ever happen
		cout << "Item in Queue is not an event, how does this even happen\n";
		return 0;
	}
	/*
	Event *e = dynamic_cast<Event*>(other);
	if(e != nullptr){
		if(this->eventTime == e->getTime()){
			Arrival *a = dynamic_cast<Arrival*>(other);
			if(a != nullptr){
				return 1;
			} else {
				Process *p = dynamic_cast<Process *>(this->process);
				return p->compareTo(e->process);
			}
		} else {
			return this->eventTime - e->getTime();
		}
	} else {
		return 0;
	}*/


	/*
	Complete *c = dynamic_cast<Complete*>(other);
	Event *e = dynamic_cast<Event*>(other);


	if(e != nullptr){
		if(c != nullptr){
			// both this and c are complete types
			if(this->eventTime == c->getTime()){
				// compare process time
				return process->compareTo(c->process);
			} else {
				return this->eventTime - c->getTime();
			}
		} else { // only this is a complete event
			if(this->eventTime == e->getTime()){
				Arrival *a = dynamic_cast<Arrival*>(other);
				if(a == nullptr) // this is smaller
					return -1;
				else // this is bigger
					return 1;
			} else {
				return this->eventTime - e->getTime();
				// compare process time
			}
		}

	} else {
		return -1;
	}*/
}

ListItem *Event::getProcess() {
	return process;
}

