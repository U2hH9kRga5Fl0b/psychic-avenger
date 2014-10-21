/*
 * save_graph_of_graph.cc
 *
 *  Created on: Oct 20, 2014
 *      Author: thallock
 */

#include "item.h"
#include "viewer.h"

namespace
{


int count;

void dfs(solution* s, itemizer& items, viewer& v, int cdepth, int maxdepth)
{
	if (cdepth >= maxdepth)
	{
		return;
	}

	int num_options = items.get_num_options();
	for (int i=0;i<3;i++)
	{
		const neighbor_option* op = items.get_option(rand() % num_options);
		solution tmp {*s};
		op->apply(&tmp);
		std::stringstream s;
		s << "output/dist_" << cdepth << "_" << count++ << ".png";
		v.snapshot(&tmp, s.str());

		dfs(&tmp, items, v, cdepth + 1, maxdepth);
	}
}



}


void save_graph_of_graph()
{
	srand(time(nullptr));
	int n = 15;
	city* c = new city{RADIAL_CITY_TYPE, n};


	solution sol {c};
	sol.nearest();
	itemizer items{c};
	viewer v{"neighbors"};

	v.snapshot(&sol, "output/starting.png");

	dfs(&sol, items, v, 0, 2);

	delete c;
}
