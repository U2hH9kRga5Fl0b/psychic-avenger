#include "tabu.h"


swap_tabu_option::swap_tabu_option(int stop1_, int stop2_, city* c) : 
	stop1{stop1_},
	stop2{stop2_},
	dist{c->distances[stop1_][stop2_]},
	next{nullptr} {}
swap_tabu_option::~swap_tabu_option() { delete next; }


double swap_tabu_option::get_cost_along_path(solution* sol) const
{
	return -1.0;
}

double swap_tabu_option::get_improvement(solution* sol) const
{
	return -1.0;
}

bool intersects(solution* sol) const
{
	return false;
}






reschedule_tabu_option::reschedule_tabu_option(int stop1_, int stop2_, city* c) : 
	stop1{stop1_},
	stop2{stop2_},
	dist{c->distances[stop1_][stop2_]},
	next{nullptr} {}
reschedule_tabu_option::~reschedule_tabu_option() { delete next; }
	
double reschedule_tabu_option::get_improvement(solution* sol) const
{
	
}

tabu_options::tabu_options() : swap_options{nullptr}, reschedule_options{nullptr} {}
tabu_options::~tabu_options() { delete swap_options; delete reschedule_options; }

tabu_options* genereate_tabu_options(city* c)
{
	tabu_options* options = new tabu_options;
	
	int n = c->num_cities;
	for (int i=0; i<n; i++)
	{
		for(int j=i+1; j<n; j++)
		{
			// Need to be able to schedule AFTER j (j cant be n-1)
			reschedule_tabu_option* r = new reschedule_tabu_option{i, j, c};
			r->next = tabu_options->reschedule_options;
			tabu_options->reschedule_options = r;
			
			swap_tabu_option* s = new swap_tabu_option{i, j, c};
			s->next = tabu_options->swap_options;
			swap_options->reschedule_options = s;
		}
	}
	
	return options;
	
}


