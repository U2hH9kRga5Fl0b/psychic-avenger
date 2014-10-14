#include "routines.h"
#include "common.h"

#include <float.h>

namespace
{

bool find_best_location_to_add(
	// the solution, and how many requests it has already serviced
	const solution*sol, int len,
	// the one we are trying to add
	int additional,
	// the place to add it, and the cost of adding it
	int& location, double& min_cost,
	double anneal)
{
//	std::cout << "Going to grow \n" << *sol << "\n with additional=" << additional << std::endl;
	
	
	bool improved = false;
	const city* c = sol->get_city();
	
	if (c->get_cost(sol->get_stop(0), additional) < min_cost)
	{
		min_cost = c->get_cost(sol->get_stop(0), additional);
		improved = true;
		location = 0;
	}
	if (c->get_cost(sol->get_stop(len-1), additional) < min_cost)
	{
		min_cost = c->get_cost(sol->get_stop(len-1), additional);
		improved = true;
		location = len;
	}
	
	int stop = std::min((int) (anneal * len), len);
	
	// consider several places to put it...
	for (int i=1; i<stop; i++)
	{
//		std::cout << "?? add it at " << i << std::endl;
		
		double current_cost =
			c->get_cost(
				sol->get_stop(i-1),
				sol->get_stop(i));
		
		double other_cost =
			c->get_cost(
				sol->get_stop(i-1),
				additional) +
			c->get_cost(
				additional,
				sol->get_stop(i));
		
		double delta = other_cost - current_cost;
//		std::cout << "delta = " << delta << std::endl;
		
		if (delta < min_cost)
		{
//			std::cout << "improvement!" << std::endl;
			improved = true;
			
			min_cost = delta;
			location = i;
		}
	}
	
	return improved;
}

}



void grow(solution *sol, int desired, std::function<void(void)> callback, double anneal)
{
	city* c = sol->get_city();
	
	if (sol->get_stop(0) < 0)
	{
		// It needs to service at least one stop
		int stop = rand() % c->num_stops;
		std::cout << "Adding random stop = " << stop << " " << std::endl;
		sol->insert_at_ndx(stop, 0);
		
//		std::cout << "After adding: " << std::endl;
//		std::cout << *sol << std::endl;
	}
	
	int n = c->num_stops;
	int len = sol->length();
	int rem = n - len;
	    desired = std::min(n, desired);
	int toadd = desired - len;
	int *remaining = new int[rem];
	
	{
		int ndx=0;
		for (int i=0; i<n; i++)
		{
			if (sol->has_serviced(i))
			{
				continue;
			}
			remaining[ndx++] = i;
		}
		if (ndx != rem)
		{
			std::cout << "Did not find the proper number of remaining points." << std::endl;
			trap();
		}
	}
	
	for (int i=0; i<toadd; i++)
	{
		double min_cost = DBL_MAX;
		int place_to_add = -1;
		int index_to_add = -1;
		
	
		std::random_shuffle(&remaining[i], &remaining[rem]);
	
		for (int j=i; j<rem; j++)
		{
			if (find_best_location_to_add(sol, len+i,
				remaining[j],
				place_to_add, min_cost,
				anneal))
			{
				index_to_add = j;
			}
		}
		if (place_to_add < 0 || index_to_add < 0)
		{
			std::cout << "Did not find an improvement." << std::endl;
			trap();
		}
		
//		std::cout << "min was " << remaining[index_to_add] << std::endl;
//		std::cout << "=================================================================================" << std::endl;
		
		double old_cost = sol->get_cost();
		sol->insert_at_ndx(remaining[index_to_add], place_to_add);
		double new_cost = sol->get_cost();
		
		callback();
		
		if (abs(min_cost - (new_cost - old_cost)) > 1e-15)
		{
			std::cout << "cost disagreement.";
			trap();
		}
		
		int tmp = remaining[index_to_add];
		remaining[index_to_add] = remaining[i];
		remaining[i] = tmp;
	}
	
	delete[] remaining;
}


void grow(solution* sol, std::function<void(void)> callback, double anneal)
{
	grow(sol, sol->get_city()->num_stops, callback, anneal);
}
