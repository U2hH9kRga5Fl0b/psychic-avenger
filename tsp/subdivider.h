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


solution *backtrack(city *beginning, void (improve)(solution* s), int max_num_cities=50);

#endif