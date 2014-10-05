
#include "routines.h"
#include "viewer.h"
#include "slvr.h"

#include <iostream>
#include <thread>
#include <algorithm>


void run_exact(city*c)
{
	optimizer *o = exact(c);
	std::cout << "E: cost = " << o->best.get_cost() << std::endl;
	delete o;
}


void neighbor_search(city* c, int num)
{
	std::stringstream s;
	s << "2opts" << num;
	optimizer *opt = new optimizer{s.str(), c, 100};
	opt->best.random();
	
	int count = 0;
	double locality = 1.0;
	
	int outerphase = 100000;

	while (count++ < outerphase)
	{
		locality *= 1.05;
		if (locality > 1.0)
		{
			locality = .05;
		}
		
		if (rand() % 3
			|| (!(rand() % 3) && !find_neighbor_2op_inter(&opt->best, &opt->current, INT_MAX)))
		{
			find_neighbor_2op(&opt->best, &opt->current, locality);
		}
		else if (rand() % 3)
		{
			find_neighbor_resched(&opt->best, &opt->current, .25);
		}
		else
		{
			find_neighbor_2pts(&opt->best, &opt->current, locality);
		}
		if (opt->offer())
		{
			count = 0;
		}
		opt->best.is_valid();
		opt->current.is_valid();
	}
	
	//delete opt;
}


#if 0

void neighbor_search(city* c, int num)
{
	std::stringstream s;
	s << "2opts" << num;
	optimizer *opt = new optimizer{s.str(), c, 100};
	opt->best.random();
	
	int count = 0;
	double locality = 1.0;
	
	int outerphase = 100000;
	int innnerphase = 10000;

	while (count++ < outerphase)
	{
		int icount;
		
		locality *= 1.05;
		if (locality > 1.0)
		{
			locality = .05;
		}
		
		icount = 0;
		while (icount++ < innnerphase)
		{
			find_neighbor_2op(&opt->best, &opt->current, locality);
			if (opt->offer())
			{
				icount = 0;
			}
		}
		opt->best.is_valid();
		opt->current.is_valid();
		std::cout << "phase" << std::endl;
		
		icount = 0;
		while (icount++ < innnerphase)
		{
			if (!find_neighbor_2op_inter(&opt->best, &opt->current, INT_MAX))
			{
				break;
			}
			if (opt->offer())
			{
				icount = 0;
			}
		}
		opt->best.is_valid();
		opt->current.is_valid();
		std::cout << "phase" << std::endl;
		
		icount = 0;
		while (icount++ < innnerphase)
		{
			find_neighbor_resched(&opt->best, &opt->current, .25);
			if (opt->offer())
			{
				icount = 0;
			}
		}
		opt->best.is_valid();
		opt->current.is_valid();
		std::cout << "phase" << std::endl;
		
		icount = 0;
		while (icount++ < innnerphase)
		{
			find_neighbor_2pts(&opt->best, &opt->current, locality);
			if (opt->offer())
			{
				icount = 0;
			}
		}
		
		opt->best.is_valid();
		opt->current.is_valid();
		std::cout << "phase" << std::endl;
	}
	
	//delete opt;
}
#endif

void test_intersect();

int main(int argc, char **argv)
{
	srand(time(nullptr));
	
//	test_intersect();
	
	if (argc < 2)
	{
		std::cout << "need to give a city size." << std::endl;
		return -1;
	}
	
	int num_cities = -1;
	num_cities = atoi(argv[1]);
	if (num_cities <= 0)
	{
		std::cout << "need more cities." << std::endl;
		return -2;
	}
	
	city c{CLUSTER_CITY_TYPE, num_cities};

	std::thread e {run_exact, &c};
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	
	std::vector<std::thread*> threads;
	for (int i=0;i<1;i++)
	{
		threads.push_back(new std::thread{neighbor_search, &c, i});
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	
	{
		solution* sol = shortest_path(&c);
		std::cout << "H: cost = " << sol->get_cost() << std::endl;
		delete sol;
	}
	
	e.join();
	std::for_each(threads.begin(), threads.end(), [](std::thread *t) {t->join(); delete t;});
	
	cv::waitKey(1);
	
	return 0;
}
