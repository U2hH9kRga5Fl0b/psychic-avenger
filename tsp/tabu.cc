#include "tabu.h"

#include "geometry.h"


#include <float.h>

tabu_option::tabu_option(int stop1_, int stop2_, city* c) : 
	stop1{stop1_},
	stop2{stop2_},
	dist{c->get_cost(stop1_,stop2_)},
	next{nullptr} {}
tabu_option::~tabu_option() { delete next; }

swap_tabu_option::swap_tabu_option(int stop1, int stop2, city* c) : tabu_option{stop1, stop2, c} {}
swap_tabu_option::~swap_tabu_option() {}

reschedule_tabu_option::reschedule_tabu_option(int stop1, int stop2, city* c) : tabu_option{stop1, stop2, c} {}
reschedule_tabu_option::~reschedule_tabu_option() {}


namespace
{
	bool priv_in_bounds(int ndx, solution* sol)
	{
		return ndx >= 0 && ndx < sol->get_city()->num_cities && sol->get_stop(ndx) >= 0;
	}
}

double swap_tabu_option::get_cost_along_path(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	if (ndx2 < ndx1)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}
	
	double cost = 0.0;
	for (int i=ndx1; i<ndx2; i++)
	{
		cost += sol->get_city()->get_cost(sol->get_stop(i), sol->get_stop(i+1));
	}
	return cost;
}


double swap_tabu_option::get_improvement(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2); 
	
	city* c = sol->get_city();
	
	// get previous cost
	double o = 
		c->get_cost(
			sol->get_stop(ndx1-1),
			sol->get_stop(ndx1  )) + 
		c->get_cost(
			sol->get_stop(ndx1-1),
			sol->get_stop(ndx1  ));
	// get new cost
	double n = 
		c->get_cost(
			sol->get_stop(ndx1-1),
			sol->get_stop(ndx2  )) + 
		c->get_cost(
			sol->get_stop(ndx2-1),
			sol->get_stop(ndx1  ));
	
	
	return n - o;
}

bool swap_tabu_option::in_bounds(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	return
		priv_in_bounds(ndx2  , sol) &&
		priv_in_bounds(ndx2-1, sol) && 
		priv_in_bounds(ndx1  , sol) &&
		priv_in_bounds(ndx1-1, sol);
}

bool swap_tabu_option::intersects(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	
	const city& c = *(sol->get_city());
	
	double x_out, y_out;
	
	return intersect(
		c.locsx[sol->get_stop(ndx1-1)], c.locsy[sol->get_stop(ndx1-1)],
		c.locsx[sol->get_stop(ndx1  )], c.locsy[sol->get_stop(ndx1  )],
		
		c.locsx[sol->get_stop(ndx2-1)], c.locsy[sol->get_stop(ndx2-1)],
		c.locsx[sol->get_stop(ndx2  )], c.locsy[sol->get_stop(ndx2  )],
		x_out, y_out);
}


void swap_tabu_option::apply(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	if (ndx2 < ndx1)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}
	
	for(;ndx1<ndx2;ndx1++,ndx2--)
	{
		int f = sol->get_stop(ndx1);
		int s = sol->get_stop(ndx2);
		
		sol->service(ndx1, s);
		sol->service(ndx2, f);
	}
	
	sol->is_valid();
}




double reschedule_tabu_option::get_improvement(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	city* ci = sol->get_city();
	
	int a = sol->get_stop(ndx1-1);
	int s = sol->get_stop(ndx1  ); // aka stop1
	int b = sol->get_stop(ndx1+1);
	int c = sol->get_stop(ndx2  ); // aka stop2
	int d = sol->get_stop(ndx2+1);
	
	// alternative 1: ...     a   s   b     ...     c       d ...
	// alternative 2: ...     a       b     ...     c   s   d ...
	
	// get previous cost
	double o = 
		ci->get_cost(a,s) + 
		ci->get_cost(s,b) + 
		ci->get_cost(c,d);
	// get new cost
	double n = 
		ci->get_cost(a,b) + 
		ci->get_cost(c,s) + 
		ci->get_cost(s,d);

	return n - o;
}

bool reschedule_tabu_option::in_bounds(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	return 
		priv_in_bounds(ndx1-1, sol) &&
		priv_in_bounds(ndx1+1, sol) &&
		priv_in_bounds(ndx2  , sol) &&
		priv_in_bounds(ndx2+1, sol);
}

void reschedule_tabu_option::apply(solution* sol)
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	// twice as many operations as we need...
	int stop = sol->remove_at(ndx1);
	if (stop != stop1)
	{
		std::cout << "This should never happen..." << std::endl;
	}
	
	sol->insert_at(stop1, ndx2);
	
	sol->is_valid();
}









tabu_options::tabu_options(city* c) : swap_options{nullptr}, reschedule_options{nullptr}
{
	int n = c->num_cities;
	for (int i=0; i<n; i++)
	{
		for(int j=i+1; j<n; j++)
		{
			reschedule_tabu_option* r = new reschedule_tabu_option{i, j, c};
			r->next = reschedule_options;
			reschedule_options = r;
			
			swap_tabu_option* s = new swap_tabu_option{i, j, c};
			s->next = swap_options;
			swap_options = s;
		}
	}
}
tabu_options::~tabu_options() { delete swap_options; delete reschedule_options; }

tabu_option* tabu_options::get_best_option(solution* sol)
{
	double best_improvement = -DBL_MAX;
	tabu_option* best = nullptr;
	
	{
		swap_tabu_option* swap = swap_options;
		while (swap != nullptr)
		{
			if (!swap->in_bounds(sol))
			{
				swap = (swap_tabu_option*) swap->next;
				continue;
			}
			
			double improvement = swap->get_improvement(sol);
			if (improvement <= 0)
			{
				swap = (swap_tabu_option*) swap->next;
				continue;
			}
			
			if (improvement > best_improvement)
			{
				best = swap;
				best_improvement = improvement;
			}
			
			swap = (swap_tabu_option*) swap->next;
		}
	}
	
	{
		reschedule_tabu_option* resched = reschedule_options;
		while (resched != nullptr)
		{
			if (!resched->in_bounds(sol))
			{
				resched = (reschedule_tabu_option*) resched->next;
				continue;
			}
			
			double improvement = resched->get_improvement(sol);
			if (improvement <= 0)
			{
				resched = (reschedule_tabu_option*) resched->next;
				continue;
			}
			
			if (improvement > best_improvement)
			{
				best = resched;
				best_improvement = improvement;
			}
			
			resched = (reschedule_tabu_option*) resched->next;
		}
	}
	
	return best;
}



tabu_option* tabu_options::get_random_option(solution* sol)
{
	if (sol->get_num_serviced() < 5)
	{
		return nullptr;
	}
	
	tabu_option *start = (rand() % 2) ? (tabu_option *)reschedule_options : (tabu_option *)swap_options;
	tabu_option *ran = start;
	
	int len = sol->get_city()->num_cities * (sol->get_city()->num_cities - 1) / 2;
	int num = rand() % len;
	
	for (int i=0;i<num || !ran->in_bounds(sol);i++)
	{
		ran = ran->next;
		if (ran == nullptr)
		{
			ran = start;
		}
	}
	
	return ran;
}


std::ostream& operator<<(std::ostream& out, const tabu_option& op)
{
	return out << "[t=" << op.get_name() << "][s1=" << op.stop1 << "][s2=" << op.stop2 << "]";
}

std::string swap_tabu_option::get_name() const
{
	return "swap";
}

std::string reschedule_tabu_option::get_name() const
{
	return "resched";
}
