
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
	
	int n = s->current.c->num_cities;
	
	for (int i=0; i<n; i++)
	{
		if (s->current.already_serviced[i])
		{
			continue;
		}
		
		s->current.already_serviced[i] = 1;
		s->current.path[cindex] = i;
		
		backtrack(s, start_index, stop_index, cindex+1);
		
		s->current.already_serviced[i] = 0;
	}
}

}


void backtrack(optimizer *s, int start_index, int stop_index)
{
	for (int i=start_index; i<stop_index; i++)
	{
		int old_value = s->current.path[i];
		if (old_value < 0)
		{
			continue;
		}
		s->current.already_serviced[old_value] = 0;
		s->current.path[i] = -1;
	}
	
	backtrack(s, start_index, stop_index, start_index);
}


optimizer* exact(city *c)
{
	optimizer *opt = new optimizer{"exact", c, 100};
	backtrack(opt, 0, c->num_cities);
	return opt;
}
