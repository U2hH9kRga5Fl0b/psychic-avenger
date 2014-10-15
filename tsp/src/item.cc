#include "item.h"

#include "geometry.h"
#include "common.h"

#include <algorithm>


#include <float.h>

neighbor_option::neighbor_option(int stop1_, int stop2_, city* c) : 
	stop1{stop1_},
	stop2{stop2_},
	dist{c->get_cost(stop1_,stop2_)}
	{}

neighbor_option::~neighbor_option()
{
	// Does this causes a stack overflow for large TSP problems:
	// delete next;
	// Instead, there is a while loop in the itemizer class.
}

swap_option::swap_option(int stop1, int stop2, city* c) : neighbor_option{stop1, stop2, c} {}
swap_option::~swap_option() {}

resched_option::resched_option(int stop1, int stop2, city* c) : neighbor_option{stop1, stop2, c} {}
resched_option::~resched_option() {}


namespace
{
	bool priv_in_bounds(int ndx, const solution* sol)
	{
		return ndx >= 0 && ndx < sol->get_city()->num_stops && sol->get_stop(ndx) >= 0;
	}
}

double swap_option::get_cost_along_path(const solution* sol) const
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


double swap_option::get_improvement(const solution* sol) const
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
	
	bool has_next_point = ndx2 + 1 < c->num_stops && sol->get_stop(ndx2+1) >= 0;
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

bool swap_option::in_bounds(const solution* sol) const
{
	return sol->get_index_of_stop(stop1) >= 0 &&
	       sol->get_index_of_stop(stop2) >= 0;
}

bool swap_option::intersects(const solution* sol) const
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


void swap_option::apply(solution* sol) const
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




double resched_option::get_improvement(const solution* sol) const
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

bool resched_option::in_bounds(const solution* sol) const
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

void resched_option::apply(solution* sol) const
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


int resched_option::map(const solution* sol, int ndx) const
{
	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	
	
}







itemizer::itemizer(city* c)
{
	int n = c->num_stops;
	num_options = 0;
	for (int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
		{
			if (i==j)
			{
				continue;
			}
			
			num_options++;
			
			if (j < i)
			{
				continue;
			}
			
			num_options++;
		}
	}
	
	options = new neighbor_option*[num_options];
	
	int ndx = 0;
	for (int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
		{
			if (i==j)
			{
				continue;
			}
			
			options[ndx++] = new resched_option{i, j, c};
			
			if (j < i)
			{
				continue;
			}
			
			options[ndx++] = new swap_option{i, j, c};
		}
	}
}
itemizer::~itemizer()
{
	for (int i=0; i<num_options; i++)
	{
		delete options[i];
	}
	delete[] options;
}

const neighbor_option* itemizer::get_best_option(const solution* sol)
{
	return get_nth_best_option(sol, 0);
}

const neighbor_option* itemizer::get_nth_best_option(const solution* sol, int n, bool hurt)
{
	n = std::max(1, n);
	
	double best_improvement = DBL_MAX;
	const neighbor_option* best = nullptr;
	
	std::vector<const neighbor_option*> better;
	
	if (num_options < n)
	{
		std::cout << "Only " << num_options << " options, can't get the " << n << "-th best!" << std::endl;
		trap();
	}
	
	double* cs = new double[n];
	const neighbor_option** b = new const neighbor_option*[n];
	
	for (int i=0;i<n;i++)
	{
		b[i] = nullptr;
	}
	
	for (int i=0; i<num_options; i++)
	{
		const neighbor_option* opt = options[i];
		if (!opt->in_bounds(sol))
		{
			continue;
		}
		
		double imp = opt->get_improvement(sol);
		if (hurt && imp < 0.0)
		{
			continue;
		}
		
		if (b[0] != nullptr && cs[0] < imp)
		{
			continue;
		}
		
		
		for (int j=0; ; j++)
		{
			if (j == n-1)
			{
				cs[j] = imp;
				b[j] = opt;
				break;
			}
			
			if (b[j+1] == nullptr || cs[j+1] > imp)
			{
				cs[j] = cs[j+1];
				b[j] = b[j+1];
				continue;
			}
			
			cs[j] = imp;
			b[j] = opt;
		}
	}
	
	const neighbor_option* ret = nullptr;
	for (int i=0; i<n; i++)
	{
		if (b[i] != nullptr)
		{
			ret = b[i];
			
			break;
		}
	}
	
	delete[] cs;
	delete[] b;
	
	return ret;
}


const neighbor_option* itemizer::get_best_of_sample(const solution* sol, int nsamples)
{
	nsamples = std::max(1, nsamples);
	
	const neighbor_option* best = nullptr;
	double best_cost = DBL_MAX;
	for (int i=0; i<nsamples; i++)
	{
		const neighbor_option* alt;
		
		do
		{
			alt = options[rand() % num_options];
		} while (!alt->in_bounds(sol));
		
		double c = alt->get_improvement(sol);
		if (c < best_cost)
		{
			best_cost = c;
			best = alt;
		}
	}
	return best;
}


const neighbor_option* itemizer::get_random_option(const solution* sol)
{
	if (sol->get_num_serviced() < 5)
	{
		return nullptr;
	}
	
	// Could run through these linearly from a random starting point...
	const neighbor_option* op = nullptr;
	do
	{
		op = options[rand() % num_options];
	} while (!op->in_bounds(sol));

	return op;
}


void itemizer::print_improvements(const solution* sol) const
{
	for (int i=0; i<num_options; i++)
	{
		const neighbor_option* op = options[i];
		if (!op->in_bounds(sol))
		{
			std::cout << 'X';
			continue;
		}
		if (op->get_improvement(sol) < 0.0)
		{
			std::cout << '1';
		}
		else
		{
			std::cout << ' ';
		}
	}
	std::cout << std::endl;
}


std::ostream& operator<<(std::ostream& out, const neighbor_option& op)
{
	return out << "[t=" << op.get_name() << "][s1=" << op.stop1 << "][s2=" << op.stop2 << "]";
}

std::string swap_option::get_name() const
{
	return "swap";
}

std::string resched_option::get_name() const
{
	return "resched";
}

void itemizer::sort(const solution* sol)
{
	std::sort(options, options + num_options, [sol](const neighbor_option* op1, const neighbor_option* op2)
	{
		return op1->get_improvement(sol) < op2->get_improvement(sol);
	});
}

void itemizer::ordered_search(solution* sol, std::function<void(void)> callback)
{
	bool improved;
	do
	{
		improved = false;
		for (int i=0;i<num_options;i++)
		{
			const neighbor_option* op = options[i];
			
			if (!op->in_bounds(sol))
			{
				continue;
			}
			if (op->get_improvement(sol) >= 0.0)
			{
				continue;
			}
			
			improved = true;
			op->apply(sol);
			callback();
		}
	} while (improved);
}


void itemizer::nth_anneal(solution* sol, int num_steps, int range, std::function<void(void)> callback)
{
	range = std::max(range, 1);
	for (int i=0; i<num_steps; i++)
	{
		const neighbor_option* op;
		do
		{
			op = get_nth_best_option(sol, rand() % range, true);
		} while (op == nullptr);
		op->apply(sol);
		callback();
	}
}

void itemizer::sample_anneal(solution* sol, int num_steps, int nsamples, std::function<void(void)> callback)
{
	nsamples = std::max(nsamples, 1);
	for (int i=0; i<num_steps; i++)
	{
		const neighbor_option* op;
		do
		{
			op = get_best_of_sample(sol, nsamples);
		} while (op == nullptr);
		op->apply(sol);
		callback();
	}
}

void itemizer::greedy_search(solution *sol, std::function<void(void)> callback)
{
	const neighbor_option* o;
	while ((o = get_best_option(sol)) != nullptr)
	{
		if (o->get_improvement(sol) >= 0.0)
		{
			break;
		}
		o->apply(sol);
		std::cout << "new cost: " << sol->get_cost() << std::endl;
		callback();
	}
}


void itemizer::hybrid_search(solution* sol, double percent, std::function<void(void)> callback)
{
	// performed greedy
	int count_improvements;
	do
	{
		const neighbor_option* best = nullptr;
		double best_improvement = DBL_MAX;
		
		count_improvements = 0;
		
		for (int i=0; i<num_options; i++)
		{
			const neighbor_option* op = options[i];
			
			if (!op->in_bounds(sol))
			{
				continue;
			}
			double imp = op->get_improvement(sol);
			if (imp < 0.0)
			{
				count_improvements++;
			}
			if (imp > best_improvement)
			{
				continue;
			}
			
			best_improvement = imp;
			best = op;
		}
		
		if (best == nullptr)
		{
			break;
		}
		
		best->apply(sol);
	} while ((count_improvements / (double) num_options) > percent);
	
	
	// perform ordered
	bool improved;
	do
	{
		improved = false;
		for (int i=0;i<num_options;i++)
		{
			const neighbor_option* op = options[i];
			
			if (!op->in_bounds(sol))
			{
				continue;
			}
			if (op->get_improvement(sol) >= 0.0)
			{
				continue;
			}
			
			improved = true;
			op->apply(sol);
			callback();
		}
	} while (improved);
}

void itemizer::ordered_tabu_search(solution* sol, tabu_list& tabs, int threshold, std::function<void(void)> callback)
{
	// perform ordered
	double global_best_cost = sol->get_cost();
	
	int count = 0;
	do
	{
		const neighbor_option *best = nullptr;
		double best_improvement = DBL_MAX;
		
		std::cout << "testing all options...";
		std::cout.flush();
		
//		for (int i=0; i<num_options; i++)
		for (int r=0;r<1000;r++)
		{
			int i = rand() % num_options;
			//std::cout << std::dec << i << " of " << num_options << std::endl;
			const neighbor_option* op = options[i];
			
			if (!op->in_bounds(sol))
			{
				continue;
			}
			
			double imp = op->get_improvement(sol);
			if (imp >= best_improvement)
			{
				continue;
			}
			
			solution copy{sol->get_city()};
			copy = (*sol);
			op->apply(&copy);
			if (tabs.is_tabu(&copy, false))
			{
				continue;
			}
			
			best_improvement = imp;
			best = op;
		}
		std::cout << " ...done" << std::endl;
		std::cout.flush();
		
		if (best == nullptr)
		{
			break;
		}
		
		best->apply(sol);
		callback();
		tabs.is_tabu(sol, true);
		
		if (sol->get_cost() < global_best_cost)
		{
			count = 0;
		}
		
		sol->is_valid();
	} while(count++ < threshold);
	
	const neighbor_option* o;
	while ((o = get_best_option(sol)) != nullptr)
	{
		std::cout << "ncost = " << o->get_improvement(sol) << std::endl;
		if (o->get_improvement(sol) >= 0.0)
		{
			break;
		}
		o->apply(sol);
		callback();
		tabs.is_tabu(sol, true);
	}
}

