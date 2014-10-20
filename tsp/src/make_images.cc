/*
 * make_images.cc
 *
 *  Created on: Oct 20, 2014
 *      Author: thallock
 */


#include "viewer.h"
#include "hash.h"
#include "item.h"
//#include <unistd.h>


void make_images()
{
//	srand(500025);
	srand(500022);
//	srand(time(nullptr));

	int n = 15;
	city* c = new city{RADIAL_CITY_TYPE, n};

	init_hash_key(c->num_stops);

	solution sol1{c};
	sol1.random();

	viewer v{"steps"};
	itemizer items{c};

	tabu_list* list = new bloom_filter{1000};

	std::list<solution> anotherlist;

	int count = 0;
	items.greedy_search(&sol1, [&count, &v, &sol1, list, &anotherlist]()
	{
		std::stringstream s;
		s << "output/step_1_" << count++ << ".png";
		std::cout << "Saving image number " << count << std::endl;
		v.snapshot(&sol1, s.str());

		hash h = get_hash(&sol1);
		std::cout << "h="<< h << "=" << h.get_value(97) << " % 97" << std::endl;
		list->mark_tabu(h);

		anotherlist.push_back(solution{sol1});
	});

#if 1
#else
	double mcost = sol1.get_cost();

	int *init = new int[n];
	for (int i = 0; i < n; i++)
	{
		init[i] = i;
	}

	bool hit_tabu = false;
	while (!hit_tabu)
	{
		if (!std::next_permutation(&init[0], &init[n-1]))
		{
			std::cout << "No useful tabu example found!" << std::endl;
			break;
		}
		for (int i = 0; i < n; i++)
		{
			sol1.set_path_at(i, init[i]);
		}
		sol1.assign_serviced_indices();
//		std::cout << "Trying " << sol1 << std::endl;

		count = 0;
		items.greedy_search(&sol1, [&count, &v, &sol1, list, &hit_tabu, &anotherlist, mcost]()
		{
			if (sol1.get_cost() > mcost && count != 0)
			{
//				if (!hit_tabu && list->is_tabu(get_hash(&sol1)))
//				{
//					std::cout << "Hit tabu at " << count << "!!!" << std::endl;
//					hit_tabu = true;
//				}

				int idx = 0;
				auto end = anotherlist.end();
				for (auto it = anotherlist.begin(); it != end; ++it)
				{
					if (idx++ >= (int) anotherlist.size() - 2)
					{
						break;
					}
					if (sol1 == (*it))
					{
						std::cout << "Hit tabu at " << count << "!!!" << std::endl;
						hit_tabu = true;
						std::cout << "cost=" << sol1.get_cost() << std::endl;
//						std::cout << "The hashes did not match, but they were the same!!" << std::endl;
//						trap();
					}
				}
			}

			std::stringstream s;
			s << "output/step_2_" << count++ << ".png";
			std::cout << "Saving image number " << count << std::endl;
			v.snapshot(&sol1, s.str());

			hash h = get_hash(&sol1);
			std::cout << "h="<< h << "=" << h.get_value(97) << " % 97" << std::endl;
		});
	}
#endif

	destroy_hash_key(c->num_stops);

	delete c;
}
