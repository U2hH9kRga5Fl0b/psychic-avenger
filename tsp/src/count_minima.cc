#include "count_minima.h"


#include <map>

void sample_decision_space(city* c) {}


void complete_decision_space(city* c)
{
	minima_collection collection;
		
	solution sol{c};
	sol.random();
	permutation p {&sol};
	
	do
	{
		p.assign(&sol);
		collection.sample(&sol);
	} while(p.increment());

	collection.view();
}

anneal_statistics* find_neighbors(solution* sol, int distance_to_anneal)
{
	return nullptr;
}

