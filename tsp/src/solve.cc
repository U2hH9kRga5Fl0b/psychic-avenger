
#include "routines.h"
#include "viewer.h"
#include "slvr.h"
#include "subdivider.h"
#include "viewer.h"
#include "item.h"
#include "count_minima.h"
#include "common.h"

#include <iostream>
#include <thread>
#include <algorithm>



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void breakpointhere(const std::string& msg)
{
	std::cout << "on the way out!" << std::endl;
	exit(-1);
}



























































////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void subdivide_base_case(solution* othersol)
{
	std::stringstream s;
	s << "subdivide " << rand();
	viewer v{othersol, "basecase"};
	itemizer itms{othersol->get_city()};
	itms.greedy_search(othersol, [&v](){v.update();});
}




































////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void run_tests(city* c)
{
	// run test intersect...
	// test exact
	void test_local_search(solution *sol, int tests_per_phase);
	solution sol{c};
	sol.random();
	test_local_search(&sol, 1);
	
	std::cout << "Tests pass!" << std::endl;
}





































////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace
{
	std::chrono::system_clock::time_point start_time;

void print_time(const std::string& name="unnamed")
{
	std::cout << "for '" << name << "' time=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time).count() <<  " milliseconds." << std::endl;
}

void show_final_solution(solution* sol, const std::string& name)
{
	print_time(name);
	
#if GRAPHICS
	viewer v{sol, name};
	v.update();
	v.pause();
#else
	std::cout << *sol << std::endl;
#endif
	delete sol;
}


void print_usage(int argc, char **argv)
{
	std::cout << "usage: " << argv[0] << " ("
		<< "  (a|anneal)"
		<< "| (e|exact) "
		<< "| (g|grower)"
		<< "| (h|nearest)"
		<< "| (i|improvements)"
		<< "| (n|neighborhood)"
		<< "| (p|print_stats)"
		<< "| (r|random)"
		<< "| (s|subdivide)"
		<< "| (t|tabu)"
		<< "| (v|verify)"
		<< "| (0|nothing)"
		<< ") <city file>" << std::endl;
	trap();
}

void seed(solution *sol, viewer& v)
{
	
#if 0
		sol->random();
#else
#if 1
		sol->empty();
		grow(sol, [&v](){v.update();});
#else
		sol->nearest([&v](){v.update();});
#endif
#endif
	
}

}

int main(int argc, char **argv)
{
//	srand(time(nullptr));
	srand(5000014);
//	test_intersect();
	
	if (argc != 3)
	{
		print_usage(argc, argv);
	}
	
	city* c;
	
	{
		std::ifstream infile {std::string{argv[2]}};
		if (!infile.is_open())
		{
			std::cout << "Unable to open city file!" << std::endl;
			std::cout << "File = " << argv[2] << std::endl;
			print_usage(argc, argv);
		}
		c = load_city(infile);
		if (c == nullptr)
		{
			print_usage(argc, argv);
		}
	}
	
	init_hash_key(c->num_stops);
	
	start_time = std::chrono::system_clock::now();
	
	std::string algo{argv[1]};
	if (algo == "e" || algo == "exact")
	{
		std::cout << "Finding exact solution." << std::endl;
		show_final_solution(exact(c), "exact");
	}
	else if (algo == "g" || algo == "grower")
	{
		solution* sol = new solution{c};
		viewer v{sol, "growing"};
		sol->empty();
		grow(sol, [&v](){v.update();});
		show_final_solution(sol, "growen");
	}
	else if (algo == "h" || algo == "nearest")
	{
		std::cout << "Using greedy shortest next path algorithm." << std::endl;
		solution* sol = new solution{c};
		{
			viewer v{sol, "nearest"};
			sol->nearest([&v](){v.update();});
			std::cout << "cost = " << sol->get_cost() << std::endl;
			std::cout << *sol << std::endl;
		}
		show_final_solution(sol, "heuristic");
	}
	else if (algo == "s" || algo == "subdivide")
	{
		solution *sol = subdivide_backtrack(c, subdivide_base_case, 500);
		show_final_solution(sol, "subdivide");
	}
	else if (algo == "n" || algo == "neighborhood")
	{
		solution *sol = new solution{c};
		viewer v{sol, "current neighbor"};
		seed(sol, v);
		itemizer itms{c};
		itms.greedy_search(sol, [&v]() {v.update();});
		show_final_solution(sol, "neighborhood");
	}
	else if (algo == "t" || algo == "tabu")
	{
		solution* sol = new solution{c};
		viewer v{sol, "smpl tabu"};
		seed(sol, v);
		itemizer options{c};
		options.ordered_search(sol, [&v](){v.update();});
		show_final_solution(sol, "tabu");
	}
	else if (algo == "r" || algo == "random")
	{
		std::cout << "Using random algorithm" << std::endl;
		solution* sol = new solution{c};
		viewer v{sol, "random"};
		sol->random([&v](){v.update();});
		std::cout << "cost = " << sol->get_cost() << std::endl;
		show_final_solution(sol, "random");
	}
	else if (algo == "0" || algo == "nothing")
	{
		
#if GRAPHICS
		view_city(c, "city");
#else
		std::cout << "Graphics are currently disabled." << std::endl;
#endif
	}
	else if (algo == "a" || algo == "anneal")
	{
		solution *sol = new solution{c};
		int ann = sol->get_city()->num_stops;
		anneal(sol, 5, .0001, ann, ann / 2);
		
		show_final_solution(sol, "annealed");
	}
	else if (algo == "i" || algo == "improvements")
	{
		solution sol{c};
		sol.random();
		itemizer options{sol.get_city()};
		
		const neighbor_option* o;
		while ((o = options.get_best_option(&sol)) != nullptr)
		{
			options.print_improvements(&sol);
			options.sort(&sol);

			o->apply(&sol);
		}
	}
	else if (algo == "v" || algo == "verify")
	{
		run_tests(c);
		print_time();
	}
	else if (algo == "p" || algo == "print_stats")
	{
		
		complete_decision_space(c);
		print_time();
	}
	else
	{
		print_usage(argc, argv);
	}
	
	destroy_hash_key(c->num_stops);
	
	delete c;
	
	return 0;
}
