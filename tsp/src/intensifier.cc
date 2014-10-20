/*
 * intensifier.cc
 *
 *  Created on: Oct 19, 2014
 *      Author: thallock
 */

#include "intensifier.h"
#include "viewer.h"


#include <sstream>

bool order_tabu_search(solution* sol, itemizer& items, tabu_list* list, std::function<void(const solution* s)> callback)
{
	int num_options = items.get_num_options();
//	std::stringstream s;
//	s << "optimizing_middle_path_" << rand() << std::endl;
//	viewer v{s.str()};

	bool improved;
	do
	{
		improved = false;

		for (int i=0; i<num_options; i++)
		{
			const neighbor_option* op = items.get_option(i);
			if (!op->in_bounds(sol))
			{
				continue;
			}

			double imp = op->get_improvement(sol);
			if (imp >= 0.0)
			{
				continue;
			}

			if (list->is_tabu(op->get_hash(sol)))
			{
//				std::cout << "hit tabu" << std::endl;
				return false;
			}

			op->apply(sol);
//			v.update(sol);
			list->mark_tabu(get_hash(sol));
		}
	} while(improved);

	callback(sol);
	return true;
}

intensifier::intensifier(city* c, tabu_list* list_, int max_length_, std::function<void(const solution*sol)> callback_) :
	items{c},
	list{list_},
	population{new solution*[max_length_]},
	tmp{c},
	max_length{max_length_},
	f{[this, callback_](const solution* middle)
	{
		if (rand() % 10)
		{
			return;
		}
		this->tmp = (*middle);
		order_tabu_search(&this->tmp, this->items, this->list, callback_);
	}}
{
	for (int i=0;i<max_length; i++)
	{
		population[i] = nullptr;
	}
}

intensifier::~intensifier()
{
	for (int i=0;i<max_length;i++)
	{
		delete population[i];
	}
	delete population;
	delete list;
}

void intensifier::discovered(solution* local_minima)
{
	int closestNdx = -1;
	int closest = INT_MAX;

	for (int i=0; i<max_length; i++)
	{
		if (population[i] == nullptr)
		{
			population[i] = new solution{*local_minima};
			return;
		}
		move_from_to(local_minima, population[i], f);

		int d = distance_between_sequence_no_order(population[i], local_minima);
		if (d < closest)
		{
			closest = d;
			closestNdx = i;
		}
	}

	(*population[closestNdx]) = (*local_minima);

}
