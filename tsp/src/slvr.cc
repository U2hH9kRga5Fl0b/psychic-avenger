
#include "slvr.h"

optimizer::optimizer(const std::string& name, city *c) :
	best{c},
	current{c},
	vbest{name + "_best"},
	vcurrent{name + "_current"} {}

optimizer::~optimizer() {}

bool optimizer::offer()
{
	vcurrent.update(&current);
	
	if (current < best)
	{
		best = current;
		vbest.update(&best);
		
		return true;
	}
	
	return false;
}
