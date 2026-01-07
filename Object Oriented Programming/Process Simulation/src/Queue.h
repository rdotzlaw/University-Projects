#pragma once

class Node;
class ListItem;

class Queue {
protected:
	Node *front;
	Node *back;
	int size;
public:
	Queue();
	int getSize();
	bool isEmpty();
	virtual void enqueue(ListItem *item);
	ListItem *getFront();
	ListItem *getBack();
	ListItem *dequeue();

	void setFront(ListItem *n);
}; // class Queue
