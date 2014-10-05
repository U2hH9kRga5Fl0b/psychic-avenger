#ifndef ROUTINES_H_
#define ROUTINES_H_

#include "slvr.h"

bool find_neighbor_2op_inter(solution *s, solution *out, int num_attempts);
void find_neighbor_2op(solution *s, solution *tmp, double locality);
void find_neighbor_2pts(solution *s, solution *tmp, double locality);
void find_neighbor_resched(solution *s, solution *tmp, double annealing_factor);

optimizer* exact(city *c);
solution*  shortest_path(city *c);


bool intersect(
	double x1b, double y1b,
	double x1e, double y1e,
	
	double x2b, double y2b,
	double x2e, double y2e);

bool find_intersecting_paths(solution* sol, int& idx1, int& idx2, int attempts);

#endif