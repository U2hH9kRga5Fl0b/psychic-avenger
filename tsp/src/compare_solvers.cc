/*
 * compare_solvers.cc
 *
 *  Created on: Oct 19, 2014
 *      Author: thallock
 */


#include "routines.h"
#include "viewer.h"
#include "slvr.h"
#include "subdivider.h"
#include "viewer.h"
#include "item.h"
#include "count_minima.h"
#include "common.h"
#include "path_relinking.h"
#include "intensifier.h"

#include <iostream>
#include <thread>
#include <algorithm>


void seed(solution *sol, viewer& v);


void work_backward(solution*sol, int anneal, itemizer& options, std::function<void(void)> l, int type=3);

namespace
{
#define ANNEAL_FACTOR 10
#define NUM_LOCALS 5
#define RESTART_PROB 10000


void kickoff_multiple_starts(city* c)
{
	itemizer items{c};
	solution sol{c};
	solution best{c};
	viewer v{"multiple_starts"};
	viewer b{"multiple_starts_best"};
	double bcost = DBL_MAX;

//	seed(&sol, v);
	auto callback = [&v, &sol]() {v.update(&sol);};
	for (;;)
	{
		seed(&sol, v);
//		sol.random();
//		work_backward(&sol, sol.get_city()->num_stops/3, items, callback, 2);
//		items.greedy_search(&sol, callback);

		for (int i = 0; i < RESTART_PROB; i++)
		{
			work_backward(&sol, ANNEAL_FACTOR, items, callback);
			items.greedy_search(&sol, callback);
			double cost = sol.get_cost();
			if (cost < bcost)
			{
				b.update(&sol);
				bcost = cost;
				best = sol;
			}

			sol = best;
		}
	}
}

void kickoff_relink(city* c)
{
	itemizer items{c};
	solution sol{c};

	viewer v{"path_relink"};
	viewer b{"path_relink_best"};
	double bcost = DBL_MAX;

//
	auto callback = [&v, &sol]() {v.update(&sol);};
	auto updater = [&b, &bcost](const solution* s1)
	{
		double cost = s1->get_cost();
		if (cost < bcost)
		{
			b.update(s1);
			bcost = cost;
		}
	};

	tabu_list* empty = new empty_list;
	intensifier inte { c, empty, NUM_LOCALS, updater };

	for (;;)
	{
		seed(&sol, v);
		items.ordered_search(&sol, callback);
//		sol.random();
//		work_backward(&sol, sol.get_city()->num_stops/3, items, callback, 2);
//		items.greedy_search(&sol, callback);

		for (int i = 0; i < RESTART_PROB; i++)
		{
			do
			{
				work_backward(&sol, ANNEAL_FACTOR, items, callback);
			} while (!order_tabu_search(&sol, items, empty, updater));
			inte.discovered(&sol);
		}
	}

}

void kickoff_tabu_relink(city* c)
{
	itemizer items{c};
	solution sol{c};

	viewer v{"tabu_relink"};
	viewer b{"tabu_relink_best"};
	double bcost = DBL_MAX;

//	seed(&sol, v);
	auto callback = [&v, &sol]() {v.update(&sol);};
	auto updater = [&b, &bcost](const solution* s1)
	{
		double cost = s1->get_cost();
		if (cost < bcost)
		{
			b.update(s1);
			bcost = cost;
		}
	};

//	circular_hash_set* blm = new circular_hash_set{100000};
	bloom_filter* blm = new bloom_filter{100000};
	intensifier inte{c, blm, NUM_LOCALS, updater};

	for (;;)
	{
		seed(&sol, v);
		items.ordered_search(&sol, callback);
//		sol.random();
//		work_backward(&sol, sol.get_city()->num_stops/3, items, callback, 2);
//		items.greedy_search(&sol, callback);

		for (int i = 0; i < RESTART_PROB; i++)
		{
			do
			{
				work_backward(&sol, ANNEAL_FACTOR, items, callback);
			} while (!order_tabu_search(&sol, items, blm, updater));
			std::cout << "stop % = " << blm->get_stop_percent() << std::endl;
			inte.discovered(&sol);
		}

		if (blm->get_stop_percent() > .5)
		{
			blm->erase_memory();
		}
		blm->refresh_stats();
	}
}

}

int compare_different_solvers()
{
	srand(5000016);
//	srand(time(nullptr));
	city* c;

	{
		std::ifstream infile {"city.txt"};
		if (!infile.is_open())
		{
			std::cout << "Unable to open city file!" << std::endl;
		}
		c = load_city(infile);
		if (c == nullptr)
		{
			return -1;
		}
	}

	init_hash_key(c->num_stops);


	std::thread t1{kickoff_multiple_starts, c};
	std::thread t2{kickoff_relink, c};
	std::thread t3{kickoff_tabu_relink, c};


	t1.join();
	t2.join();
	t3.join();

	destroy_hash_key(c->num_stops);

	delete c;

	return 0;
}
