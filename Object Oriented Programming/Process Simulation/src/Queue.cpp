
#include "Queue.h"
#include "ListItem.h"
#include "Node.h"

/**** Queue implementation */
Queue::Queue() : front(nullptr), back(nullptr), size(0) {}
int Queue::getSize(){ 
	return size; 
}
bool Queue::isEmpty(){ 
	return size == 0; 
}

void Queue::enqueue(ListItem *item){
	// special case: adding to empty queue
	if(front == nullptr){
		front = new Node(item, nullptr);
		back = front;
	} else {
		back->setNext(new Node(item, nullptr));
		back = back->getNext();
	}
	size++;
}// enqueue

ListItem *Queue::dequeue(){
	ListItem *theItem = nullptr;
	Node *theNode = front;
	if(front != nullptr){
		theItem = front->getItem();
		// special case: removing last item
		if(front == back){
			front = back = nullptr;
		} else {
			front = front->getNext();
		}
		size--;
		delete(theNode);
	}
	return theItem;
}// dequeue

void Queue::setFront(ListItem* n){
	front->setItem(n);
}

ListItem *Queue::getFront(){
	ListItem *theItem = nullptr;
	if(front != nullptr){
		theItem = front->getItem();
	}
	return theItem;
}

ListItem *Queue::getBack() {
	ListItem *theItem = nullptr;
	if(back != nullptr)
		theItem =  back->getItem();
	return theItem;
}
// getFront

