
#ifndef GEOMETRY_H_
#define GEOMETRY_H_


#include "solution.h"

bool intersect(
	double x1b, double y1b,
	double x1e, double y1e,
	
	double x2b, double y2b,
	double x2e, double y2e,
	
	double& x_intersection, double& y_intersection);



void test_intersect();


bool find_intersecting_paths(solution* sol, int& idx1, int& idx2, int attempts);


void get_bounds(double *allx, double *ally, int n,
		double&xmin, double& xmax, double&ymin, double& ymax);




#endif