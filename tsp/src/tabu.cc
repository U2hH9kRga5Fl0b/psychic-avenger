#include "tabu.h"

#include "geometry.h"
#include "common.h"

#include <algorithm>


#include <float.h>

tabu_option::tabu_option(int stop1_, int stop2_, city* c, const tabu_option *n) : 
	stop1{stop1_},
	stop2{stop2_},
	dist{c->get_cost(stop1_,stop2_)},
	next{n} {}

tabu_option::~tabu_option()
{
	// Does this causes a stack overflow for large TSP problems:
	// delete next;
	// Instead, there is a while loop in the tabu_options class.
}

swap_tabu_option::swap_tabu_option(int stop1, int stop2, city* c, const tabu_option *n) : tabu_option{stop1, stop2, c, n} {}
swap_tabu_option::~swap_tabu_option() {}

reschedule_tabu_option::reschedule_tabu_option(int stop1, int stop2, city* c, const tabu_option *n) : tabu_option{stop1, stop2, c, n} {}
reschedule_tabu_option::~reschedule_tabu_option() {}


namespace
{
	bool priv_in_bounds(int ndx, const solution* sol)
	{
		return ndx >= 0 && ndx < sol->get_city()->num_cities && sol->get_stop(ndx) >= 0;
	}
}

double swap_tabu_option::get_cost_along_path(const solution* sol) const
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


double swap_tabu_option::get_improvement(const solution* sol) const
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2); 
	
//	std::cout << ndx1 << " and " << ndx2 << std::endl;
//	std::cout << "---------------------------------" << std::endl;
	
	city* c = sol->get_city();
	
	if (ndx1 > ndx2)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}
	
	
	
	
	// get previous cost
	double o = 0.0;
	// get new cost
	double n = 0.0;
	
	bool has_prev_point = ndx1 > 0;
	if (has_prev_point)
	{
		o += c->get_cost(
			sol->get_stop(ndx1-1),
			sol->get_stop(ndx1  ));
		
		n += c->get_cost(
			sol->get_stop(ndx1-1),
			sol->get_stop(ndx2  ));
	}
	
	bool has_next_point = ndx2 + 1 < c->num_cities && sol->get_stop(ndx2+1) >= 0;
	if (has_next_point)
	{
		o += c->get_cost(
			sol->get_stop(ndx2  ),
			sol->get_stop(ndx2+1));
		n +=  c->get_cost(
			sol->get_stop(ndx1  ),
			sol->get_stop(ndx2+1));
	}
	
	return n - o;
}

bool swap_tabu_option::in_bounds(const solution* sol) const
{
	return sol->get_index_of_stop(stop1) >= 0 &&
	       sol->get_index_of_stop(stop2) >= 0;
}

bool swap_tabu_option::intersects(const solution* sol) const
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


void swap_tabu_option::apply(solution* sol) const
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




double reschedule_tabu_option::get_improvement(const solution* sol) const
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	if (abs(ndx1 - ndx2) < 2)
	{
		// should handle this situation some day...
		return DBL_MAX;
	}
	
	
//	std::cout << ndx1 << " and " << ndx2 << std::endl;
	
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

bool reschedule_tabu_option::in_bounds(const solution* sol) const
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	bool ret =
		priv_in_bounds(ndx1-1, sol) &&
		priv_in_bounds(ndx1+1, sol) &&
		priv_in_bounds(ndx2  , sol) &&
		priv_in_bounds(ndx2+1, sol);
	
	return ret;
}

void reschedule_tabu_option::apply(solution* sol) const
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	// twice as many operations as we need...
	int stop = sol->remove_at_ndx(ndx1);
	if (stop != stop1)
	{
		std::cout << "This should never happen..." << std::endl;
		trap();
	}
	
	sol->is_valid();
	
	if (ndx1 < ndx2)
	{
		sol->insert_at_ndx(stop1, ndx2  );
	}
	else
	{
		sol->insert_at_ndx(stop1, ndx2+1);
	}
	
	sol->is_valid();
}









tabu_options::tabu_options(city* c) : swap_options{nullptr}, reschedule_options{nullptr}
{
	int n = c->num_cities;
	for (int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
		{
			if (i==j)
			{
				continue;
			}
			
			reschedule_tabu_option* r = new reschedule_tabu_option{i, j, c, reschedule_options};
			reschedule_options = r;
			
			if (j < i)
			{
				continue;
			}
			
			swap_tabu_option* s = new swap_tabu_option{i, j, c, swap_options};
			swap_options = s;
		}
	}
}
tabu_options::~tabu_options()
{
	auto delop = [](const tabu_option* op)
	{
		while (op != nullptr)
		{
			const tabu_option* c = op;
			op = op->next;
			delete c;
		}
	};
	delop(swap_options);
	delop(reschedule_options);
}

const tabu_option* tabu_options::get_best_option(const solution* sol)
{
	double best_improvement = DBL_MAX;
	const tabu_option* best = nullptr;
	
	if(1)
	{
		const swap_tabu_option* swap = swap_options;
		while (swap != nullptr)
		{
			if (!swap->in_bounds(sol))
			{
				swap = (swap_tabu_option*) swap->next;
				continue;
			}
			
			double improvement = swap->get_improvement(sol);
			if (improvement >= 0)
			{
				swap = (swap_tabu_option*) swap->next;
				continue;
			}
			
			if (improvement < best_improvement)
			{
				best = swap;
				best_improvement = improvement;
			}
			
			swap = (swap_tabu_option*) swap->next;
		}
	}
	
	if(0)
	{
		const reschedule_tabu_option* resched = reschedule_options;
		while (resched != nullptr)
		{
			if (!resched->in_bounds(sol))
			{
				resched = (reschedule_tabu_option*) resched->next;
				continue;
			}
			
			double improvement = resched->get_improvement(sol);
			if (improvement >= 0)
			{
				resched = (reschedule_tabu_option*) resched->next;
				continue;
			}
			
			if (improvement < best_improvement)
			{
				best = resched;
				best_improvement = improvement;
			}
			
			resched = (reschedule_tabu_option*) resched->next;
		}
	}
	
	return best;
}



const tabu_option* tabu_options::get_random_option(const solution* sol)
{
	if (sol->get_num_serviced() < 5)
	{
		return nullptr;
	}
	
	const tabu_option *start = (rand() % 2) ? (tabu_option *)reschedule_options : (tabu_option *)swap_options;
	const tabu_option *ran = start;
	
	const int len = sol->get_city()->num_cities * (sol->get_city()->num_cities - 1) / 2;
	const int num = rand() % len;
	
	for (int i=0; i<num || !ran->in_bounds(sol); i++)
	{
		ran = ran->next;
		if (ran == nullptr)
		{
			ran = start;
		}
	}
	
	return ran;
}


void tabu_options::print_improvements(const solution* sol) const
{
	const tabu_option* resched = reschedule_options;
	auto pri = [sol](const tabu_option* op)
	{
		while (op != nullptr)
		{
			if (op->in_bounds(sol))
			{
				
				if (op->get_improvement(sol) < 0.0)
				{
					std::cout << '1';
				}
				else
				{
					std::cout << ' ';
				}
			}
			else
			{
				std::cout << 'X' << std::endl;
			}
			op = op->next;
		}
		std::cout << std::endl;
	};
	// Recursive functions will cause stack overflows...
//	std::function<void(const tabu_option* op)> f = [&, sol](const tabu_option* op)
//	{
//		if (op == nullptr) { std::cout << std::endl; return; }
//		
//		f(op->next);
//	};
	pri(swap_options);
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

void local_search(solution *sol, std::function<void(void)> callback)
{
	tabu_options options{sol->get_city()};
	
	const tabu_option* o;
	while ((o = options.get_best_option(sol)) != nullptr)
	{
		o->apply(sol);
		callback();
	}
}


void tabu_options::sort(const solution* sol)
{
	bubble_sort(swap_options, sol);
	bubble_sort(reschedule_options, sol);
}

void tabu_options::bubble_sort(const tabu_option* first, const solution* sol)
{
	bool changed = true;
	while (changed)
	{
		changed = false;
		
		const tabu_option* prev = first;
		const tabu_option* curr = first->next;
		const tabu_option* next;
		
		while ((next = curr->next) != nullptr)
		{
			if (curr->get_improvement(sol) > next->get_improvement(sol))
			{
				// p    c     n      n->n
				((tabu_option*) curr)->next = next->next;
				((tabu_option*) prev)->next = next;
				((tabu_option*) next)->next = curr;
				
				changed = true;
				
				// p n c n->n
				prev = next;
				// no need to set curr
			}
			else
			{
				prev = curr;
				curr = next;
			}
		}
	}
}




void tabu_options::apply_all_improvements(solution* sol, std::function<void(void)> callback)
{
	auto apply = [sol, &callback](const tabu_option* op) -> bool
	{
		bool improved = false;
		while(op != nullptr)
		{
			if (op->in_bounds(sol) && op->get_improvement(sol) < 0.0)
			{
				improved = true;
				op->apply(sol);
				callback();
			}
			op = op->next;
		}
		return improved;
	};
	
	bool impr;
	do
	{
		impr = false;
		while (apply(swap_options))       impr = true;
		while (apply(reschedule_options)) impr = true;
	} while (impr);
}

