#ifndef SUBDIVIDER_H_
#define SUBDIVIDER_H_

#include "solution.h"

class subcity
{
public:
	subcity(city*p, city*s, int*m);
	~subcity();
	
	city* parent;
	city* c;
	int *mapping;
};


subcity *get_subcity(city *old, double xmin, double xmax, double ymin, double ymax);
void append_to(subcity *subcity, solution *subsol, solution *sol);


solution *subdivide_backtrack(city *beginning, void (improve)(solution* s), int max_num_stops=50);

#endif