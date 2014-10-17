#include "solution.h"
#include "item.h"
#include "limits.h"
#include "viewer.h"


int distance_between_positioned(solution* sol1, solution *sol2)
{
	 int sum = 0;

	 const int n = sol1->get_city()->num_stops;

	 for (int i=0; i<n; i++)
	 {
		int stop1 = sol1->get_stop(i);
		int stop2 = sol2->get_stop(i);
		if (stop1 == stop2)
		{
			sum++;
		}
	 }

	 return sum;
}

int distance_between_sequence(solution* sol1, solution *sol2)
{
	 int sum = 0;
	 
	 const int n = sol1->get_city()->num_stops;
	 
	 for (int i=0; i<n-1; i++)
	 {
		int stop1 = sol1->get_stop(i  );
		int stop2 = sol1->get_stop(i+1);
		 
		if (stop2 < 0)
		{
			 break;
		}
		 
		int ndx = sol2->get_index_of_stop(stop1);
		if (ndx < 0 || ndx >= n-1)
		{
			 sum++;
			 continue;
		}
		
		if (sol2->get_stop(ndx+1) != stop2)
		{
			sum++;
		}
	 }
	 
	 return sum;
}



class operation_path
{
public:
	operation_path(const neighbor_option *op_, operation_path *n_) :
		op {op_},
		next{n_} {}

	~operation_path()
	{
		delete next;
	}

	void print()
	{




	}

	const neighbor_option* op;
	operation_path* next;
};




operation_path* dfs_search(solution* current, solution* destination, itemizer& items,
		const neighbor_option *op,
		int remaining_dist, int clength, int& best_length,
		tabu_list& l, solution* draw, viewer& v, double worst_cost)
{
	if (clength >= best_length)
	{
		return nullptr;
	}

	if (!op->in_bounds(current))
	{
		return nullptr;
	}

	solution intermediate{*current};
	op->apply(&intermediate);

	{
		hash h = get_hash(&intermediate);
		if (l.is_tabu(h))
		{
			return nullptr;
		}
		l.mark_tabu(h);
	}

	int ncost = distance_between_sequence(&intermediate, destination);
	if (ncost == 0)
	{
		std::cout << "Found one path with length " << clength << std::endl;

		// We found it!
		best_length = clength;
		return new operation_path{op, nullptr};
	}

	if (ncost >= remaining_dist)
	{
		// Moving away :(
		if (intermediate.get_cost() > 3 * worst_cost)
		{
			return nullptr;
		}
	}


	{
		for (int i=0; i<clength; i++)
		{
			std::cout << ' ';
		}

		std::cout << ncost << std::endl;
		(*draw) = intermediate;
		v.update();
	}

	// We are getting closer, take the best option from here...
	operation_path* best_path = nullptr;

	int num = items.get_num_options();
	for (int i=0; i<num; i++)
	{
		operation_path* p = dfs_search(&intermediate, destination, items,
				items.get_option(i),
				ncost, clength+1,
				best_length, l, draw, v, worst_cost);

		if (p != nullptr)
		{
			delete best_path;
			best_path = p;
		}
	}

	if (best_path != nullptr)
	{
		return new operation_path{op, best_path};
	}
	return nullptr;
}
















void link_paths(solution* source, solution* dest, itemizer& items, std::function<void(void)> callback)
{
	double worst_cost = std::max(source->get_cost(), dest->get_cost());

	int blength = INT_MAX;
	operation_path* shortest_path = nullptr;

	solution draw {source->get_city()};
	viewer v{&draw, "current_link"};

	bloom_filter blf{1000000};

	int d = distance_between_sequence(source, dest);

	int num_options = items.get_num_options();
	for (int i=0; i<num_options; i++)
	{
		operation_path* p = dfs_search(source, dest, items, items.get_option(i), d, 1, blength, blf, &draw, v, worst_cost);
		if (p != nullptr)
		{
			delete shortest_path;
			shortest_path = p;
		}
	}

	delete shortest_path;
}



















#if 0

solution intermediate{source->get_city()};

int rem = distance_between(source, dest);
do
{
	const neighbor_option* best_option = nullptr;
	int best_cost = INT_MAX;

	int num = items.get_num_options();
	for (int i=0; i<num; i++)
//		for (int t=0; t<100; t++)
	{
//			int i = rand() % num;
		const neighbor_option* alternative = items.get_option(i);
		if (!alternative->in_bounds(source))
		{
			continue;
		}

		intermediate = (*source);
		alternative->apply(&intermediate);

		int delta = distance_between(&intermediate, dest);
		if (delta <= rem)
		{
			// Only get closer
			continue;
		}

		if (delta < best_cost)
		{
			best_option = alternative;
			best_cost = delta;
		}
	}

	if (best_option == nullptr)
	{
		std::cout << "Could not get closer to the destination.";
		return;
	}

	std::cout << "Best operation " << *best_option << std::endl;
	std::cout << " has distance " << distance_between(&intermediate, dest) << " from the destination.";
	std::cin.ignore();

	best_option->apply(source);
	rem = best_cost;
	callback();
} while (rem > 0);
#endif
