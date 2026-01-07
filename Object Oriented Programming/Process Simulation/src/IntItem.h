
#pragma once
#include "ListItem.h"
class IntItem : public ListItem{

	private:
		int time;
	public:
		IntItem(int t);
		int get();

		int compareTo(ListItem *other);


};



