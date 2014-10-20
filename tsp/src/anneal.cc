
#include "routines.h"
#include "tabu_list.h"
#include "viewer.h"
#include "item.h"

void anneal_s(solution *sol, int convergence_threshold, double accept_negative_prob)
{
	itemizer options{sol->get_city()};
	viewer v{"2opts+resched anneal"};
	
	std::cout << "P(hurt myself) = " << accept_negative_prob << std::endl;
	
	double min_cost = DBL_MAX;
	int count = 0;
	while (count++ < convergence_threshold)
	{
		const neighbor_option* option = options.get_random_option(sol);
		if (option == nullptr)
		{
			std::cout << "No more options" << std::endl;
			return;
		}
		
		double improvement = option->get_improvement(sol);
		if (improvement < 0 && accept_negative_prob < (rand() / (double) RAND_MAX))
		{
			continue;
		}
		
		option->apply(sol);
		if (sol->get_cost() < min_cost)
		{
			count = 0;
		}
		
		v.update(sol);
	}
}

constexpr	double grow_anneal = .9;

void work_backward(solution*sol, int anneal, itemizer& options, std::function<void(void)> l, int type=3);
void work_backward(solution*sol, int anneal, itemizer& options, std::function<void(void)> l, int type)
{
	switch(type)
	{
		case 0:
		{
			std::cout << "randomizing" << std::endl;
			for (int i=0;i<anneal;i++)
			{
				const neighbor_option* option = options.get_random_option(sol);
				if (option == nullptr)
				{
					std::cout << "No more options" << std::endl;
					return;
				}
				option->apply(sol);
				l();
			}
		}
		break;
		case 1:
		{
			std::cout << "removing" << std::endl;
			int start = rand() % (sol->get_city()->num_stops - anneal + 1);
			for (int i=0; i<anneal; i++)
			{
				sol->remove_at_ndx(start);
				l();
			}
		}
		break;
		case 2:
		{
			std::cout << "removing" << std::endl;
			for (int i=0; i<anneal; i++)
			{
				int stop;
				int idx;
				do
				{
					stop = rand() % sol->get_city()->num_stops;
				} while ((idx = sol->get_index_of_stop(stop)) < 0);
				
				sol->remove_at_ndx(idx);
				l();
			}
		}
		break;
		case 3:
		{
//			std::cout << "applying good bad options..." << std::endl;
			options.sample_anneal(sol, anneal, 10, l);
		}
		break;
	}
	
	if (type == 3 || type == 0)
	{
		int size = sol->length();
		int mid = (sol->get_city()->num_stops + size) / 2;
		grow(sol, mid, l, grow_anneal);
		options.ordered_search(sol, l);
		grow(sol, l, grow_anneal);
	}
	else
	{
		sol->nearest(l);
	}
}

void anneal(solution *sol, int convergence_threshold, double tol, int start_anneal, int dec)
{
	itemizer options{sol->get_city()};
	viewer v{"2opts+resched anneal"};
	auto l = [&sol, &v](){v.update(sol);};
	
	sol->empty();
	grow(sol, l, grow_anneal);
	
	solution* best = new solution{sol->get_city()};	
	best = sol;
	double bcost = best->get_cost();
	
	
	bloom_filter blm{500};
	
	for (int anneal = start_anneal; anneal>0; anneal-= dec)
	{
		int count = 0;
		int conv = 0;
		
		while(conv++ < convergence_threshold)
		{
			count++;
			(*sol) = (*best);
			
			work_backward(sol, anneal, options, l);
			
			if (0)
			{
				std::cout << "neighbor search" << std::endl;
				options.ordered_search(sol, l);
			}
			else
			{
				std::cout << "tabu search" << std::endl;
				options.ordered_tabu_search(sol, blm, 100, l);
				
				std::cout << "stop % = " << blm.get_stop_percent() << std::endl;
				blm.refresh_stats();
			}
			
			double ncost = sol->get_cost();
			double reldiff = (bcost - ncost) / bcost;
			std::cout << "relative diff = " << reldiff << " to " << ncost << " from " << bcost << std::endl;
			
			if (ncost < bcost)
			{
				(*best) = (*sol);
				bcost = ncost;
				
				if (reldiff > tol)
				{
					conv = 0;
				}
			}
		}
		
		std::cout << "Anneal = " << anneal << " converged in " << count << " iterations." << std::endl;
	}
	
	(*sol) = (*best);
}

