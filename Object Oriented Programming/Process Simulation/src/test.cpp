
#pragma once
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "PQueue.h"
#include "IntItem.h"
#include "Simulation.h"
#include "Arrival.h"
#include "Process.h"
#include "StartCPU.h"
#include "CompleteCPU.h"
#include "CompleteIO.h"


// test priority queue
TEST_CASE("Tests pQueue", "[pQueue]"){
	cout << "IGNORE OUTPUT TO COMMAND LINE\n";
	PQueue *p = new PQueue();
	p->enqueue(new IntItem(1));
	p->enqueue(new IntItem(1));
	p->enqueue(new IntItem(2));
	p->enqueue(new IntItem(3));
	p->enqueue(new IntItem(0));

	REQUIRE(dynamic_cast<IntItem*>(p->dequeue())->get() == 0);
	REQUIRE(dynamic_cast<IntItem*>(p->dequeue())->get() == 1);
	REQUIRE(dynamic_cast<IntItem*>(p->dequeue())->get() == 1);
	REQUIRE(dynamic_cast<IntItem*>(p->dequeue())->get() == 2);
	REQUIRE(dynamic_cast<IntItem*>(p->dequeue())->get() == 3);
}

TEST_CASE("Tests Process::compareTo", "[Process]"){
	// needs to work to test Event::compareTo
	Process *p1 = new Process(nullptr, nullptr, 1, 1);
	Process *p2 = new Process(nullptr, nullptr, 3, 2);
	Process *p3 = new Process(nullptr, nullptr, 3, 3);

	REQUIRE(p1->compareTo(p2) < 0);
	REQUIRE(p2->compareTo(p1) > 0);
	REQUIRE(p2->compareTo(p3) == 0);
}

TEST_CASE("Tests Event::compareTo" , "[Event]"){
	Process *p1 = new Process(nullptr, nullptr, 1, 1);
	Process *p2 = new Process(nullptr, nullptr, 3, 2);


	Event *e1 = new Arrival(1, p1, nullptr);
	Event *e2 = new StartCPU(1, p1, nullptr);
	REQUIRE(e1->compareTo(e2) < 0); // arrival 1st

	e1 = new StartCPU(4, p1, nullptr);
	e2 = new StartCPU(4, p2, nullptr);
	REQUIRE(e1->compareTo(e2) < 0); // e1 first
	REQUIRE(e2->compareTo(e1) > 0);

	e1 = new StartCPU(4, p1, nullptr);
	e2 = new StartCPU(4, p1, nullptr);
	REQUIRE(e1->compareTo(e2) == 0); //same

	e1 = new CompleteCPU(4, p1, nullptr);
	e2 = new StartCPU(4, p1, nullptr);
	REQUIRE(e1->compareTo(e2) < 0); //e1 first



}

// cannot test Arrival, since the arrival::handleEvent calls Simulation::read, which will seg fault w/out file input
TEST_CASE("Tests StartCPU flow: Timeout", "[StartCPU]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(10));
	Process *p1 = new Process(s, q, 1, 1);


	StartCPU *a = new StartCPU(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<Timeout *>(p1->getCurrent());
	REQUIRE(current != nullptr); // StartCPU creates a Timeout
}
TEST_CASE("Tests StartCPU flow: CompleteCPU", "[StartCPU]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(8));
	Process *p1 = new Process(s, q, 1, 1);


	StartCPU *a = new StartCPU(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<CompleteCPU *>(p1->getCurrent());
	REQUIRE(current != nullptr); // StartCPU creates a CompleteCPU
}


TEST_CASE("Tests CompleteCPU flow: Exit", "[CompleteCPU]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(8));
	Process *p1 = new Process(s, q, 1, 1);


	CompleteCPU *a = new CompleteCPU(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<Exit *>(p1->getCurrent());
	REQUIRE(current != nullptr); // CompleteCPU creates an Eit
}
TEST_CASE("Tests CompleteCPU flow: StartIO", "[CompleteCPU]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(8));
	q->enqueue(new IntItem(4));
	Process *p1 = new Process(s, q, 1, 1);


	CompleteCPU *a = new CompleteCPU(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<StartIO *>(p1->getCurrent());
	REQUIRE(current != nullptr); // CompleteCPU creates an Eit
}

TEST_CASE("Tests CompleteIO flow: StartCPU", "[CompleteIO]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(8));
	q->enqueue(new IntItem(4));
	Process *p1 = new Process(s, q, 1, 1);


	CompleteIO *a = new CompleteIO(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<StartCPU *>(p1->getCurrent());
	REQUIRE(current != nullptr); // CompleteCPU creates an Eit
}
TEST_CASE("Tests CompleteIO flow: Exit", "[CompleteIO]") {

	//Process *p2 = new Process(nullptr, nullptr, 3, 2);
	//Process *p3 = new Process(nullptr, nullptr, 3, 3);

	Simulation *s = new Simulation();
	s->setMax(9);
	Queue *q = new Queue();
	q->enqueue(new IntItem(8));
	Process *p1 = new Process(s, q, 1, 1);


	CompleteIO *a = new CompleteIO(1, p1, s);
	a->handleEvent(); // should create startCPU event as p1's current event

	Event *current = dynamic_cast<Exit *>(p1->getCurrent());
	REQUIRE(current != nullptr); // CompleteCPU creates an Eit
}


