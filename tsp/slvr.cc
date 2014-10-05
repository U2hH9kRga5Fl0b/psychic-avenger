
#include "slvr.h"

optimizer::optimizer(const std::string& name, city *c, int freq) :
	best{c},
	current{c},
	vbest{&best, name + "_best"},
	vcurrent{&current, name + "_current", freq} {}

optimizer::~optimizer() {}

bool optimizer::offer()
{
	vcurrent.update();
	
	if (current < best)
	{
		best = current;
		vbest.update();
		
		return true;
	}
	
	return false;
}
