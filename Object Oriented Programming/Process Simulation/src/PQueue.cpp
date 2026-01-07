


#include "PQueue.h"
#include "Node.h"
#include "ListItem.h"
#include <iostream>
using namespace std;

/*
// when events happen at the same time, they are ordered by arrival time
void PQueue::enqueue(ListItem *item) {
	// special case: adding to empty queue
	if(front == nullptr){
		front = new Node(item, nullptr);
		back = front;
	} else if(back->getItem()->compareTo(item) <= 0){ // item >= back, if front != nullptr then back != nullptr
		back->setNext(new Node(item, nullptr));
		back = back->getNext();
	} else { // need to search thru until reach proper spot
		Node *curr = front; // cant be nullptr
		Node *prev = nullptr;
		while(curr != nullptr && curr->getItem()->compareTo(item) < 0){ //while curr is smaller than item
			prev = curr;
			curr = curr->getNext();
		}
		Node *n = new Node();
		n->setItem(item);
		n->setNext(curr);
		prev->setNext(n);
	}
	size++;
}*/

void PQueue::enqueue(ListItem *item) {
// problems here!!!!
	// what are the problems thoo
	if (front == nullptr) {
		//cout << "Front is null\n";
		front = new Node(item, nullptr);
		back = front;
	} else if(front->getItem()->compareTo(item) >= 0){ // front is bigger than item
		Node *n = new Node(item, front);
		front = n;
	} else{
		Node *curr = front;
		Node *prev = curr;

		while(curr != nullptr && curr->getItem()->compareTo(item) <= 0){ // obj->compareTo(item) returns < 0 if obj < item

			prev = curr;
			curr = curr->getNext();
		} // until curr >= item or prev == back
		Node *n = new Node(item, curr);
		if(curr == nullptr)
			back = n;
		prev->setNext(n);
		//cout << "enqueueing " << n << "as " << prev << "'s next\n";
	}

	size++;
	//sort();
}

// somehow my queue isn't in order, can't figure out why, so here's a terrible sort to fix it
// nevermind, when making my events queue in Simulation constructor, i was making it a regular queue instead of a priority queue...
/*
void PQueue::sort(){
	Node *n =  new Node(this->dequeue(), nullptr);
	int i = 0;
	while(i < size){
		Node *next = new Node(this->dequeue(), nullptr);
		Node *curr = n;
		while(curr != nullptr){


			curr = curr->getNext();
		}

		i++;
	}

}
*/