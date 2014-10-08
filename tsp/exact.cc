
#include "routines.h"

namespace 
{

void backtrack(optimizer *s, int start_index, int stop_index, int cindex)
{
	if (cindex >= stop_index)
	{
		s->offer();
		return;
	}
	
	int n = s->current.get_city()->num_cities;
	
	for (int i=0; i<n; i++)
	{
		if (s->current.has_serviced(i))
		{
			continue;
		}
		
		s->current.service(cindex, i);
		backtrack(s, start_index, stop_index, cindex+1);
		s->current.service(cindex, -1);
	}
}

void backtrack(optimizer *s, int start_index, int stop_index)
{
	for (int i=start_index; i<stop_index; i++)
	{
		s->current.service(i, -1);
	}
	
	backtrack(s, start_index, stop_index, start_index);
}

}


solution* exact(city *c)
{
	optimizer opt{"exact", c, 100};
	backtrack(&opt, 0, c->num_cities);
	
	solution* ret = new solution{c};
	(*ret) = opt.best;
	return ret;
}
