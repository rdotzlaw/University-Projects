
#include "IntItem.h"

IntItem::IntItem(int t) {
	time = t;
}

int IntItem::get() {
	return time;
}

int IntItem::compareTo(ListItem *other) {
	IntItem *i = dynamic_cast<IntItem*>(other);
	if(i != nullptr)
		return time - i->get();
	else
		return -1;
}


