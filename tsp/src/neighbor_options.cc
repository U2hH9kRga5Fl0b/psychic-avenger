/*
 * neighbor_options.cc
 *
 *  Created on: Oct 16, 2014
 *      Author: thallock
 */


#include "neighbor_options.h"

#include <float.h>
#include "routines.h"

neighbor_option::neighbor_option(int stop1_, int stop2_, city* c) :
	stop1{stop1_},
	stop2{stop2_},
	dist{c==nullptr?0:c->get_cost(stop1_,stop2_)}
	{}

neighbor_option::~neighbor_option()
{
	// Does this causes a stack overflow for large TSP problems:
	// delete next;
	// Instead, there is a while loop in the itemizer class.
}

two_opt::two_opt(int stop1, int stop2, city* c) : neighbor_option{stop1, stop2, c} {}
two_opt::~two_opt() {}

resched_option::resched_option(int stop1, int stop2, city* c) : neighbor_option{stop1, stop2, c} {}
resched_option::~resched_option() {}


namespace
{
	bool priv_in_bounds(int ndx, const solution* sol)
	{
		return ndx >= 0 && ndx < sol->get_city()->num_stops && sol->get_stop(ndx) >= 0;
	}
}

double two_opt::get_cost_along_path(const solution* sol) const
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


double two_opt::get_improvement(const solution* sol) const
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

bool two_opt::in_bounds(const solution* sol) const
{
	return sol->get_index_of_stop(stop1) >= 0 &&
	       sol->get_index_of_stop(stop2) >= 0;
}

bool two_opt::intersects(const solution* sol) const
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


void two_opt::apply(solution* sol) const
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
		sol->swap(ndx1, ndx2);
	}

	sol->is_valid();
}




double resched_option::get_improvement(const solution* sol) const
{
	city* ci = sol->get_city();
	int n = ci->num_stops;

	bool hasc = stop2 >= 0;

	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = hasc ? sol->get_index_of_stop(stop2) : -1;

	bool hasa = ndx1 > 0;
	bool hasb = ndx1 < n-1;
	bool hasd = ndx2 < n-1;

	int a = hasa ? sol->get_stop(ndx1-1) : -1;
	int s = sol->get_stop(ndx1  ); // aka stop1
	int b = hasb ? sol->get_stop(ndx1+1) : -1;
	int c = sol->get_stop(ndx2  ); // aka stop2
	int d = hasd ? sol->get_stop(ndx2+1) : -1;

	double pc = 0;
	double nc = 0;

	if (ndx1 == ndx2 + 1)
	{
		return 0.0;
	}
	else if (ndx1 + 1 == ndx2)
	{
		// alternative 1: ...     a   s   c   d
		// alternative 2: ...     a   c   s   d


		pc += ci->get_cost(s,c);
		nc += ci->get_cost(c,s);

		if (hasa)
		{
			pc += ci->get_cost(a,s);
			nc += ci->get_cost(a,c);
		}
		if (hasd)
		{
			pc += ci->get_cost(c,d);
			nc += ci->get_cost(s,d);
		}
	}
	else
	{
		// alternative 1: ...     a   s   b     ...     c       d ...
		// alternative 2: ...     a       b     ...     c   s   d ...


		if (hasa && hasb)
		{
			pc += ci->get_cost(a,s);
			pc += ci->get_cost(s,b);

			nc += ci->get_cost(a,b);
		}
		else if (hasa)
		{
			pc += ci->get_cost(a,s);
		}
		else if (hasb)
		{
			pc += ci->get_cost(s,b);
		}

		if (hasd && hasc)
		{
			pc += ci->get_cost(c,d);

			nc += ci->get_cost(c,s);
			nc += ci->get_cost(s,d);
		}
		else if (hasc)
		{
			nc += ci->get_cost(c,s);
		}
		else if (hasd)
		{
			nc += ci->get_cost(s,d);
		}
	}

	return nc - pc;
}

bool resched_option::in_bounds(const solution* sol) const
{
	return			priv_in_bounds(sol->get_index_of_stop(stop1), sol) &&
		(stop2 == -1 || priv_in_bounds(sol->get_index_of_stop(stop2), sol));
}

void resched_option::apply(solution* sol) const
{
	if (stop2 == -1)
	{
		sol->remove_at_ndx(sol->get_index_of_stop(stop1));
		sol->insert_at_ndx(stop1, 0);
		return;
	}

	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);

	if (ndx1 == ndx2 + 1)
	{
		return;
	}

	// twice as many operations as we need...
	sol->remove_at_ndx(ndx1);
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



hash resched_option::get_hash(const solution* sol) const
{
	int n = sol->get_city()->num_stops;

	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = stop2 >= 0 ? sol->get_index_of_stop(stop2) : -1;

	hash h;

	if (stop2 < 0)
	{
		h.set_component(0, stop1);
		for (int i=0; i<ndx1; i++)
		{
			h.set_component(i+1, sol->get_stop(i));
		}

		for (int i=ndx1+1; i<n; i++)
		{
			h.set_component(i, sol->get_stop(i));
		}
		return h;
	}

	if (ndx1 < ndx2)
	{
		// 0 1 2 3 ...         ndx1-1 ndx1   ndx1+1 ... ndx2-1   ndx2      ndx2+1 .... n
		// 0 1 2 3 ...         ndx1-1 ndx1+1 ndx1+2 ... ndx2     ndx1      ndx2+1 .... n

		for (int i=0; i<ndx1; i++)
		{
			h.set_component(i, sol->get_stop(i));
		}

		for (int i=ndx1; i<ndx2; i++)
		{
			h.set_component(i, sol->get_stop(i+1));
		}
		int i = ndx2;
		h.set_component(i, sol->get_stop(ndx1));

		for (int i=ndx2+1; i<n; i++)
		{
			h.set_component(i, sol->get_stop(i));
		}
	}
	else // ndx2 == minidx
	{
		// 0 1 2 3 ...         ndx2-1 ndx2   ndx2+1   ndx2+2 ... ndx1-1   ndx1      ndx1+1 .... n
		// 0 1 2 3 ...         ndx2-1 ndx2   ndx1     ndx2+1 ... ndx1-2   ndx1-1    ndx1+1 .... n

		for (int i=0; i<=ndx2; i++)
		{
			h.set_component(i, sol->get_stop(i));
		}

		h.set_component(ndx2+1, sol->get_stop(ndx1));

		for (int i=ndx2+2;i<=ndx1;i++)
		{
			h.set_component(i, sol->get_stop(i-1));
		}

		for (int i=ndx1+1; i<n; i++)
		{
			h.set_component(i, sol->get_stop(i));
		}
	}

	return h;
}

hash two_opt::get_hash(const solution* sol) const
{
	int n = sol->get_city()->num_stops;

	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	if (ndx2 < ndx1)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}

	hash h;

	for (int i=0; i<ndx1; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}
	for (int i=ndx1; i<=ndx2; i++)
	{
		h.set_component(i, sol->get_stop(ndx2 + ndx1 - i));
	}
	for (int i=ndx2+1; i<n; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}

	return h;
}






// This is the same thing as the resched option...
swap_option::swap_option(int stop1, int stop2, city* c) :
		neighbor_option{stop1, stop2, c}
{
}

swap_option::~swap_option()
{
}

double swap_option::get_improvement(const solution* sol) const
{
	int idx1 = sol->get_index_of_stop(stop1);
	int idx2 = sol->get_index_of_stop(stop2);

	if (idx1 > idx2)
	{
		int tmp = idx1;
		idx1 = idx2;
		idx2 = tmp;
	}

	const city* ci = sol->get_city();
	const int n = ci->num_stops;
	// there is always a c and d.
	const bool hasa = idx1 > 0                                                ; //
	const bool hasc = (idx1 < n-1 && sol->get_stop(idx1+1) >= 0)   || false   ; // there is always a c and d.
	const bool hasd = idx2 > 0                                     || false   ; //
	const bool hasf = (idx2 < n-1 && sol->get_stop(idx2+1) >= 0)              ; //

	const int a = hasa ? sol->get_stop(idx1-1) : -1;
	const int b = sol->get_stop(idx1); // These might have been swapped in order to make idx1 < idx2
	const int c = hasc ? sol->get_stop(idx1+1) : -1;
	const int d = hasd ? sol->get_stop(idx2-1) : -1;
	const int e = sol->get_stop(idx2); // These might have been swapped in order to make idx1 < idx2
	const int f = hasf ? sol->get_stop(idx2+1) : -1;

	double oc = 0.0;
	double nc = 0.0;

	if (hasa)
	{
		oc += ci->get_cost(a, b);
		nc += ci->get_cost(a, e);
	}
	if (hasf)
	{
		oc += ci->get_cost(e, f);
		nc += ci->get_cost(b, f);
	}

	if (idx1 + 1 == idx2)
	{
		// a b e f
		// a e b f

		oc += ci->get_cost(b, e);
		nc += ci->get_cost(e, b);
	}
	else
	{
		//  a b c .... d e f ...
		//  a e c .... d b f ...

		if (hasc)
		{
			oc += ci->get_cost(b, c);
			nc += ci->get_cost(e, c);
		}
		if (hasd)
		{
			oc += ci->get_cost(d, e);
			nc += ci->get_cost(d, b);
		}
	}

	return nc - oc;
}

bool swap_option::in_bounds(const solution* sol) const
{
	return priv_in_bounds(stop1, sol) && priv_in_bounds(stop2, sol);
}

void swap_option::apply(solution* sol) const
{
	sol->swap(sol->get_index_of_stop(stop1),
			sol->get_index_of_stop(stop2));
}

hash swap_option::get_hash(const solution* sol) const
{
	int n = sol->get_city()->num_stops;

	int ndx1 = sol->get_index_of_stop(stop1);
	int ndx2 = sol->get_index_of_stop(stop2);
	if (ndx2 < ndx1)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}

	hash h;

	for (int i=0; i<ndx1; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}

	h.set_component(ndx1, sol->get_stop(ndx2));

	for (int i=ndx1+1; i<ndx2; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}

	h.set_component(ndx2, sol->get_stop(ndx1));

	for (int i=ndx2+1; i<n; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}

	return h;

}

std::string swap_option::get_name() const
{
	return "swap";
}


#if 0
three_opt::three_opt(int stop1, int stop2, int stop3)
{

	// move everything from indx1 to index2 to after index3
}

three_opt::~three_opt()
{
}

double three_opt::get_improvement(const solution* sol) const
{




}

bool three_opt::in_bounds(const solution* sol) const
{
}

void three_opt::apply(solution* sol) const
{
	int n = sol->get_city()->num_stops;
	int *tmp = new int[n];
//	for (int i=0;i<)
}

hash three_opt::get_hash(const solution* sol) const
{

}

std::string three_opt::get_name() const
{
	return "three-opt";
}
#endif
