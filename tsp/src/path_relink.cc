#include "path_relinking.h"
#include "limits.h"
#include "viewer.h"


int distance_between_positioned(const solution* sol1, const solution *sol2)
{
	 int sum = 0;

	 const int n = sol1->get_city()->num_stops;

	 for (int i=0; i<n; i++)
	 {
		int stop1 = sol1->get_stop(i);
		int stop2 = sol2->get_stop(i);
		if (stop1 != stop2)
		{
			sum++;
		}
	 }

	 return sum;
}

int distance_between_sequence(const solution* sol1, const solution *sol2)
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

int distance_between_sequence_no_order(const solution* sol1, const solution *sol2)
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

		int ndx1 = sol2->get_index_of_stop(stop1);
		int ndx2 = sol2->get_index_of_stop(stop2);

		if (ndx1 != i)
		{
			sum++;
		}


		if (ndx1 < 0 || ndx2 < 0)
		{
			sum++;
			continue;
		}

		if (ndx2 == ndx1 + 1)
		{
			continue;
		}
		if (ndx2 == ndx1 - 1)
		{
			continue;
		}

		sum++;
	 }

	 if (sum == 0)
	 {
		 for (int i=0;i<n;i++)
		 {
			 if (sol1->get_stop(i) != sol2->get_stop(i))
			 {
				 return 1;
			 }
		 }
		 return 0;
	 }
	 else
	 {
		 return 1 + sum;
	 }
}


class opt
{
public:
	opt(const neighbor_option *o, const solution& other, const solution* dest, int l, opt* p) :
		op{o},
		sol{other},
		cost{0},
		length{l},
		prev{p}
	{
		op->apply(&sol);
		cost = distance_between_sequence(&sol, dest);
	}
	~opt() {}

	void print(viewer &v)
	{
		if (prev != nullptr)
		{
			prev->print(v);
		}
		std::cout << *op << std::endl;
		v.show(&sol);
		v.update();
//		v.pause(500);
	}


	const neighbor_option *op;
	solution sol;
	int cost;
	int length;
	opt* prev;
};

operation_path* dfs_search(opt* current, solution* destination, itemizer& items,
		int& best_length, double worst_cost,
		solution* draw, viewer& v)
{
	{
		for (int i=0; i<current->length; i++)
		{
			std::cout << ' ';
		}

		std::cout << current->cost << std::endl;
		(*draw) = (current->sol);
		v.update();
	}

	if (current->cost == 0)
	{
		std::cout << "Found one path with length " << current->length << std::endl;
		std::cout << "Path = \n";
//		std::cin.ignore();
		current->print(v);

		// We found it!
		best_length = current->length;
		return new operation_path{current->op, nullptr};

		v.pause();
		exit(-1);
	}

	if (current->length + 1 >= best_length)
	{
		std::cout << "hit max depth" << std::endl;
		return nullptr;
	}

	std::list<opt*> good_options;

	int num = items.get_num_options();

	for (int i=0; i<num; i++)
//	for (int t=0; t<100; t++)
	{
//		int i = rand() % num;
		const neighbor_option* o = items.get_option(i);
		if (!o->in_bounds(&current->sol))
		{
			continue;
		}

		opt* anopt = new opt{o, current->sol, destination, current->length + 1, current};
		if (anopt->cost >= current->cost)
		{
			// Moving too far away :(
			if (current->sol.get_cost() > 1.0 * worst_cost)
			{
//				delete anopt;
//				continue;
			}
			delete anopt;
			continue;
		}
		good_options.push_back(anopt);
	}

	good_options.sort([](const opt *o1, const opt* o2)
	{
//		return o1->sol.get_cost() < o2->sol.get_cost();
		return o1->cost < o2->cost;
	});

	if (good_options.size() == 0)
	{
		// We should never run out of ways to improve if we have not hit the destination!
		std::cout << "Ran out of improvements from \n" << current->sol << "\nto\n" << *destination << std::endl;
		trap();
	}


	bool skip = false;

	operation_path* best_path = nullptr;
	auto end = good_options.end();
	for (auto it = good_options.begin(); it != end; ++it)
	{
		operation_path* p = nullptr;

//		if (skip)
//		{
//			delete (*it);
//			continue;
//		}

		p = dfs_search(*it, destination, items,
				best_length, worst_cost,
				draw, v);

		if (p != nullptr)
		{
			delete best_path;
			best_path = p;

			if (!skip &&
					current->prev != nullptr &&
					current->prev->cost < current->cost + 2)
			{
				skip = true;
			}
		}

		delete *it;
	}

	if (best_path != nullptr)
	{
		return new operation_path{current->op, best_path};
	}
	return nullptr;
}


void link_paths(solution* source, solution* dest, itemizer& items, std::function<void(void)> callback)
{
	solution draw {source->get_city()};
	viewer v{&draw, "current_link"};

	double worst_cost = std::max(source->get_cost(), dest->get_cost());
	int blength = source->get_city()->num_stops;

	neighbor_option* sent_opt = new nothing_option;
	opt* sent = new opt{sent_opt, *source, dest, 0, nullptr};

	operation_path* shortest_path = dfs_search(sent, dest, items, blength, worst_cost, &draw, v);

	delete sent;
	delete sent_opt;

	delete shortest_path;
}






void fastest_descent(solution* source, solution* dest, itemizer& items, std::function<void(void)> callback)
{
	solution intermediate{source->get_city()};

	std::cout << "going from\n" << (*source) << "\nto\n" << (*dest) << std::endl;


	viewer myviewer{source, "approaching"};
	viewer myinnerviewer{&intermediate, "temp"};

	int (*const dist_func)(const solution*s1, const solution*s2) =
//			&distance_between_sequence_no_order;
			&distance_between_positioned;

	int rem = dist_func(source, dest);
	do
	{
		const neighbor_option* best_option = nullptr;
		int best_cost = rem;

		int num = items.get_num_options();
		for (int i=0; i<num; i++)
		{
			const neighbor_option* alternative = items.get_option(i);
			if (!alternative->in_bounds(source))
			{
				std::cout << "This shouldn't happen." << std::endl;
				continue;
			}

			intermediate = (*source);

//			bool goodone = alternative->get_name() == "swap" &&
//					intermediate.get_index_of_stop(alternative->stop1) == 0 &&
//					intermediate.get_index_of_stop(alternative->stop2) == 9;

			alternative->apply(&intermediate);
			myinnerviewer.update();
			int newdist = dist_func(&intermediate, dest);
			if (newdist >= best_cost)
			{
				continue;
			}

			best_option = alternative;
			best_cost = newdist;
		}

		if (best_option == nullptr)
		{
			std::cout << "Could not get closer to the destination.";
			return;
		}

		best_option->apply(source);

//		std::cout << "Best operation " << *best_option << std::endl;
//		std::cout << "it leaves solution " << (*source) << std::endl;
//		std::cout << "it is distance " << dist_func(source, dest) << "\nfrom:\n" << (*dest) << std::endl;

		rem = best_cost;
		myviewer.update();
		myviewer.pause();
//		callback();
	} while (rem > 0);
}








int get_match_length(solution* s1, solution* s2, int idx1, int d)
{
	int n = s1->get_city()->num_stops;

	int stop = s1->get_stop(idx1);
	int idx2 = s2->get_index_of_stop(stop);

	int count = 0;

	while (0 <= idx1 && idx1 < n && 0 <= idx2 && idx2 <= n)
	{
		if (s1->get_stop(idx1) != s2->get_stop(idx2))
		{
			break;
		}
		idx1 += d;
		idx2 += d;
		count++;
	}

	return count;
}




void get_there(solution* s1, solution *s2)
{
	viewer v{s2, "inter"};

	int count = 0;

	int n = s1->get_city()->num_stops;
	for (int i=0; i<n; i++)
	{
		int stop = s1->get_stop(i);
		int idx2 = s2->get_index_of_stop(stop);
		if (idx2 == i)
		{
			continue;
		}

//		std::cout << "====================================================================" << std::endl;
//		std::cout << *s1 << std::endl;
//		std::cout << *s2 << std::endl;

//		std::cout << stop << ", " << idx2 << std::endl;

		int m1 = get_match_length(s1, s2, i,  1);
		int m2 = get_match_length(s1, s2, i, -1);

//		std::cout << m1 << ", " << m2 << std::endl;

		if (m1 > m2)
		{
			// swap from i to i + m1
			two_opt op{
				stop,
				s2->get_stop(idx2 + m1 - 1),
				s2->get_city()
			};
			op.apply(s2);

//			std::cout << *s2 << std::endl;
			v.update();
			v.pause(500);

			count++;
		}

		two_opt op{
			s2->get_stop(i),
			stop,
			s2->get_city()
		};

		op.apply(s2);
//		std::cout << *s2 << std::endl;
		v.update();
		v.pause(500);
		count++;
	}

	std::cout << "This took " << count << " steps." << std::endl;
}
