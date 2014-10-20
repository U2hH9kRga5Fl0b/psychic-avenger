#include "item.h"

#include <float.h>

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
//		std::cout << "new cost: " << sol->get_cost() << std::endl;
		callback();
	}
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


void itemizer::hybrid_search(solution* sol, double percent, std::function<void(void)> callback)
{
	// perform greedy
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
	ordered_search(sol, callback);
}

void itemizer::ordered_tabu_search(solution* sol, bloom_filter& tabs, int threshold, std::function<void(void)> callback)
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
		
		for (int i=0; i<num_options; i++)
//		for (int r=0;r<1000;r++)
		{
//			int i = rand() % num_options;
//			std::cout << std::dec << i << " of " << num_options << std::endl;
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
			
			if (tabs.is_tabu(op->get_hash(sol)))
			{
				std::cout << "s";
				continue;
			}
			std::cout << "k";
			best_improvement = imp;
			best = op;
		}
		std::cout << "\n ...done" << std::endl;
		std::cout.flush();
		
		if (best == nullptr)
		{
			break;
		}
		
		best->apply(sol);
		callback();
		tabs.mark_tabu(get_hash(sol));
		
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
		tabs.mark_tabu(get_hash(sol));
	}
}

