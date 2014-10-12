#include "viewer.h"
#include "tabu.h"
#include "common.h"

void test_option(const tabu_option* option, const solution* origin)
{
	solution another{origin->get_city()};
	another = (*origin);
	solution* sol = &another;
	
	double improvement = option->get_improvement(sol);
	
//	std::cout << "for sol" << *sol << " it appears that the best option is \n" << *option << std::endl;
//	std::cout << "The improvement is " << improvement << std::endl;
//	v.update();
//	v.pause();
		
	double old_cost = sol->get_cost();
	option->apply(sol);
	double new_cost = sol->get_cost();
	
//		std::cout << "And leaves: " << *sol << std::endl;
	
	if (abs(new_cost - old_cost - improvement) > 1e-14)
	{
		std::cout << "The improvement function needs some work. actual improvement=" << (new_cost - old_cost)
			<< " calculated improvement=" << improvement << std::endl;
		trap();
	}

	sol->is_valid();
}


void test_local_search(solution *sol, int tests_per_phase)
{
	tabu_options options{sol->get_city()};
//	viewer v{sol, name, 0};
	
	for(int j=0;j<3;j++)
	{
		const tabu_option* best = options.get_best_option(sol);
		if (best == nullptr)
		{
			std::cout << "found local minima" << std::endl;
			return;
		}
		
		for(int i=0; i<tests_per_phase; i++)
		{
			const tabu_option* ran = options.get_random_option(sol);
			test_option(ran, sol);
			if (ran->get_improvement(sol) < best->get_improvement(sol))
			{
				std::cout << "Best is not best!" << std::endl;
				trap();
			}
		}
		
		best->apply(sol);
		sol->is_valid();
		
		std::cout << "done with iteration" << std::endl;
		std::cout << "cost now = " << sol->get_cost() << std::endl;
	}
}
