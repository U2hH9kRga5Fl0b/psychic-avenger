
#include "routines.h"

#include <cfloat>

solution *shortest_path(city *c)
{
	solution *s = new solution{c};
	int n = c->num_cities;
	
	s->path[0] = 0;
	s->already_serviced[0] = 1;
	
	for (int i=1;i<n;i++)
	{
		double min = DBL_MAX;
		
		int idxmin = -1;
		for (int j=0;j<n;j++)
		{
			if (s->already_serviced[j])
			{
				continue;
			}
			
			double d = c->get_cost(i, j);
			if (d < min)
			{
				idxmin = j;
				min = d;
			}
		}
		
		s->path[i] = idxmin;
		s->already_serviced[idxmin] = 1;
	}
	
	return s;
}
