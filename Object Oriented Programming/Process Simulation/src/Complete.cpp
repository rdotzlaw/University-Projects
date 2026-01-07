#include "Complete.h"
#include "Arrival.h"

Complete::Complete(int theTime, ListItem *theProcess, Simulation *sim) : Event(theTime, theProcess, sim) {
 // nothin here
}

/*
int Complete::compareTo(ListItem *other) {
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
	}


}*/