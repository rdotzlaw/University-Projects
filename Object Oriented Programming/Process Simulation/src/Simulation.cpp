



#include "Simulation.h"
#include "PQueue.h"
#include "IntItem.h"


using namespace std;
/*TODO LIST:
 * DONE Events need to print out info on whats happening
 * DONE Processes need to keep track of arrivalTime, exitTime and waitTime
 * DONE When creating events, eventTime is the time from the file input + the time from the event that created them
 * DONE In compareTo, if both events have equal time, if one of the events is an instance of Arrival, then Arrival is smaller
 * DONE Reading in from file, pass line to Process as a Queue of IntItem
 * DONE Fix memory access violation when reading in a second process, most likely in Simulation::read() or in Arrival its possible that there is a problem when queueing events or processes into the events/qCPU/qIO queues
 * DONE Fix unnecessary dequeue from Simulation::events pQueue, causing program to terminate early
 * DONE ^ Early termination is from *e in Simulation::step() being recognized as a nullptr when it isn't
 * DONE ^ Events list is size 1, when the front node is null, something is most likely wrong with dequeue or enqueue for pQueue (problem was forgetting to set Node::back in PQueue::enqueue
 * Current issues with waitTime and event execution time can be solved using Priority Queues for qCPU and qIO, but there needs to be a way to order the processes using Process::current::eventTime
 *
 * How Execution works:
 * 	Process at the front of the CPU/IO queue will have it's front value of process->times
 * 		1. Reduced by up to maxTime if positive (CPU Burst)
 * 			a. Will create a Timout event and move process to back of qCPU if process->times->dequeue()->get() - maxTime != 0
 * 			b. Will otherwise create a CompleteCPU event which will either exit if process->times is empty or create a StartIO event
 * 		2. Dequeued from process->times (IO Burst)
 * 			a. Will then create a CompleteIO event which will either exit if process->times is empty or create a StartCPU event
 *
 */

Simulation::Simulation() {
	events = new PQueue(); // priority queue of events to do
	processes = new PQueue(); // priority queue of processes ordered by arrival time
	qCPU = new Queue(); // queue of processes
	qIO = new Queue(); // queue of processes
	//CPU = nullptr;
	//IO = nullptr;
	cpuStep = 0; // no need for io step, b/c no io timeout
	pId = 1;
	maxTime = -1;
	//globalTime = 1;
	file = nullptr;
}


void Simulation::runSimulation(char *fileName) {

	// set up file reader
	string buffer;
	cout << "Opening file : " << fileName << "\n";
	file = new ifstream;
	file->open(fileName);

	cout << "File opened\n";
	getline(*file,buffer);
	cout << "Processes will timeout in : " << buffer << " steps.\n";
	this->maxTime = stoi(buffer); // get int value of 1st line

	// create the 1st process to load arrival event into pQueue, then step
	read();
	step(); // calls self recursively until done everything

	file->close(); // all events executed, close file
}

void Simulation::step() { // recursively calls itself until EOF and all events done

	if(!events->isEmpty()) { //still have events to execute
		Event *e = dynamic_cast<Event *>(events->dequeue());
		if(e != nullptr){
			//cout << "Handling event : " << e << " at time: " << e->getTime() << "\n";
			e->handleEvent();
			step();
		} else {
			cout << "null  ==  "<< e <<"\n";
			cout << "size : " << events->getSize() << "\n";
		}
	} else {
		cout << "All events finished, ending simulation.\n";
	}

}


void Simulation::read() {

	/*
	static string s;
	// read int by int
	char c;
	file->get(c);
	if(c != ' ' && c != '\n'){
		s = s + c;
		read();
	} else { // c == ' ' or c == '\n'
		// done reading in int
		int time = stoi(s);
		return time;
	}*/


	string buffer;
	Queue *q = new Queue();
	int i = 0;
	string delimiter = " ";
	size_t pos = 0;
	if(getline(*file, buffer) && !buffer.empty()) { // get next line, if there is one
		//cout << "--Grabbing next Line\n";
		while ((pos = buffer.find(delimiter)) != string::npos) { // while there is still an instance of delimiter in buffer
			// take the substring from 0 to the next instance of delimiter
			q->enqueue( new IntItem(abs(stoi(buffer.substr(0, pos))))); // int val of the substring
			i++;
			// remove everything up to, and including the next instance of the delimiter from position:0
			buffer.erase(0, pos + delimiter.length());
		}
		// no more delimiters, just the last int value left
		q->enqueue(new IntItem(abs(stoi(buffer))));

		// create the first arrival event and process

		int i = dynamic_cast<IntItem*>(q->dequeue())->get(); // literally cant be anything but an IntItem
		ListItem *p = new Process(this, q, i, pId); // i is the arrival time of the process, not needed aside from construction
		cpuStep = i;
		ListItem *a = new Arrival(i, p, this);
		events->enqueue(a);

		pId++;
	} // end if

}

int Simulation::ioTime(){ // time until all processes done in io
	Process *p = dynamic_cast<Process *>(qIO->getBack());
	return p->getCurrentDone();
}

int Simulation::cpuTime(){ // time until all processes done in cpu
	Process *p = dynamic_cast<Process *>(qCPU->getBack());
	if(p != nullptr)
		return p->getCurrentDone();
	else return 0;
}


bool Simulation::cpuBusy() {
	return !qCPU->isEmpty();
}

bool Simulation::ioBusy(){
	return !qIO->isEmpty();
}

void Simulation::summary() {
	cout << "Process    Arrival    Exit    Wait\n      #       Time    Time    Time\n- - - - - - - - - - - - - - - - - - - - -\n";
	while(!processes->isEmpty()){
		Process *p = dynamic_cast<Process *>(processes->dequeue());
		if(p != nullptr){
			p->out(); // summary of arrivalTime, ExitTime and WaitTime
		}
	}
}

void Simulation::addEventCPU(ListItem *i) {
	qCPU->enqueue(i);
}

void Simulation::addEventIO(ListItem *i) {
	qIO->enqueue(i);
}

void Simulation::addEvent(ListItem *i) {
	events->enqueue(i);
}

int Simulation::getMax() const {
	return maxTime;
}

//needed for testing suite
void Simulation::setMax(int n){
	maxTime = n;
}

ListItem *Simulation::dqCPU() {
	return qCPU->dequeue();
}

ListItem *Simulation::dqIO(){
	return qIO->dequeue();
}

int Simulation::cpuSteps() {
	return cpuStep;
}

int Simulation::cpuSteps(int n) {
	cpuStep += n;
	return cpuStep;
}

void Simulation::log(Process *pProcess) {
	processes->enqueue(pProcess);
}
