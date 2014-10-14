#ifndef ROUTINES_H_
#define ROUTINES_H_

#include "solution.h"

solution* exact(city *c);

// If anneal = 0, this always chooses the most optimal point to add.
// If anneal = .5 it only considers adding each to roughly half the places it could be added.
void grow(solution* sol, std::function<void(void)> callback=[](){}, double anneal=1.0);
void grow(solution *sol, int desired, std::function<void(void)> callback, double anneal);

bool intersect(
	double x1b, double y1b,
	double x1e, double y1e,
	
	double x2b, double y2b,
	double x2e, double y2e);


void anneal_s(solution *sol, int convergence_threshold, double accept_negative_prob);
void anneal(solution *sol, int convergence_threshold, double tol, int start_anneal, int dec);

#endif