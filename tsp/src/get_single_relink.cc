/*
 * get_single_relink.cc
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


void seed(solution *sol, viewer& v);


int get_single_relink()
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

//	void test_hash_diffs(city* c);
//	test_hash_diffs(c);

	itemizer items{c};


	solution* sol1 = new solution{c};
	viewer v1{"nearest1"};
	seed(sol1, v1);
	items.greedy_search(sol1, [&v1, sol1](){v1.update(sol1);});

	solution* sol2 = new solution{c};
	viewer v2{"nearest2"};
	seed(sol2, v2);
	items.greedy_search(sol2, [&v2, sol2](){v2.update(sol2);});

	solution *traveler = new solution{c};
	(*traveler) = (*sol1);

	viewer tv{"path_between"};
	move_from_to(sol1, sol2, [&tv](const solution* s1)
	{
		tv.update(s1);
		tv.pause(20);
	});




//	levenshtein(sol1, sol2);
//	link_paths(sol1, sol2, items, [&tv](){tv.update();});
//	fastest_descent(sol1, sol2, items, [&tv](){tv.update();});

	destroy_hash_key(c->num_stops);

	delete c;

	return 0;
}
