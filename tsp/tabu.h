#ifndef TABU_H_
#define TABU_H_


#include "solution.h"


class tabu_option
{
public:
	tabu_option(int stop1, int stop2, city* c);
	virtual ~tabu_option();
	

	virtual double get_improvement(solution* sol) = 0;
	virtual bool in_bounds(solution* sol) = 0;
	virtual void apply(solution* sol) = 0;
	
	int stop1, stop2;
	double dist;
	
	tabu_option* next;
};

// This represents the action of reversing all stops from stop1 to stop2
class swap_tabu_option : public tabu_option
{
public:
	swap_tabu_option(int stop1, int stop2, city* c);
	~swap_tabu_option();
	
	double get_improvement(solution* sol);
	bool in_bounds(solution* sol);
	void apply(solution* sol);
	
	double get_cost_along_path(solution* sol);
	bool intersects(solution* sol);
};

// This represents the action of rescheduling stop1 to come after stop2
class reschedule_tabu_option : public tabu_option
{
public:
	reschedule_tabu_option(int stop1, int stop2, city* c);
	~reschedule_tabu_option();
	
	double get_improvement(solution* sol);
	bool in_bounds(solution* sol);
	void apply(solution* sol);
};

// This represents all the actions that could be performed on a city (whether they be good, tabu, or bad).
class tabu_options
{
public:
	tabu_options();
	~tabu_options();
	
	tabu_option* get_random_option(solution* sol);
	tabu_option* get_best_option(solution* sol);
	
	swap_tabu_option* swap_options;
	reschedule_tabu_option* reschedule_options;
};

tabu_options* generate_tabu_options(city* c);




#endif