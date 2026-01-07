
#pragma once
#include "Queue.h"
class Node;
class ListItem;

class PQueue : public Queue {
	// only change is in Queue::enqueue function
private:
	//void sort();
public:
	void enqueue(ListItem *item);


};