#include "path_relinking.h"
#include "limits.h"
#include "viewer.h"

#include <sstream>

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

namespace
{


constexpr int PRINT_IF_LESS_THAN = 50;

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

#if 0

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



#endif




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


void print_step(int ndx1, int ndx2, solution* s1)
{
	if (ndx1 > ndx2)
	{
		int tmp = ndx1;
		ndx1 = ndx2;
		ndx2 = tmp;
	}

	int n = s1->get_city()->num_stops;
	std::cout << std::setw(2) << ndx1 << ", " << std::setw(2) << ndx2 << ": ";
	for (int i=0; i<n; i++)
	{
		if (i == ndx1)
		{
			std::cout << "{" << std::setw(2) << s1->get_stop(i) << " ";
		}
		else if (i == ndx2)
		{
			std::cout << std::setw(2) << s1->get_stop(i) << "} ";
		}
		else
		{
			std::cout << " " << std::setw(2) << s1->get_stop(i) << " ";
		}
	}
	std::cout << std::endl;
}

int match_first_after(solution *dst, solution* src, std::function<void(const solution*s)> f, int ndx, int m, int dir)
{
	int n = dst->get_city()->num_stops;
	int stop = dst->get_stop(ndx);
	int idx2 = src->get_index_of_stop(stop);
	if (idx2 == ndx)
	{
		return 0;
	}

	int count = 0;
	if (dir > 0)
	{
		// swap from i to i + m1
		two_opt op{
			stop,
			src->get_stop(idx2 + m - 1),
			src->get_city()
		};
		op.apply(src);

		if (n < PRINT_IF_LESS_THAN)
		{
			print_step(idx2, idx2 + m - 1, src);
		}

//		f(src);
		count++;
	}

	two_opt op{
		src->get_stop(ndx),
		stop,
		src->get_city()
	};

	op.apply(src);
	f(src);

	if (n < PRINT_IF_LESS_THAN)
	{
		print_step(ndx, idx2, src);
	}
	return count + 1;
}

int match_first_after(solution *dest, solution*sorc, std::function<void(const solution*s)> f, int ndx)
{
	int stop = dest->get_stop(ndx);
	int idx2 = sorc->get_index_of_stop(stop);
	if (idx2 == ndx)
	{
		return 0;
	}

	int m1 = get_match_length(dest, sorc, ndx,  1);
	int m2 = get_match_length(dest, sorc, ndx, -1);

	if (m1 > m2)
	{
//		std::cout << "matching " << m1 << std::endl;
		return match_first_after(dest, sorc, f, ndx, m1, 1);
	}
	else
	{
//		std::cout << "matching " << m2 << std::endl;
		return match_first_after(dest, sorc, f, ndx, m2, -1);
	}
}




int find_a_good_one(solution *src, solution *dest, std::function<void(const solution*s)> f, int ignore_up_to)
{
	int n = src->get_city()->num_stops;

	int idxmax = -1;
	int longest_match = -1;
	int dir = 0;

	for (int i = ignore_up_to; i < n; i++)
	{
		if (src->get_stop(i) == dest->get_stop(i))
		{
			continue;
		}

		int m1 = get_match_length(src, dest, i, 1);
		if (m1 > longest_match)
		{
			dir = 1;
			longest_match = m1;
			idxmax = i;
		}
		int m2 = get_match_length(src, dest, i, -1);
		if (m2 > longest_match)
		{
			dir = -1;
			longest_match = m2;
			idxmax = i;
		}
	}
	if (idxmax < 0)
	{
		return 0;
	}

	if (longest_match < 5)
	{
		return 0;
	}

	std::cout << "found match of length " << longest_match << std::endl;
	return match_first_after(dest, src, f, idxmax, longest_match, dir);
}




}


#if 1

int move_from_to(solution* source, solution *dest, std::function<void(const solution*s)> f)
{
	int n = source->get_city()->num_stops;

	solution tmp { *source };

	std::stringstream s;
	s << "inter_" << rand();
//	viewer v{s.str()};


	int nbeg = 100;
	if (n < PRINT_IF_LESS_THAN)
	{
//		v.snapshot("output/snapshot_start.png");

//		for (int i = 0; i < nbeg; i++)
//		{
//			v.update();
//		}

		print_step(-1, -1, dest);
		print_step(-1, -1, &tmp);
	}


	int sum = 0;

	for (int i=0; i<n; i++)
	{
		if (tmp.get_stop(i) == dest->get_stop(i))
		{
			continue;
		}
#if 0
		find_a_good_one(&tmp, dest, f, i);
#endif
		sum += match_first_after(dest, &tmp, f, i);
//		v.update(&tmp);
	}

//	if (n < PRINT_IF_LESS_THAN)
//	{
//		for (int i=0; i<nbeg; i++)
//		{
//			v.update();
//		}
//
//		v.snapshot("output/snapshot_stop.png");
//	}

//	std::cout << "distance = " << sum << std::endl;

	return sum;
}



#else

int move_from_to(solution* source, solution *dest, std::function<void(const solution*s)> f)
{
	int n = source->get_city()->num_stops;

	solution tmp1 { *source };
	solution tmp2 { *source };

	city* ci = source->get_city();

	int sum = 0;
	int mindist = INT_MAX;

	viewer v1{"alt"};
	viewer v2{"best_alt"};
	viewer v3{"current"};

	while (mindist != 0)
	{
		int min1, min2, min3;
		min1 = min2 = min3 = -1;

//		for (int i=0;i<n;i++)
//		for (int j=0;j<n;j++)
//		for (int k=0;k<n;k++)
		while(true)
		{
			int i = rand() % n;
			int j = rand() % n;
			int k = rand() % n;

			tmp2 = tmp1;
			two_opt{
				i,
				j,
				nullptr
			}.apply(&tmp2);
			two_opt{
				j,
				k,
				nullptr
			}.apply(&tmp2);
//			v1.update(&tmp2);

			int dist = distance_between_sequence_no_order(&tmp2, dest);
			if (dist < mindist)
			{
				dist = mindist;
				min1 = i;
				min2 = j;
				min3 = k;

				v2.update(&tmp2);
				break;
			}
		}

		if (min1 < 0)
		{
			std::cout << "Could not get closer!" << std::endl;
			trap();
		}

		tmp2 = tmp1;
		two_opt{
			min1,
			min2,
			nullptr
		}.apply(&tmp1);
		two_opt{
			min2,
			min3,
			nullptr
		}.apply(&tmp1);

		v3.update(&tmp1);
	}

	return sum;
}






#endif













