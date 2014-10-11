
#include "routines.h"

#include "slvr.h"
#include "permutation.h"

#include <algorithm>

namespace 
{
	void exact_solution(optimizer& s, int start_index, int stop_index)
	{
		permutation p{&s.current, start_index, stop_index};
		
		do
		{
			p.assign(&s.current);
//			std::cout << "Trying ";
//			p.print_to(std::cout);
			s.current.is_valid();
			s.offer();
		} while (p.increment());
	}
}


solution* exact(city *c)
{
	optimizer opt{"exact", c, 100};
	
	opt.current.random();
	opt.best.empty();
	
	exact_solution(opt, 0, c->num_cities);
	
	solution* ret = new solution{c};
	(*ret) = opt.best;
	return ret;
}
