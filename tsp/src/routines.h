#ifndef ROUTINES_H_
#define ROUTINES_H_

#include "solution.h"

solution* exact(city *c);
void grow(solution* sol, std::function<void(void)> callback=[](){});

bool intersect(
	double x1b, double y1b,
	double x1e, double y1e,
	
	double x2b, double y2b,
	double x2e, double y2e);

#endif